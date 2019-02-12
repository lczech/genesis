/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/core/fs.hpp"

#include <string>

using namespace genesis::utils;

TEST( FileSystem, DirCreate )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    std::string dir = environment->data_dir + "/foo/bar/test/long";
    // dir_create( dir, true );
}

TEST( FileSystem, CurrentPath )
{
    // Run once for exception check.
    ASSERT_NO_THROW( current_path() );

    // Run again for result check.
    auto const cwd = current_path();
    EXPECT_NE( 0, cwd.size() );

    // std::cout << cwd << "\n";
}

TEST( FileSystem, RealPath )
{
    auto const cwd = current_path();
    ASSERT_NO_THROW( real_path( cwd ));

    auto rp = real_path( cwd );
    EXPECT_NE( 0, rp.size() );

    // std::cout << rp << "\n";
}

TEST( File, ReadLines )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    std::string const infile = environment->data_dir + "utils/csv/table.csv";
    auto const lines = file_read_lines( infile );

    ASSERT_EQ( 11, lines.size() );
    EXPECT_EQ( "alpha,0,0,4.5", lines[1] );
}
