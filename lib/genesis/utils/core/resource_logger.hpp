#ifndef GENESIS_UTILS_CORE_RESOURCE_LOGGER_H_
#define GENESIS_UTILS_CORE_RESOURCE_LOGGER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include "genesis/utils/core/info.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/date_time.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Resource Logger
// =================================================================================================

/**
 * @brief Log resource usage of the current program.
 *
 * This is meant for long-term tracking and optimization, where options such as valgrind might
 * be too slow, and gdb/perf not available or too cumbersome to use, i.e., in cluster environments.
 * This tracker can be added to any program, and will write out resource usage in regular intervals,
 * along with time stamps to cross-reference with the normal logging output, to see what the
 * program was doing at the time.
 *
 * Usage: Simply add to the beginning of main() like this:
 *
 *     ResourceLogger logger( std::chrono::seconds(10), "resources.log" );
 *
 * Can be adjusted as needed.
 */
class ResourceLogger
{
public:

    using OutputSink = std::function<void(const std::string&)>;

    enum class MemoryUnit
    {
        kBytes,
        kKilobytes,
        kMegabytes,
        kGigabytes,
        kTerabytes
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    ResourceLogger( std::chrono::duration<int> interval )
        : interval_(interval)
        , running_(false)
    {}

    ResourceLogger( std::chrono::duration<int> interval, std::string const& log_file )
        : ResourceLogger( interval )
    {
        log_to_file( log_file );
        start();
    }

    ~ResourceLogger()
    {
        stop();
    }

    ResourceLogger( ResourceLogger const& ) = delete;
    ResourceLogger( ResourceLogger&& )      = delete;

    ResourceLogger& operator= ( ResourceLogger const& ) = delete;
    ResourceLogger& operator= ( ResourceLogger&& )      = delete;

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    void start()
    {
        // Do nothing if we are already running, as otherwise
        // another logging thread would be spawned.
        if( running_ ) {
            return;
        }
        running_ = true;
        logging_thread_ = std::thread( &ResourceLogger::logging_worker_thread_, this );
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(logging_mutex_);
            if( !running_ ) {
                return;
            }
            running_ = false;
        }
        logging_cv_.notify_all();
        if( logging_thread_.joinable() ) {
            logging_thread_.join();
        }
    }

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    void set_interval( std::chrono::duration<int> new_interval )
    {
        {
            std::lock_guard<std::mutex> lock(logging_mutex_);
            interval_ = new_interval;
        }
        logging_cv_.notify_all();
    }

    void set_memory_unit( MemoryUnit unit )
    {
        mem_unit_ = unit;
    }

    // -------------------------------------------------------------------------
    //     Log Sinks
    // -------------------------------------------------------------------------

    void log_to_sink( OutputSink sink, bool write_header = true )
    {
        // Write the header, and add it for logging.
        if( write_header ) {
            sink( make_header_() );
        }
        std::unique_lock<std::mutex> lock( sink_mutex_ );
        output_sinks_.push_back(sink);
    }

    void log_to_file( std::string const& log_file, bool write_header = true )
    {
        auto target = std::make_shared<std::ofstream>();
        file_output_stream( log_file, *target );
        log_to_sink(
            [target]( std::string const& message ){
                (*target) << message << std::endl;
            },
            write_header
        );
    }

    void log_to_stdout( bool write_header = false )
    {
        log_to_sink(
            []( std::string const& message ) {
                std::cout << message << std::endl;
            },
            write_header
        );
    }

    void log_to_stderr( bool write_header = false )
    {
        log_to_sink(
            []( std::string const& message ) {
                std::cerr << message << std::endl;
            },
            write_header
        );
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    std::string make_header_()
    {
        return "num\tdate\ttime\tcpu\tmem\ttasks\tfiles";
    }

    std::string make_log_message_()
    {
        // TODO make this a functional instead, so that users can provide any form of logging they want. // TODO rename the class into TimedLogger or something, then, provide a default implementation
        // of this here, so that we still have an easy to use ResourceLogger based on that.

        auto get_current_mem_ = [this]() -> double
        {
            double const mem_bytes = info_process_current_memory_usage();
            switch( mem_unit_ ) {
                case MemoryUnit::kBytes: {
                    return mem_bytes;
                }
                case MemoryUnit::kKilobytes: {
                    return mem_bytes / 1024.0;
                }
                case MemoryUnit::kMegabytes: {
                    return mem_bytes / 1024.0 / 1024.0;
                }
                case MemoryUnit::kGigabytes: {
                    return mem_bytes / 1024.0 / 1024.0 / 1024.0;
                }
                case MemoryUnit::kTerabytes: {
                    return mem_bytes / 1024.0 / 1024.0 / 1024.0 / 1024.0;
                }
                // case MemoryUnit::kHuman: {
                //     return to_string_byte_format( mem_bytes );
                // }
                default: {
                    throw std::invalid_argument( "Invalid memory size" );
                }
            }
        };

        auto get_pending_tasks_ = [&]() -> size_t
        {
            if( Options::get().global_thread_pool() ) {
                return Options::get().global_thread_pool()->pending_tasks_count();
            }
            return 0;
        };

        LOG_DBG << info_process_current_memory_usage();

        std::stringstream ss;
        ss << (num_counter_++);
        ss << "\t" << current_date();
        ss << "\t" << current_time();
        ss << "\t" << std::fixed << std::setprecision(2) << info_process_current_cpu_usage();
        ss << "\t" << std::fixed << std::setprecision(2) << get_current_mem_();
        ss << "\t" << get_pending_tasks_();
        ss << "\t" << info_process_current_file_count();

        // ss << "\n";
        return ss.str();
    }

    void log_message_to_sinks_()
    {
        auto const message = make_log_message_();
        std::unique_lock<std::mutex> lock( sink_mutex_ );
        for( auto& sink : output_sinks_ ) {
            sink( message );
        }
    }

    void logging_worker_thread_()
    {
        // Log one intial time, as otherwise the logging thread will first
        // wait for the time interval to pass before making the first log.
        log_message_to_sinks_();

        // Run while the atomic var is active, then exit.
        while( true ) {
            // Wait for either the time interval, or check that we are still running.
            // This avoids blocking at the end of the program, when the thread needs to join,
            // but is in sleep.
            std::unique_lock<std::mutex> lock(logging_mutex_);
            if( logging_cv_.wait_for( lock, interval_, [this] { return !running_; })) {
                break;
            }

            // Now we have waited, so make a log.
            log_message_to_sinks_();
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

    std::chrono::duration<int> interval_;
    std::atomic<bool> running_;

    std::thread logging_thread_;
    std::mutex logging_mutex_;
    std::condition_variable logging_cv_;

    std::atomic<size_t> num_counter_ = 0;
    MemoryUnit mem_unit_ = MemoryUnit::kMegabytes;

    std::mutex sink_mutex_;
    std::vector<OutputSink> output_sinks_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
