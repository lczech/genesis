#ifndef GENESIS_UTILS_TOOLS_TIMER_H_
#define GENESIS_UTILS_TOOLS_TIMER_H_

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

#include <chrono>
#include <ctime>
#include <cstddef>
#include <functional>

namespace genesis {
namespace utils {

// =================================================================================================
//     Timer
// =================================================================================================

/**
 * @brief Measure time.
 */
struct Timer
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Timer( bool auto_start = false )
        : start_time_()
        , total_time_(0.0)
        , running_(false)
    {
        if( auto_start ) {
            start();
        }
    }

    ~Timer() = default;

    Timer(Timer const&) = default;
    Timer(Timer&&)      = default;

    Timer& operator= (Timer const&) = default;
    Timer& operator= (Timer&&)      = default;

    // -------------------------------------------------------------------------
    //     Timing Functions
    // -------------------------------------------------------------------------

    void start()
    {
        if( !running_ ) {
            start_time_ = std::chrono::high_resolution_clock::now();
            running_ = true;
        }
    }

    void stop()
    {
        if( running_ ) {
            auto end_time = std::chrono::high_resolution_clock::now();
            total_time_ += std::chrono::duration<double>(end_time - start_time_).count();
            running_ = false;
        }
    }

    void reset()
    {
        total_time_ = 0.0;
        running_ = false;
    }

    void pause()
    {
        stop();
    }

    void resume()
    {
        start();
    }

    /**
     * @brief Return elapsed time in seconds.
     */
    double elapsed() const
    {
        if( running_ ) {
            auto const now = std::chrono::high_resolution_clock::now();
            return total_time_ + std::chrono::duration<double>( now - start_time_ ).count();
        }
        return total_time_;
    }

    /**
     * @brief Return elapsed time in a given `chrono` duration type (defaulting to seconds).
     */
    template <typename DurationType = std::chrono::seconds>
    double elapsed_as() const
    {
        if( running_ ) {
            auto const now = std::chrono::high_resolution_clock::now();
            auto const duration = std::chrono::duration<double>(total_time_) + (now - start_time_);
            return std::chrono::duration<double, typename DurationType::period>(duration).count();
        }
        auto const duration = std::chrono::duration<double>(total_time_);
        return std::chrono::duration<double, typename DurationType::period>(duration).count();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Start time, and accumulated time in seconds.
    std::chrono::high_resolution_clock::time_point start_time_;
    double total_time_;
    bool running_;

};

// =================================================================================================
//     Timer Functions
// =================================================================================================

/**
 * @brief Measure the execution time of a function, in the given `chrono` duration type
 * (defaulting to seconds).
 */
template <typename DurationType = std::chrono::seconds, typename Func, typename... Args>
double measure_execution_time(Func&& func, Args&&... args)
{
    // Time the execution of the function.
    auto const start_time = std::chrono::high_resolution_clock::now();
    std::forward<Func>( func )( std::forward<Args>( args )... );
    auto const end_time = std::chrono::high_resolution_clock::now();

    // Calculate duration as double, preserving precision
    auto duration = end_time - start_time;
    return std::chrono::duration<double, typename DurationType::period>(duration).count();
    // return std::chrono::duration_cast<DurationType>(end_time - start_time).count();
}

} // namespace utils
} // namespace genesis

#endif // include guard
