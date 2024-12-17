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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/tools/timer.hpp"

#include <chrono>
#include <iostream>
#include <thread>

using namespace genesis;
using namespace utils;

// Function to test
void time_example_function_( int milliseconds )
{
    std::this_thread::sleep_for(
        std::chrono::milliseconds( milliseconds )
    );
}

TEST( Tools, Timer )
{
    Timer timer;

    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.pause();
    // Paused period, not counted.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.stop();

    LOG_MSG << "Elapsed time: " << timer.elapsed() << " seconds\n";
    LOG_MSG << "Elapsed time: " << timer.elapsed_as() << " seconds\n";
    LOG_MSG << "Elapsed time: " << timer.elapsed_as<std::chrono::milliseconds>() << " ms\n";
    LOG_MSG << "Elapsed time: " << timer.elapsed_as<std::chrono::microseconds>() << " µs\n";
    LOG_MSG << "Elapsed time: " << timer.elapsed_as<std::chrono::nanoseconds>() << " ns\n";
}

TEST( Tools, MeasureExecTime )
{
    // Using the free function without specifying a duration type (defaults to seconds)
    double const time1 = measure_execution_time( time_example_function_, 50 );
    LOG_MSG << "time_example_function_ took " << time1 << " seconds\n";

    // Using the free function with milliseconds
    double const time2 = measure_execution_time<std::chrono::milliseconds>( time_example_function_, 50 );
    LOG_MSG << "time_example_function_ took " << time2 << " milliseconds\n";
}
