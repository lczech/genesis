#ifndef GENESIS_UTILS_IO_INPUT_READER_H_
#define GENESIS_UTILS_IO_INPUT_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/input_source.hpp"

#include "utils/core/logging.hpp"

#include <memory>
#include <string>
#include <utility>

#ifdef PTHREADS
#    include <condition_variable>
#    include <mutex>
#    include <thread>
#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Input Reader
// =================================================================================================

#ifdef PTHREADS

    class AsynchronousReader;
    using InputReader = AsynchronousReader;

#else

    class SynchronousReader;
    using InputReader = SynchronousReader;

#endif

// =================================================================================================
//     Asynchronous Reader
// =================================================================================================

#ifdef PTHREADS

/**
 * @brief Read bytes from an @link InputSourceInterface InputSource@endlink into a `char buffer`.
 *
 * The reading is done asynchronously, that is, a second thread is started.
 *
 * Implementation details inspired by
 * [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) by Ben Strasser.
 */
class AsynchronousReader
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    AsynchronousReader() = default;

    AsynchronousReader( AsynchronousReader const& ) = delete;
    AsynchronousReader( AsynchronousReader&& )      = default;

    AsynchronousReader& operator= ( AsynchronousReader const& ) = delete;
    AsynchronousReader& operator= ( AsynchronousReader&& )      = default;

    ~AsynchronousReader()
    {
        if( input_source_ == nullptr ) {
            return;
        }

        // Terminate the reading process, in case it is still running.
        { // Scoped lock.
            std::unique_lock< std::mutex > guard( lock_ );
            destructor_called_ = true;
        }

        cond_read_requested_.notify_one();
        worker_.join();
    }

    // -------------------------------------------------------------
    //     Init and General Members
    // -------------------------------------------------------------

    void init( std::unique_ptr< InputSourceInterface > input_source )
    {
        // Get a lock.
        std::unique_lock< std::mutex > guard( lock_ );

        // Prepare input variables.
        input_source_      = std::move( input_source );
        target_size_       = -1;
        destructor_called_ = false;

        // Prepare worker thread.
        worker_ = std::thread( [&] {
            std::unique_lock< std::mutex > guard( lock_ );
            try {

                // Read until termination requested.
                while( true ) {

                    // Condition: wait until the master wants the worker to read.
                    cond_read_requested_.wait(
                        guard,
                        [&] () {
                            return ( target_size_ != -1 ) || destructor_called_;
                        }
                    );

                    // If we are about to destroy the object, we can stop here.
                    if( destructor_called_ ) {
                        return;
                    }

                    // Read.
                    achieved_size_ = input_source_->read( target_buffer_, target_size_ );
                    target_size_   = -1;

                    // If we did not get any data, we are done with the input source.
                    if( achieved_size_ == 0 ) {
                        break;
                    }

                    cond_read_finished_.notify_one();
                }

            // Store any exception, so that we can re-throw from main thread.
            } catch( ... ) {
                read_except_ptr_ = std::current_exception();
            }

            cond_read_finished_.notify_one();
        });
    }

    bool valid() const
    {
        return input_source_ != nullptr;
    }

    InputSourceInterface const* input_source() const
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

    void start_reading( char* target_buffer, int target_size )
    {
        // Set the target variables and start the worker.
        std::unique_lock< std::mutex > guard( lock_ );
        target_buffer_ = target_buffer;
        target_size_   = target_size;
        achieved_size_ = -1;
        cond_read_requested_.notify_one();
    }

    int finish_reading()
    {
        // Wait until the worker is done reading.
        std::unique_lock< std::mutex > guard(lock_);
        cond_read_finished_.wait(
            guard,
            [&]{
                    return achieved_size_ != -1 || read_except_ptr_;
            }
        );

        // If there was an exception, re-throw. Otherwise, return number of read bytes.
        if( read_except_ptr_ ) {
            std::rethrow_exception( read_except_ptr_ );
        } else {
            return achieved_size_;
        }
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    std::unique_ptr<InputSourceInterface> input_source_;

    char* target_buffer_;
    int   target_size_;
    int   achieved_size_;

    std::thread worker_;
    bool destructor_called_;
    std::exception_ptr read_except_ptr_;

    std::mutex lock_;
    std::condition_variable cond_read_requested_;
    std::condition_variable cond_read_finished_;
};

#else

// =================================================================================================
//     Synchronous Reader
// =================================================================================================

/**
 * @brief Read bytes from an @link InputSourceInterface InputSource@endlink into a `char buffer`.
 *
 * The reading is done synchronously, that is, reading occurs on request.
 *
 * Implementation details inspired by
 * [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) by Ben Strasser.
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

    void init( std::unique_ptr<InputSourceInterface> input_source )
    {
        input_source_ = std::move( input_source );
    }

    bool valid() const
    {
        return input_source_ != nullptr;
    }

    InputSourceInterface const* input_source() const
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

    void start_reading( char* target_buffer, int target_size )
    {
        target_buffer_ = target_buffer;
        target_size_   = target_size;
    }

    int finish_reading()
    {
        return input_source_->read( target_buffer_, target_size_ );
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    std::unique_ptr<InputSourceInterface> input_source_;

    char* target_buffer_;
    int   target_size_;
};

#endif

} // namespace utils
} // namespace genesis

#endif // include guard
