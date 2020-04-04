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
 * @brief Testing string functions.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/style.hpp"
#include "genesis/utils/text/table.hpp"

#include <cctype>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

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

// TEST( Text, Wrap )
// {
    // std::string const text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec a "
    // "diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec "
    // "diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec "
    // "diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec\n\n"
    // "diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec "
    // "diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec "
    // "diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec\n"
    // "consectetur ante hendrerit. Donec et mollis dolor. Praesent et diam eget libero egestas "
    // "mattis sit amet vitae augue. Nam tincidunt congue enim, ut porta lorem lacinia consectetur!";
    //
    // LOG_DBG << wrap( text );
    // LOG_DBG << wrap( text, 15 );
    // LOG_DBG << wrap( text, 5 );
// }
