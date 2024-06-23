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

#include <fstream>
#include <string>

using namespace genesis::utils;

TEST( Info, Compiler )
{
    // It's hard to test things explicitly, but we can at least call the function,
    // check if it runs without failing, and returns some characters.
    auto info = info_print_compiler();
    ASSERT_GT( info.size(), 200 );
    // LOG_DBG << info;
}

TEST( Info, Hardware )
{
    // Same as above
    auto info = info_print_hardware();
    ASSERT_GT( info.size(), 1000 );
    // LOG_DBG << info;
}

TEST( Info, Endianness )
{
    std::string endian = "Genesis currently only supports little endian systems!";
    ASSERT_TRUE( info_get_hardware().is_little_endian ) << endian;
    // ASSERT_FALSE( info_get_hardware().is_big_endian )   << endian;
}

TEST( Info, FileCount )
{
    NEEDS_TEST_DATA;
    // LOG_DBG << info_max_file_count();
    // LOG_DBG << info_current_file_count();

    // Get how many files are open at the moment. That should just be stdin, stdout, stderr.
    // Also, apparently, in the GitHub Actions workflow, there are three more...
    auto const initial_size = info_current_file_count();
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
        EXPECT_EQ( initial_size + 1, info_current_file_count() );
    }

    // After the scope closes, the file handle should be released,
    // so we are back to where we started.
    EXPECT_EQ( initial_size, info_current_file_count() );
}
