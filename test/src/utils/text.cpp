/**
 * @brief Testing string functions.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/text/string.hpp"
#include "lib/utils/text/style.hpp"
#include "lib/utils/text/table.hpp"

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
