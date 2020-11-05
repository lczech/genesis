/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/containers/filter_iterator.hpp"
#include "genesis/utils/containers/transform_iterator.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

using namespace genesis::utils;

TEST( Containers, TransformIterator )
{
    // Prepare a list of consequtive numbers.
    size_t size = 10;
    std::vector<size_t> list( size );
    std::iota( list.begin(), list.end(), 0 );

    // We double each of them.
    auto twice = []( size_t x ){
        return 2*x;
    };
    std::vector<size_t> result{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

    // Version 1: Two independent iterators to begin and end.
    std::vector<size_t> res_it;
    auto begin = make_transform_iterator( twice, list.begin() );
    auto end   = make_transform_iterator( twice, list.end() );
    for( auto it = begin; it != end; ++it ) {
        res_it.push_back( *it );
    }
    EXPECT_EQ( result, res_it );

    // Version 2: A range with specified begin end end.
    std::vector<size_t> res_range_be;
    for( auto e : make_transform_range( twice, list.begin(), list.end() )) {
        res_range_be.push_back( e );
    }
    EXPECT_EQ( result, res_range_be );

    // Version 3: A range based on a container.
    std::vector<size_t> res_range_cont;
    for( auto e : make_transform_range( twice, list )) {
        res_range_cont.push_back( e );
    }
    EXPECT_EQ( result, res_range_cont );
}

TEST( Containers, FilterIterator )
{
    // Prepare a list of consequtive numbers.
    size_t size = 20;
    std::vector<size_t> list( size );
    std::iota( list.begin(), list.end(), 0 );

    // We only filter even numbers. Incidentally. the result is the same as above.
    auto even = []( size_t x ){
        return x % 2 == 0;
    };
    std::vector<size_t> result{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

    // Version 1: Two independent iterators to begin and end.
    std::vector<size_t> res_it;
    auto begin = make_filter_iterator( even, list.begin(), list.end() );
    auto end   = make_filter_iterator( even, list.end(), list.end() );
    for( auto it = begin; it != end; ++it ) {
        res_it.push_back( *it );
    }
    EXPECT_EQ( result, res_it );

    // Version 2: A range with specified begin end end.
    std::vector<size_t> res_range_be;
    for( auto e : make_filter_range( even, list.begin(), list.end() )) {
        res_range_be.push_back( e );
    }
    EXPECT_EQ( result, res_range_be );

    // Version 3: A range based on a container.
    std::vector<size_t> res_range_cont;
    for( auto e : make_filter_range( even, list )) {
        res_range_cont.push_back( e );
    }
    EXPECT_EQ( result, res_range_cont );
}
