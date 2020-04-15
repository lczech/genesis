/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
#include <cstdio>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

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

TEST(InputStream, FileReading)
{
    NEEDS_TEST_DATA;
    SCOPED_TRACE("InputStream.FileReading");

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

TEST( InputStream, LargeFile )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    // Create a large file with a known number and length of lines.
    std::string tmpfile = environment->data_dir + "utils/large_file.txt";
    std::ofstream out{ tmpfile };
    ASSERT_TRUE( out );

    // We use a quarter of the block len as our text block,
    // and then construct a file with multiples of this for the lines.
    // That means, at some point we will have lines that are exact multiples of the block size.
    // This is good for testing boundary conditions of exact matches of lengths,
    // so that we can check some of the assertions of the input stream!
    auto const block_len = InputStream::BlockLength;
    auto const text = std::string( block_len / 4, 'x' );

    // Make a file with the form
    //
    // x
    // xx
    // xxx
    // xxxx
    // ...
    for( size_t i = 0; i < 16; ++i ) {
        for( size_t j = 0; j < i; ++j ) {
            out << text;
        }
        out << "\n";
    }
    out.close();

    // Now read it again and expect the correct line length.
    auto it = InputStream( from_file( tmpfile ));
    size_t cnt = 0;
    while( it ) {

        EXPECT_EQ( cnt + 1, it.line() );
        EXPECT_EQ( 1, it.column() );

        auto const line = it.get_line();

        EXPECT_EQ( cnt * text.size(), line.size() );
        EXPECT_EQ( cnt + 2, it.line() );
        EXPECT_EQ( 1, it.column() );

        ++cnt;
    }

    // Make sure the file is deleted.
    ASSERT_EQ( 0, std::remove(tmpfile.c_str()) );
}
