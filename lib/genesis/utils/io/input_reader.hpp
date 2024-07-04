#ifndef GENESIS_UTILS_IO_INPUT_READER_H_
#define GENESIS_UTILS_IO_INPUT_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/options.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/threading/thread_pool.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <stdexcept>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     Input Reader
// =================================================================================================

class AsynchronousReader;
class SynchronousReader;

/**
 * @brief Alias for the either AsynchronousReader or SynchronousReader.
 *
 * This typedef is an alias for AsynchronousReader. We use it in order to be able to easily switch
 * to using SynchronousReader, by aliasing that instead. However, as of now, we always use
 * the AsynchronousReader throughout all reading functionality, for efficiency reasons.
 */
using InputReader = AsynchronousReader;

// =================================================================================================
//     Asynchronous Reader
// =================================================================================================

/**
 * @brief Read bytes from an @link BaseInputSource InputSource@endlink into a `char buffer`.
 *
 * The reading is done asynchronously, that is, in another thread. This is usually faster than
 * synchronous reading (see SynchronousReader), particularly for large data blocks, because we can
 * avoid blocking any computational threads for I/O wait times. It is thus the preferred reader,
 * if available.
 *
 * In detail, if we have a trival input source, such as just reading data from a file, it get its
 * own thread pool with one thread, which boils down to just a thread and condition variable for
 * notifying the thread, and hence is a realative lightweight way of not blocking any computational
 * threads with I/O wait times. For more complex inputs, such as gzip that needs decompression,
 * and hence is not a trivial I/O wait, we instead use a given or the global thread pool, to avoid
 * oversubscribing our desired thread count, and spamming too many async reader threads when
 * multiple files need to be processed at the same time.
 *
 * The caller is responsible for keeping the data buffer alive while the reading is happening.
 * That is, calling start_reading() without then also calling finish_reading() and having the
 * buffer go out of scope could lead to a segfault. Don't do that. Also, each call to start_reading()
 * needs to be matched by a call to finish_reading() before calling start_reading() again, as
 * otherwise, the input data will get scrambled.
 *
 * Implementation details inspired by
 * [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) by Ben Strasser,
 * see also @link supplement_acknowledgements_code_reuse_input_stream Acknowledgements@endlink.
 */
