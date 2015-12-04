/**
 * @brief Testing string functions.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/text/string.hpp"
#include "lib/utils/text/table.hpp"

using namespace genesis;

TEST(String, Split)
{
    auto simple = text::split("one:two:three:four", ":");
    EXPECT_EQ(4, simple.size());

    auto mulit_delim = text::split("one:two three-four", ": -");
    EXPECT_EQ(4, mulit_delim.size());

    auto empty = text::split("::one:two:three::four:", ":");
    EXPECT_EQ(4, empty.size());

    auto non_empty = text::split("::one:two:three::four:", ":", false);
    EXPECT_EQ(8, non_empty.size());
}

TEST(String, Table)
{
    // Not sure yet how to test all this automatically...

    auto t = text::Table();
    t.add_column("TEST").justify(genesis::text::Table::Column::Justification::kRight);
    t.add_column("ME").justify(genesis::text::Table::Column::Justification::kCentered);
    t.add_column("MORE");

    t << "hello" << "world" << "madness";
    t << "my" << "goodness my" << "guinness!";
    t << "time" << "again?" << "yes";
    t << "something" << "" << "end.";

    // Disabled, because it spams the test output.
    // std::cout << "no layout:\n";
    // std::cout << t << "\n";
    //
    // std::cout << "minimal_layout:\n";
    // std::cout << text::minimal_layout()(t) << "\n";
    //
    // std::cout << "simple_layout:\n";
    // std::cout << text::simple_layout(false)(t) << "\n";
    // std::cout << text::simple_layout(true)(t) << "\n";
    //
    // std::cout << "simple_grid:\n";
    // std::cout << text::simple_grid(false)(t) << "\n";
    // std::cout << text::simple_grid(true)(t) << "\n";
    //
    // std::cout << "simple_frame:\n";
    // std::cout << text::simple_frame(false)(t) << "\n";
    // std::cout << text::simple_frame(true)(t) << "\n";
    //
    // std::cout << "extended_grid:\n";
    // std::cout << text::extended_grid(false)(t) << "\n";
    // std::cout << text::extended_grid(true)(t) << "\n";
    //
    // std::cout << "extended_frame:\n";
    // std::cout << text::extended_frame(false)(t) << "\n";
    // std::cout << text::extended_frame(true)(t) << "\n";
    //
    // std::cout << "double_grid:\n";
    // std::cout << text::double_grid(false)(t) << "\n";
    // std::cout << text::double_grid(true)(t) << "\n";
    //
    // std::cout << "double_frame:\n";
    // std::cout << text::double_frame(false)(t) << "\n";
    // std::cout << text::double_frame(true)(t) << "\n";
}
