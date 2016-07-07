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
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/io/counting_istream.hpp"

#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>
#include <sstream>

using namespace genesis::utils;

static void test_input_specs( CountingIstream& iit, size_t lines, size_t columns )
{
    CountingIstream eos;

    size_t max_col = 0;
    size_t max_lin = 0;

    while( !iit.eos() ) {
        max_col = std::max( max_col, iit.column() );
        max_lin = std::max( max_lin, iit.line() );

        iit.advance();
    }

    EXPECT_EQ( columns, max_col );
    EXPECT_EQ( lines,   max_lin );
}

static void test_string ( std::string str, size_t lines, size_t columns )
{
    std::istringstream iss ( str );
    CountingIstream iit (iss);
    test_input_specs( iit, lines, columns );
}

TEST(CountingIstream, Strings)
{
    test_string ("",              0, 0);
    test_string ("\n",            1, 1);
    test_string ("\n\n\n",        3, 1);
    test_string ("x",             1, 1);
    test_string ("xyz",           1, 3);
    test_string ("xyz\n",         1, 4);
    test_string ("xyz\nxy\nx\nx", 4, 4);
}

TEST(CountingIstream, LargeFile)
{
    NEEDS_TEST_DATA;
    SCOPED_TRACE("CountingIstream.LargeFile");

    std::string infile = environment->data_dir + "sequence/dna_354.fasta";

    std::ifstream ifs (infile);
    CountingIstream iit (ifs);

    test_input_specs( iit, 3894, 51 );
}

TEST(CountingIstream, NewLines)
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
