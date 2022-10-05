#ifndef GENESIS_UTILS_IO_INPUT_READER_H_
#define GENESIS_UTILS_IO_INPUT_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/thread_pool.hpp"
#include "genesis/utils/io/input_source.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     Input Reader
// =================================================================================================

#ifdef GENESIS_PTHREADS

    class AsynchronousReader;

    /**
     * @brief Alias for the either AsynchronousReader or SynchronousReader, depending on the
     * threading setting.
     *
     * This typedef is an alias for AsynchronousReader, if threading is available, that is,
     * if the `GENESIS_PTHREADS` macro definition is set.
     * If not, it is an alias for SynchronousReader.
     *
     * Using this typedef instead of one of the two reader classes directly thus makes it possible
     * to ignore the `GENESIS_PTHREADS` setting when using them. It serves as an abstraction.
     * For example, InputStream uses the typedef this way.
     */
    using InputReader = AsynchronousReader;

#else

    class SynchronousReader;

    /**
    * @brief Alias for the either AsynchronousReader or SynchronousReader, depending on the
    * threading setting.
    *
    * This typedef is an alias for AsynchronousReader, if threading is available, that is,
    * if the `GENESIS_PTHREADS` macro definition is set.
    * If not, it is an alias for SynchronousReader.
    *
    * Using this typedef instead of one of the two reader classes directly thus makes it possible
    * to ignore the `GENESIS_PTHREADS` setting when using them. It serves as an abstraction.
    * For example, InputStream uses the typedef this way.
    */
    using InputReader = SynchronousReader;

#endif

// =================================================================================================
//     Asynchronous Reader
// =================================================================================================

#ifdef GENESIS_PTHREADS

/**
 * @brief Read bytes from an @link BaseInputSource InputSource@endlink into a `char buffer`.
 *
 * The reading is done asynchronously, that is, a second thread is started, or a thread pool is used.
 * This is usually faster than synchronous reading (see SynchronousReader), particularly for large
 * data blocks. It is thus the preferred reader, if available.
 *
 * The caller is responsible for keeping the data buffer alive while the reading is happening.
 * That is, calling start_reading() without then also calling finish_reading() and having the
 * buffer go out of scope could lead to a segfault. Don't do that.
 *
 * Note that we recommend using an individual ThreadPool of size 1 for using this class, which is
 * the default if no external thread pool is provided. We however also allow to set an external
 * thread pool, so that in cases where the number of spawned threads need to be limited, this
 * can be achieved. Not recommended though, as it will likely result in slowdown.
 *
 * This class is only available if threading is available, that is, if the `GENESIS_PTHREADS` macro
 * definition is set. If this is the case, the @link utils::InputReader InputReader@endlink
 * typedef is an alias of this class. Otherwise, only the SynchronousReader is available and
 * @link utils::InputReader InputReader@endlink aliases this one instead.
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

    AsynchronousReader( std::shared_ptr<utils::ThreadPool> thread_pool = {} )
    {
        if( thread_pool ) {
            thread_pool_ = thread_pool;
            assert( thread_pool_->size() > 0 );
        } else {
            thread_pool_ = std::make_shared<utils::ThreadPool>( 1 );
        }
    }

    AsynchronousReader( AsynchronousReader const& ) = delete;
    AsynchronousReader( AsynchronousReader&& )      = delete;

    AsynchronousReader& operator= ( AsynchronousReader const& ) = delete;
    AsynchronousReader& operator= ( AsynchronousReader&& )      = delete;

    ~AsynchronousReader() = default;

    // -------------------------------------------------------------
    //     Init and General Members
    // -------------------------------------------------------------

    void init( std::shared_ptr< BaseInputSource > input_source )
    {
        input_source_ = input_source;
    }

    bool valid() const
    {
        return input_source_ != nullptr;
    }

    BaseInputSource const* input_source() const
    {
        return input_source_.get();
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
        future_ = thread_pool_->enqueue(
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
    std::future<size_t> future_;

};

#endif

// =================================================================================================
//     Synchronous Reader
// =================================================================================================

/**
 * @brief Read bytes from an @link BaseInputSource InputSource@endlink into a `char buffer`.
 *
 * The reading is done synchronously, that is, reading occurs on request. This is usually slower
 * than asynchronous reading (see AsynchronousReader).
 *
 * This class is always available. If threading is not available (that is, if the `GENESIS_PTHREADS`
 * macro definition is not set), the @link utils::InputReader InputReader@endlink typedef is an
 * alias for this class.
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

    SynchronousReader()  = default;
    ~SynchronousReader() = default;

    SynchronousReader( SynchronousReader const& ) = delete;
    SynchronousReader( SynchronousReader&& )      = default;

    SynchronousReader& operator= ( SynchronousReader const& ) = delete;
    SynchronousReader& operator= ( SynchronousReader&& )      = default;

    // -------------------------------------------------------------
    //     Init and General Members
    // -------------------------------------------------------------

    void init( std::shared_ptr<BaseInputSource> input_source )
    {
        input_source_ = input_source;
    }

    bool valid() const
    {
        return input_source_ != nullptr;
    }

    BaseInputSource const* input_source() const
    {
        return input_source_.get();
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
