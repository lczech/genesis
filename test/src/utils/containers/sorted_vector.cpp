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
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/containers/sorted_vector.hpp"
#include "lib/utils/containers/sorted_vector/operators.hpp"

using namespace genesis::utils;

TEST( Containers, SortedVectorBasics )
{
    SortedVector<int> vec = { 0, 1, 2, 3, -5 };

    EXPECT_EQ(  5, vec.size() );
    EXPECT_EQ( -5, vec.front() );
    EXPECT_EQ(  3, vec.back() );
    EXPECT_EQ(  2, vec.index_of( 1 ) );

    EXPECT_FALSE( vec.contains( 7 ));
    EXPECT_TRUE(  vec.contains( 1 ));
    vec.remove( 7 );
    vec.remove( 1 );
    EXPECT_FALSE( vec.contains( 1 ));

    EXPECT_EQ( 4, vec.size() );
    EXPECT_EQ( 4, vec.index_of( 1 ) );
    EXPECT_EQ( 2, vec.index_of( 2 ) );
}

TEST( Containers, SortedVectorCompare )
{
    SortedVector<int> sup = { 0, 1, 2, 3, 4, 5 };

    EXPECT_TRUE( equals( sup, { 0, 1, 2, 3, 4, 5 } ) );
    EXPECT_TRUE( ! equals( sup, { 0, 2, 3, 4, 5 } ) );

    EXPECT_TRUE( is_subset_of( {}, sup ));
    EXPECT_TRUE( is_subset_of( { 0 }, sup ));
    EXPECT_TRUE( is_subset_of( { 1, 3 }, sup ));
    EXPECT_TRUE( is_subset_of( { 5 }, sup ));
    EXPECT_TRUE( !is_subset_of( { 0, 1, 2, 3, 4, 5 }, sup ));
    EXPECT_TRUE( is_subset_or_equal( { 0, 1, 2, 3, 4, 5 }, sup ));
}
