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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/core/info.hpp"
#include "genesis/utils/io/input_reader.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"

#include <chrono>
#include <fstream>
#include <string>

using namespace genesis::utils;

TEST( Info, Compiler )
{
    // It's hard to test things explicitly, but we can at least call the function,
    // check if it runs without failing, and returns some characters.
    auto info = info_print_compiler();
    ASSERT_GT( info.size(), 200 );
    LOG_DBG << info;

    std::string defs;
    for( auto const& elem : info_preprocessor_definitions() ) {
        defs += elem.first + "=" + elem.second + " ";
    }
    LOG_DBG << "Preprocessor definitons: " << defs;
}

TEST( Info, Hardware )
{
    // Same as above
    auto info = info_print_hardware();
    ASSERT_GT( info.size(), 300 );
    LOG_DBG << info;

    std::string endian = "Genesis currently only supports little endian systems!";
    ASSERT_TRUE( info_get_hardware().is_little_endian ) << endian;
    // ASSERT_FALSE( info_get_hardware().is_big_endian )   << endian;
}

TEST( Info, Usage )
{
    // Init. CPU usage has to be called onces to get started.
    size_t sum = 0;
    info_process_current_cpu_usage();
    info_system_current_cpu_usage();

    // Do some busy work. We need to pass some time here,
    // in order for the CPU usage to register the work.
    auto const start_time = std::chrono::high_resolution_clock::now();
    while (true) {
        // Check elapsed time. If the loop has been running for more than a second, break
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
        if( elapsed_time.count() >= 1 ) {
            break;
        }

        for( size_t j = 0; j < 100000; ++j ) {
            sum += permuted_congruential_generator();
        }
    }
    EXPECT_GT( sum, 0 );

    // Now report the cpu usage. We are fully using it, so let's assume that
    // that is at least 50% of one core. Of course that's kinda random, but good enough
    // to just see that it is not zero, meaning that something reasonable got measured.
    // For the overall system usage, we just test that we get a valid result,
    // as any attempt to predict usage in the virtual envs of the GitHub Actions runners,
    // in particular for Apple M1, is futile.
    auto const process_cpu_usage = info_process_current_cpu_usage();
    auto const total_cpu_usage = info_system_current_cpu_usage();
    EXPECT_GT( process_cpu_usage, 50 );
    EXPECT_GT( total_cpu_usage, 0 );
    // LOG_DBG << process_cpu_usage << " " << total_cpu_usage;

    // Apparently, MacOS automatically multithreads, or something weird.
    // On the GitHub Actions tests, this function runs on 400%, on two cores with hyperthreading,
    // meaning that it is fully loaded. Weird. Anyway, can't test for an upper limit here.
    // EXPECT_LT( process_cpu_usage, 120 );

    // Test some other properties as well
    EXPECT_GT( info_process_current_memory_usage(), 0 );
    EXPECT_GT( info_system_current_memory_usage(), 0 );
    EXPECT_GT( info_system_current_memory_available(), 0 );

    // Print for our amusement
    LOG_DBG << "proc mem use   " << info_process_current_memory_usage();
    LOG_DBG << "sys  mem use   " << info_system_current_memory_usage();
    LOG_DBG << "sys  mem avail " << info_system_current_memory_available();
    LOG_DBG << "sys  mem total " << info_get_hardware().total_memory;

    // Test totals
    EXPECT_GT( info_process_peak_memory_usage(), 0 );
    EXPECT_GT( info_process_total_cpu_time().first, 0.0 );

    // Same as above
    auto info_total = info_process_print_total_usage();
    ASSERT_GT( info_total.size(), 20 );
    LOG_DBG << info_total;
    // LOG_DBG << guess_number_of_threads();
}

TEST( Info, FileCount )
{
    NEEDS_TEST_DATA;
    // LOG_DBG << info_process_max_file_count();
    // LOG_DBG << info_process_current_file_count();
    EXPECT_GT( info_process_max_file_count(), 0 );

    // Get how many files are open at the moment. That should just be stdin, stdout, stderr.
    // Also, apparently, in the GitHub Actions workflow, there are three more...
    auto const initial_size = info_process_current_file_count();
    ASSERT_LE( 3, initial_size );

    {
        // Prepare an input. We cannot just use our InputStream class,
        // as it buffers, and so for all file sizes that we have in our test data,
        // just reads the whole file into the buffer, and then is done with it,
        // so that the file handle is closed immediately already.
        // But in order to still test that our infrastructure works with this,
        // we at least can use all other classes, and do some shennenigans with them here.
        std::string const infile = environment->data_dir + "utils/csv/simple.csv";
        auto input_source = from_file( infile );
        auto input_reader = genesis::utils::make_unique<InputReader>( input_source );

        // Now we have a reader. Let's have it read some data, so that the file source
        // is acutually opened (we are using lazy opening there, which is good, as it minimizes
        // the number of open file handles, but means we have to do reading here).
        char buffer[20];
        input_reader->start_reading( buffer, 10 );
        auto const got_bytes = input_reader->finish_reading();

        // Now we can check our expectations.
        EXPECT_EQ( 10, got_bytes );
        EXPECT_EQ( initial_size + 1, info_process_current_file_count() );
    }

    // After the scope closes, the file handle should be released,
    // so we are back to where we started.
    EXPECT_EQ( initial_size, info_process_current_file_count() );
}
