/**
 * @brief Testing string functions.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/string.hpp"

using namespace genesis;

TEST(String, Split)
{
    auto simple = string_split("one:two:three:four", ":");
    EXPECT_EQ(4, simple.size());

    auto mulit_delim = string_split("one:two three-four", ": -");
    EXPECT_EQ(4, mulit_delim.size());

    auto empty = string_split("::one:two:three::four:", ":");
    EXPECT_EQ(4, empty.size());

    auto non_empty = string_split("::one:two:three::four:", ":", false);
    EXPECT_EQ(8, non_empty.size());
}