class AsynchronousReader
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    AsynchronousReader(
        std::shared_ptr<BaseInputSource> input_source,
        std::shared_ptr<ThreadPool> thread_pool = nullptr
    ) {
        if( !input_source ) {
            throw std::runtime_error( "Cannot read from BaseInputSource nullptr" );
        }

        // Depending on the async policy (see below), we might want to use a thread pool for the
        // reading - either the given one, or if none is given, the global one. We use a lambda
        // for that decision to avoid code duplcation below, but also being able to ignore the
        // global thread pool if we do not need it. This is relevant when we do not want to use
        // it, and init_global_thread_pool() has not been called.
        auto get_thread_pool_ = [&]()
        {
            if( thread_pool ) {
                return thread_pool;
            }
            return Options::get().global_thread_pool();
        };

        // Set the threads according to the global options policy for file reading.
        // By default, for trivial input such as reading a simple file, use our own thread.
        // This makes sure that simple file I/O does not need to wait in the thread pool
        // to be executed, but gets spawned in its own thread that can operate idependently
        // of the pool, and just wait for the I/O to be done, without blocking any other
        // threads while waiting.
        // For complex sources, we use the given or global thread pool instead, for instance
        // when decompressing gzipped files. That means that internally, for such sources,
        // the actual I/O for getting the data that needs to be decompressed is blocking
        // that thread until I/O is completed. We could add another layer of data fetching
        // to avoid that, and load the gzipped data async into some other buffer first,
        // but well, decompression probably takes up enough time for that to not matter too much.
        switch( Options::get().input_reading_thread_policy() ) {
            case Options::InputReadingThreadPolicy::kStrict: {
                thread_pool_ = get_thread_pool_();
                break;
            }
            case Options::InputReadingThreadPolicy::kTrivialAsync: {
                if( input_source->is_trivial() ) {
                    thread_pool_ = std::make_shared<utils::ThreadPool>( 1 );
                } else {
                    thread_pool_ = get_thread_pool_();
                }
                break;
            }
            case Options::InputReadingThreadPolicy::kAllAsync: {
                thread_pool_ = std::make_shared<utils::ThreadPool>( 1 );
                break;
            }
        }
        input_source_ = input_source;
    }

    AsynchronousReader( AsynchronousReader const& ) = delete;
    AsynchronousReader( AsynchronousReader&& )      = delete;

    AsynchronousReader& operator= ( AsynchronousReader const& ) = delete;
    AsynchronousReader& operator= ( AsynchronousReader&& )      = delete;

    ~AsynchronousReader() = default;

    // -------------------------------------------------------------
    //     Init and General Members
    // -------------------------------------------------------------

    bool valid() const
    {
        return input_source_ != nullptr;
    }

    std::shared_ptr<BaseInputSource> input_source() const
    {
        return input_source_;
    }

    std::string class_name() const
    {
        return "AsynchronousReader";
    }

    // -------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------

    void start_reading( char* target_buffer, size_t target_size )
    {
        // We only assert the validity of the buffer here, as it is a user error to not provide
        // a large enough buffer here.
        assert( target_buffer );

        // The function shall only ever be called once in a row, followed by finish_reading().
        // Not doing that is a user error, so we here just assert that here.
        assert( ! future_.valid() );

        // We need a local copy of the input source here, as C++11 does not allow to capture
        // class member variables by value...
        auto input_source = input_source_;

        // We capture the target by value, meaning that the caller has to stay alive until the
        // task is finished, so that we don't get a memory access violation for the buffer.
        future_ = thread_pool_->enqueue_and_retrieve(
            [=](){
                return input_source->read( target_buffer, target_size );
            }
        );
    }

    size_t finish_reading()
    {
        // Same as above for start_reading(), we here only assert the correct order of execution.
        assert( future_.valid() );

        // Now get the future, which blocks until the data is actually there.
        // This also re-throws any errors that might have occurred during executing,
        // see https://stackoverflow.com/q/14222899/4184258
        return future_.get();
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    // Where to read from.
    std::shared_ptr<BaseInputSource> input_source_;

    // Thread pool to run the reading in the background.
    std::shared_ptr<ThreadPool> thread_pool_;

    // Future that stores the achieved size of how many bytes were red.
    // If we ever want to make this class moveable, this probably needs to live in a shared_ptr.
    ProactiveFuture<size_t> future_;

};

// =================================================================================================
//     Synchronous Reader
// =================================================================================================

/**
 * @brief Read bytes from an @link BaseInputSource InputSource@endlink into a `char buffer`.
 *
 * The reading is done synchronously, that is, reading occurs on request. This is usually slower
 * than asynchronous reading (see AsynchronousReader).
 *
 * Implementation details inspired by
 * [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) by Ben Strasser,
 * see also @link supplement_acknowledgements_code_reuse_input_stream Acknowledgements@endlink.
 */
class SynchronousReader
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SynchronousReader(
        std::shared_ptr<BaseInputSource> input_source
    )
        : input_source_( input_source )
    {}

    ~SynchronousReader() = default;

    SynchronousReader( SynchronousReader const& ) = delete;
    SynchronousReader( SynchronousReader&& )      = default;

    SynchronousReader& operator= ( SynchronousReader const& ) = delete;
    SynchronousReader& operator= ( SynchronousReader&& )      = default;

    // -------------------------------------------------------------
    //     Init and General Members
    // -------------------------------------------------------------

    bool valid() const
    {
        return input_source_ != nullptr;
    }

    std::shared_ptr<BaseInputSource> input_source() const
    {
        return input_source_;
    }

    std::string class_name() const
    {
        return "SynchronousReader";
    }

    // -------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------

    void start_reading( char* target_buffer, size_t target_size )
    {
        target_buffer_ = target_buffer;
        target_size_   = target_size;
    }

    size_t finish_reading()
    {
        return input_source_->read( target_buffer_, target_size_ );
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    std::shared_ptr<BaseInputSource> input_source_;

    char* target_buffer_;
    size_t target_size_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
