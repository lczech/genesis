/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/core/algorithm.hpp"

#include <string>
#include <vector>

using namespace genesis::utils;

TEST( Algorithm, SortIndices )
{
    auto const list = std::vector<int>({ 3, 100, -1, 0, 13, 42, -32 });

    auto exp_less_order = list;
    std::sort( exp_less_order.begin(), exp_less_order.end() );
    auto act_less_order = sort_indices( list.begin(), list.end() );

    for( size_t i = 0; i < act_less_order.size(); ++i ) {
        EXPECT_EQ( exp_less_order[i], list[ act_less_order[i] ] );
    }

    auto exp_greater_order = list;
    std::sort( exp_greater_order.begin(), exp_greater_order.end(), std::greater<int>() );
    auto act_greater_order = sort_indices( list.begin(), list.end(), std::greater<int>() );

    for( size_t i = 0; i < act_greater_order.size(); ++i ) {
        EXPECT_EQ( exp_greater_order[i], list[ act_greater_order[i] ] );
    }
}


TEST( Algorithm, ContainsDuplicates )
{
    auto list = std::vector<int>({ 3, 100, -1, 0, 13, 42, -32 });
    EXPECT_FALSE( contains_duplicates( list ));
    list.push_back( 100 );
    EXPECT_TRUE( contains_duplicates( list ));
}

TEST( Algorithm, Concatenate )
{
    // Test concatenating two non-empty vectors of int.
    {
        std::vector<int> v1 = {1, 2, 3};
        std::vector<int> v2 = {4, 5, 6};
        auto result = concatenate(v1, v2);
        std::vector<int> expected = {1, 2, 3, 4, 5, 6};
        EXPECT_EQ(result, expected);
    }

    // Test concatenating three vectors of int.
    {
        std::vector<int> v1 = {1};
        std::vector<int> v2 = {2, 3};
        std::vector<int> v3 = {4, 5, 6};
        auto result = concatenate(v1, v2, v3);
        std::vector<int> expected = {1, 2, 3, 4, 5, 6};
        EXPECT_EQ(result, expected);
    }

    // Test concatenating an empty vector with a non-empty vector.
    {
        std::vector<int> v1;
        std::vector<int> v2 = {1, 2, 3};
        auto result = concatenate(v1, v2);
        std::vector<int> expected = {1, 2, 3};
        EXPECT_EQ(result, expected);
    }

    // Test concatenating two empty vectors.
    {
        std::vector<int> v1;
        std::vector<int> v2;
        auto result = concatenate(v1, v2);
        std::vector<int> expected;
        EXPECT_EQ(result, expected);
    }

    // Test using rvalue vectors to check that move semantics work.
    {
        auto result = concatenate(std::vector<int>{10, 20}, std::vector<int>{30, 40});
        std::vector<int> expected = {10, 20, 30, 40};
        EXPECT_EQ(result, expected);
    }

    // Test concatenation with vectors of std::string.
    {
        std::vector<std::string> s1 = {"Hello", "World"};
        std::vector<std::string> s2 = {"Foo", "Bar"};
        auto result = concatenate(s1, s2);
        std::vector<std::string> expected = {"Hello", "World", "Foo", "Bar"};
        EXPECT_EQ(result, expected);
    }

    // Test self-assignment where the target container is also the first argument.
    {
        std::vector<int> vec_a = {1, 2, 3};
        std::vector<int> vec_b = {4, 5, 6};
        // Self-assignment: vec_a is used as both target and an input.
        vec_a = concatenate(vec_a, vec_b);
        std::vector<int> expected = {1, 2, 3, 4, 5, 6};
        EXPECT_EQ(vec_a, expected);
    }

    // Test using the same container twice in the argument list.
    {
        std::vector<int> vec_a = {7, 8, 9};
        // Concatenating vec_a with itself should double the sequence.
        vec_a = concatenate(vec_a, vec_a);
        std::vector<int> expected = {7, 8, 9, 7, 8, 9};
        EXPECT_EQ(vec_a, expected);
    }

    // Test a more complex case where the target appears in more than one position.
    {
        std::vector<int> vec_a = {10, 20};
        std::vector<int> vec_b = {30};
        // Here vec_a is both the first and third argument.
        vec_a = concatenate(vec_a, vec_b, vec_a);
        std::vector<int> expected = {10, 20, 30, 10, 20};
        EXPECT_EQ(vec_a, expected);
    }
}
