/**
 * @brief Testing Indexed List class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/core/indexed_list.hpp"

using namespace genesis;

TEST(IndexedList, Basics)
{
    IndexedList<int> list1;
    EXPECT_EQ(0, list1.size());

    IndexedList<int> list2(5);
    EXPECT_EQ(5, list2.size());
    EXPECT_EQ(0, list2.front());
    EXPECT_EQ(0, list2.back());

    IndexedList<int> list3(5, 42);
    EXPECT_EQ(5, list3.size());
    EXPECT_EQ(42, list3.front());
    EXPECT_EQ(42, list3.back());
}

TEST(IndexedList, Iterators)
{
    IndexedList<int> list(5, 42);

    for (auto it = list.begin(); it != list.end(); ++it) {
        EXPECT_EQ(42, *it);
    }
}
