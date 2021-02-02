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
#include <utility>
#include <vector>

using namespace genesis::utils;

TEST( Containers, TransformIteratorCopy )
{
    // Prepare a list of consequtive numbers.
    size_t size = 10;
    std::vector<size_t> list( size );
    std::iota( list.begin(), list.end(), 0 );

    // We double each of them, returning a copy of the result.
    auto twice = []( size_t x ){
        return 2*x;
    };
    std::vector<size_t> expected_result{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

    // Version 1: Two independent iterators to begin and end.
    std::vector<size_t> res_it;
    auto begin = make_transform_iterator( twice, list.begin() );
    auto end   = make_transform_iterator( twice, list.end() );
    for( auto it = begin; it != end; ++it ) {
        res_it.push_back( *it );
    }
    EXPECT_EQ( expected_result, res_it );

    // Version 2: A range with specified begin end end.
    std::vector<size_t> res_range_be;
    for( auto e : make_transform_range( twice, list.begin(), list.end() )) {
        res_range_be.push_back( e );
    }
    EXPECT_EQ( expected_result, res_range_be );

    // Version 3: A range based on a container.
    std::vector<size_t> res_range_cont;
    for( auto e : make_transform_range( twice, list )) {
        res_range_cont.push_back( e );
    }
    EXPECT_EQ( expected_result, res_range_cont );
}

TEST( Containers, TransformIteratorConstReference )
{
    // Prepare a list of consequtive numbers, in a pair, where only one entry is useful for us here.
    size_t size = 10;
    std::vector<std::pair<size_t, size_t>> list;
    for( size_t i = 0; i < size; ++i ) {
        list.push_back( std::pair<size_t, size_t>( i, 0 ));
    }

    // We return the first element as our test transform iterator.
    auto first_entry = []( std::pair<size_t, size_t> const& e ) -> size_t const& {
        return e.first;
    };

    // Version 1: Two independent iterators to begin and end.
    auto begin = make_transform_iterator( first_entry, list.begin() );
    auto end   = make_transform_iterator( first_entry, list.end() );
    size_t i = 0;
    for( auto it = begin; it != end; ++it ) {
        EXPECT_EQ( i, *it );
        ++i;
    }

    // Version 2: A range with specified begin end end.
    i = 0;
    for( auto e : make_transform_range( first_entry, list.begin(), list.end() )) {
        EXPECT_EQ( i, e );
        ++i;
    }

    // Version 3: A range based on a container.
    i = 0;
    for( auto e : make_transform_range( first_entry, list )) {
        EXPECT_EQ( i, e );
        ++i;
    }
}

TEST( Containers, TransformIteratorReference )
{
    // Prepare a list of consequtive numbers, in a pair, where only one entry is useful for us here.
    size_t size = 10;
    std::vector<std::pair<size_t, size_t>> list;
    for( size_t i = 0; i < size; ++i ) {
        list.push_back( std::pair<size_t, size_t>( i, 0 ));
    }

    // We return the first element as our test transform iterator.
    auto first_entry = []( std::pair<size_t, size_t>& e ) -> size_t& {
        return e.first;
    };

    // In the following, we iterator three times, each time using a different method of creating
    // the iterator (to test them all), and each time doubling the values in the original list
    // (as everything is handled by non-const reference here). After that, each entry is 8 times
    // its original value, which we also test at the end.

    // Version 1: Two independent iterators to begin and end.
    auto begin = make_transform_iterator( first_entry, list.begin() );
    auto end   = make_transform_iterator( first_entry, list.end() );
    size_t i = 0;
    for( auto it = begin; it != end; ++it ) {
        *it *= 2;
        EXPECT_EQ( 2 * i, *it );
        ++i;
    }

    // Version 2: A range with specified begin end end.
    i = 0;
    for( auto& e : make_transform_range( first_entry, list.begin(), list.end() )) {
        e *= 2;
        EXPECT_EQ( 4 * i, e );
        ++i;
    }

    // Version 3: A range based on a container.
    i = 0;
    for( auto& e : make_transform_range( first_entry, list )) {
        e *= 2;
        EXPECT_EQ( 8 * i, e );
        ++i;
    }

    // Finally, test that the vector has been modifed now.
    std::vector<std::pair<size_t, size_t>> expected_result;
    for( size_t i = 0; i < size; ++i ) {
        expected_result.push_back( std::pair<size_t, size_t>( 8 * i, 0 ));
    }
    EXPECT_EQ( expected_result, list );
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
