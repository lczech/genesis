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
    auto t = text::Table();
    t.add_column("TEST").justify(genesis::text::Table::Column::Justification::kRight);
    t.add_column("ME").justify(genesis::text::Table::Column::Justification::kCentered);
    t.add_column("MORE");

    t << "hello" << "world" << "madness";
    t << "my" << "goodness my" << "guinness!";
    t << "time" << "again?" << "yes";
    t << "something" << "" << "end.";

    t.write( std::cout );
}
