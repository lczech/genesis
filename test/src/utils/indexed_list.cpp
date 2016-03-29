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
 * @brief Testing Indexed List class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/core/indexed_list.hpp"

using namespace genesis;
using namespace genesis::utils;

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
