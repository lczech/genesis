/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/io/output_target.hpp"
#include "genesis/utils/core/fs.hpp"

#include <cstdio>
#include <sstream>

using namespace genesis;
using namespace genesis::utils;

TEST( OutputTarget, WriteCompressed )
{
    NEEDS_TEST_DATA;
    std::string outfile = environment->data_dir + "output_target_write_compressed.txt";
    std::string data;

    // Scope, so that file is closed at the end.
    {
        // Open a compressed output target
        auto target = to_file( outfile, GzipCompressionLevel::kDefaultCompression );

        // Write some stuff to the file.
        for( size_t i = 0; i < 1000; ++i ) {
            data.append( std::to_string(i) + "\n" );
        }
        (*target) << data;
    }

    // Read the file again. This uses auto-detection of the compression.
    outfile +=  + ".gz";
    ASSERT_TRUE( file_exists( outfile ));
    auto read = file_read( outfile );
    EXPECT_EQ( data, read );

    ASSERT_EQ (0, std::remove( outfile.c_str() ));
}
