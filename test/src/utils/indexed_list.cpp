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
    IndexedList<int> list2(5);
    IndexedList<int> list3(5, 42);
}
