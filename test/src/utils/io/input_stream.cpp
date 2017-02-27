/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/core/std.hpp"

#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>
#include <sstream>

using namespace genesis;
using namespace genesis::utils;

static void test_input_specs( InputStream& instr, size_t lines, size_t columns )
{
    size_t max_col = 0;
    size_t max_lin = 0;

    while( instr ) {
        max_col = std::max( max_col, instr.column() );
        max_lin = std::max( max_lin, instr.line() );

        ++instr;
    }

    EXPECT_EQ( columns, max_col );
    EXPECT_EQ( lines,   max_lin );
}

static void test_string ( std::string str, size_t lines, size_t columns )
{
    InputStream instr( utils::make_unique< StringInputSource >( str ));
    test_input_specs( instr, lines, columns );
}

TEST(InputStream, Strings)
{
    test_string ("",              0, 0);
    test_string ("\n",            1, 1);
    test_string ("\n\n\n",        3, 1);
    test_string ("x",             1, 2);
    test_string ("xyz",           1, 4);
    test_string ("xyz\n",         1, 4);
    test_string ("xyz\nxy\nx\nx", 4, 4);
}

TEST(InputStream, LargeFile)
{
    NEEDS_TEST_DATA;
    SCOPED_TRACE("InputStream.LargeFile");

    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    InputStream instr( utils::make_unique< FileInputSource >( infile ));

    test_input_specs( instr, 110, 51 );
}

TEST(InputStream, NewLines)
{
    // Just \n.
    test_string( "a\nb",   2, 2);
    test_string( "a\nb\n", 2, 2);

    // Just \r.
    test_string( "a\rb",   2, 2);
    test_string( "a\rb\r", 2, 2);

    // Both.
    test_string( "a\r\nb",     2, 2);
    test_string( "a\r\nb\r\n", 2, 2);

    // Go crazy.
    test_string( "\r\r\n\r\n\n", 4, 1);
}
