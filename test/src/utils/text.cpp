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
 * @brief Testing string functions.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/style.hpp"
#include "genesis/utils/text/table.hpp"

#include <sstream>

using namespace genesis::utils;

TEST(Text, Table)
{
    // Not sure yet how to test all this automatically...

    auto t = Table();
    t.add_column("TEST").justify(Table::Column::Justification::kRight);
    t.add_column("ME").justify(Table::Column::Justification::kCentered);
    t.add_column("MORE");

    t << "hello" << "world" << "madness";
    t << "my" << "goodness my" << "guinness!";
    t << "time" << "again?" << "yes";
    // t << "time" << Style("again?") << Style("yes", "blue");
    t << "something" << "" << "end.";

    // Disabled, because it spams the test output.
    // std::cout << "no layout:\n";
    // std::cout << t << "\n";
    //
    // std::cout << "minimal_layout:\n";
    // std::cout << minimal_layout()(t) << "\n";
    //
    // std::cout << "simple_layout:\n";
    // std::cout << simple_layout(true)(t) << "\n";
    // std::cout << simple_layout(false)(t) << "\n";
    //
    // std::cout << "simple_grid:\n";
    // std::cout << simple_grid(true)(t) << "\n";
    // std::cout << simple_grid(false)(t) << "\n";
    //
    // std::cout << "simple_frame:\n";
    // std::cout << simple_frame(true)(t) << "\n";
    // std::cout << simple_frame(false)(t) << "\n";
    //
    // std::cout << "extended_grid:\n";
    // std::cout << extended_grid(true)(t) << "\n";
    // std::cout << extended_grid(false)(t) << "\n";
    //
    // std::cout << "extended_frame:\n";
    // std::cout << extended_frame(true)(t) << "\n";
    // std::cout << extended_frame(false)(t) << "\n";
    //
    // std::cout << "double_grid:\n";
    // std::cout << double_grid(true)(t) << "\n";
    // std::cout << double_grid(false)(t) << "\n";
    //
    // std::cout << "double_frame:\n";
    // std::cout << double_frame(true)(t) << "\n";
    // std::cout << double_frame(false)(t) << "\n";
}

TEST( Text, HeadTail )
{
    std::string large = "hello\nworld.\nthis\nis\na\ntest\nwith\nsome\ntext.";

    // Check length.
    auto vec = split( large, "\n" );
    EXPECT_EQ( 9, vec.size() );

    // Get first and last 3 words.
    EXPECT_EQ( "hello\nworld.\nthis", head( large, 3 ) );
    EXPECT_EQ( "with\nsome\ntext.",   tail( large, 3 ) );

    // Get all words.
    EXPECT_EQ( large, head( large, 10 ) );
    EXPECT_EQ( large, tail( large, 10 ) );
}

TEST( Text, Indent )
{
    std::string lines_orig_1 = "this is\na test with normal\nlines  ";
    std::string lines_orig_2 = "this is  \n   a test with weird\nlines \n  ";

    std::string lines_idnt_1 = "    this is\n    a test with normal\n    lines";
    std::string lines_idnt_2 = "    this is  \n       a test with weird\n    lines \n";

    EXPECT_EQ( lines_idnt_1, indent( lines_orig_1 ));
    EXPECT_EQ( lines_idnt_2, indent( lines_orig_2 ));
}

TEST(Text, Split)
{
    auto simple = split("one:two:three:four", ":");
    EXPECT_EQ(4, simple.size());

    auto mulit_delim = split("one:two three-four", ": -");
    EXPECT_EQ(4, mulit_delim.size());

    auto empty = split("::one:two:three::four:", ":");
    EXPECT_EQ(4, empty.size());

    auto non_empty = split("::one:two:three::four:", ":", false);
    EXPECT_EQ(8, non_empty.size());
}

TEST( Text, Style )
{
    Style blue( "blue" );
    blue.bold( true );

    // Basic
    std::stringstream ss;
    ss << blue( "text" );
    EXPECT_EQ( "\x1B[1;34mtext\x1B[0m", ss.str() );

    // Weird color names.
    blue.foreground_color( "_R eD_ " );
    blue.bold( false );
    ss.str("");
    ss << blue( "is now red!" );
    EXPECT_EQ( "\x1B[31mis now red!\x1B[0m", ss.str() );

    // Reset manually.
    blue.foreground_color( "" );
    ss.str("");
    ss << blue( "empty" );
    EXPECT_EQ( "empty", ss.str() );
}

TEST( Text, ToString )
{
    double pi    = 3.14159267535;
    double zeros = 42.4200000000;

    EXPECT_EQ( "3.14159", to_string( pi ) );
    EXPECT_EQ( "42.42",   to_string( zeros ) );

    EXPECT_EQ( "3",       to_string_precise( pi, 0 ) );
    EXPECT_EQ( "3.1",     to_string_precise( pi, 1 ) );
    EXPECT_EQ( "3.1416",  to_string_precise( pi, 4 ) );

    EXPECT_EQ( "42",      to_string_precise( zeros, 0 ) );
    EXPECT_EQ( "42.4",    to_string_precise( zeros, 1 ) );
    EXPECT_EQ( "42.4200", to_string_precise( zeros, 4 ) );

    EXPECT_EQ( "3",       to_string_rounded( pi, 0 ) );
    EXPECT_EQ( "3.1",     to_string_rounded( pi, 1 ) );
    EXPECT_EQ( "3.1416",  to_string_rounded( pi, 4 ) );

    EXPECT_EQ( "42",      to_string_rounded( zeros, 0 ) );
    EXPECT_EQ( "42.4",    to_string_rounded( zeros, 1 ) );
    EXPECT_EQ( "42.42",   to_string_rounded( zeros, 4 ) );
}
