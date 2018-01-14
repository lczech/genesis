/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief Testing Matrix class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <algorithm>
#include <sstream>

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/containers/matrix/row.hpp"

using namespace genesis;
using namespace utils;

TEST( Matrix, Row )
{
    auto mat_1 = Matrix<int>(4, 4, {
        1, 2, 3, 4,
        10, 9, 8, 7,
        42, 13, 69, 17,
        0, 0, 0, 0
    });
    auto mat_2 = Matrix<int>(1, 5, {
        1, 2, 3, 4, 5
    });

    // Basic access
    auto row_0 = mat_1.row( 0 );
    EXPECT_EQ( 4, row_0.size());
    EXPECT_EQ( 2, row_0.at( 1 ));
    EXPECT_EQ( 3, row_0[ 2 ]);
    EXPECT_EQ( 0, row_0.row() );
    EXPECT_EQ( mat_1, row_0.matrix() );
    EXPECT_EQ( &mat_1, &row_0.matrix() );

    // Multiple instances should compare equal
    EXPECT_EQ( mat_1.row( 0 ), mat_1.row( 0 ) );
    EXPECT_TRUE( mat_1.row( 0 ) == mat_1.row( 0 ) );

    // Multiple iterators should be equal
    EXPECT_EQ( mat_1.row( 0 ).begin(), mat_1.row( 0 ).begin() );
    EXPECT_TRUE( mat_1.row( 0 ).begin() == mat_1.row( 0 ).begin() );

    // Copy to and from vector
    std::vector<int> vec;
    vec = row_0;
    EXPECT_EQ( 4, vec[3] );
    EXPECT_EQ( std::vector<int>({ 1, 2, 3, 4 }), vec );
    EXPECT_TRUE( mat_1.row( 3 ) != mat_1.row( 0 ) );
    EXPECT_FALSE( mat_1.row( 3 ) == mat_1.row( 0 ) );
    mat_1.row( 3 ) = vec;
    EXPECT_TRUE( mat_1.row( 3 ) == mat_1.row( 0 ) );
    EXPECT_EQ( 4, mat_1( 3, 3 ) );
    EXPECT_EQ( std::vector<int>({ 1, 2, 3, 4 }), static_cast<std::vector<int>>( mat_1.row( 3 ) ));

    // Explicit cast
    vec = static_cast<std::vector<int>>( mat_1.row(1) );
    EXPECT_EQ( 7, vec[3] );
    mat_1.row( 3 ) = vec;
    EXPECT_EQ( 7, mat_1( 3, 3 ) );

    // Explicit conversion.
    vec = mat_1.row( 2 ).to_vector();
    EXPECT_EQ( 17, vec[3] );

    // Copy row
    auto row_3 = mat_1.row( 3 );
    EXPECT_EQ( 7, row_3[ 3 ] );
    row_3.assign( mat_1.row( 2 ) );
    EXPECT_EQ( 17, row_3[ 3 ] );

    // Bad vector and row copy
    auto const bad_vec = static_cast<std::vector<int>>( mat_2.row(0) );
    EXPECT_ANY_THROW( row_3 = bad_vec );
    EXPECT_ANY_THROW( row_3.assign( mat_2.row(0) ));
    EXPECT_EQ( 17, row_3[ 3 ] );

    // Use iterator
    auto row_2 = mat_1.row( 2 );
    EXPECT_EQ( 42, row_2[ 0 ] );
    std::sort( row_2.begin(), row_2.end() );
    EXPECT_EQ( 13, row_2[ 0 ] );

    // Iterator details
    auto row_1 = mat_1.row( 1 );
    auto const it_0 = row_1.begin();

    // Deref
    EXPECT_EQ( 10, *it_0 );

    // Copy and post incr
    auto it_1 = it_0;
    EXPECT_EQ( 10, *it_1 );
    EXPECT_EQ( 10, *(it_1++) );
    EXPECT_EQ( 9, *it_1 );

    // Pre incr
    it_1 = it_0;
    EXPECT_EQ( 10, *it_1 );
    EXPECT_EQ( 9, *(++it_1) );
    EXPECT_EQ( 9, *it_1 );

    // Offset and post decr
    auto it_2 = it_0 + 2;
    EXPECT_EQ( 8, *it_2 );
    EXPECT_EQ( 8, *(it_2--) );
    EXPECT_EQ( 9, *it_2 );

    // Pre decr
    EXPECT_EQ( 9, *it_2 );
    EXPECT_EQ( 10, *(--it_2) );
    EXPECT_EQ( 10, *it_2 );

    // Assign plus
    EXPECT_EQ( 8, *(it_2 += 2) );
    EXPECT_EQ( 8, *it_2 );

    // Assign minus
    EXPECT_EQ( 9, *(it_2 -= 1) );
    EXPECT_EQ( 9, *it_2 );

    // Offset
    auto it_3 = it_0 + 2;
    EXPECT_EQ( 8, *it_3 );
    it_3 = 2 + it_0;
    EXPECT_EQ( 8, *it_3 );
    EXPECT_EQ( 9, *it_2 );
    it_3 = it_2 - 1;
    EXPECT_EQ( 10, *it_3 );

    // Access
    EXPECT_EQ( 9, it_0[1] );
    EXPECT_EQ( 10, it_2[-1] );

    // Equality
    EXPECT_EQ( 10, *it_0 );
    EXPECT_EQ(  9, *it_2 );
    EXPECT_EQ( 10, *it_3 );
    EXPECT_EQ( it_0, it_3 );
    EXPECT_NE( it_2, it_3 );

    // Difference
    EXPECT_EQ( -1, it_3 - it_2 );
    EXPECT_EQ( 0, it_3 - it_0 );

    // Comparison
    EXPECT_TRUE(  it_0 == it_0 );
    EXPECT_TRUE(  it_0 == it_3 );
    EXPECT_FALSE( it_0 == it_2 );
    EXPECT_TRUE(  it_0 != it_2 );
    EXPECT_FALSE( it_0 != it_3 );
    EXPECT_FALSE( it_0 != it_0 );
    EXPECT_TRUE(  it_0 <= it_2 );
    EXPECT_TRUE(  it_0 <= it_3 );
    EXPECT_TRUE(  it_0 <= it_0 );
    EXPECT_FALSE( it_2 <= it_0 );
    EXPECT_TRUE(  it_0 <  it_2 );
    EXPECT_FALSE( it_0 <  it_3 );
    EXPECT_FALSE( it_0 <  it_0 );
    EXPECT_TRUE(  it_2 >  it_3 );
    EXPECT_FALSE( it_0 >  it_3 );
    EXPECT_FALSE( it_0 >  it_0 );
    EXPECT_TRUE(  it_2 >= it_3 );
    EXPECT_TRUE(  it_3 >= it_3 );
    EXPECT_FALSE( it_3 >= it_2 );

    // Some more...
    it_3 = it_0 + 2;
    auto const it_4 = ++it_3;
    EXPECT_EQ( 7, *it_3 );
    EXPECT_EQ( 7, *it_4 );
    EXPECT_EQ( it_3, it_4 );
    EXPECT_EQ( 0, it_3 - it_4 );
    EXPECT_EQ( 3, it_4 - it_0 );
    EXPECT_EQ( -3, it_0 - it_3 );
    EXPECT_EQ( 9, it_0[1] );
    EXPECT_EQ( 9, it_4[-2] );

    // for( auto& e : row_0 ) {
    //     LOG_DBG << e;
    // }
}

TEST( Matrix, Col )
{
    auto mat_1 = Matrix<int>(4, 4, {
        1, 10, 42, 0,
        2, 9, 13, 0,
        3, 8, 69, 0,
        4, 7, 17, 0
    });
    auto mat_2 = Matrix<int>(5, 1, {
        1, 2, 3, 4, 5
    });

    // Basic access
    auto col_0 = mat_1.col( 0 );
    EXPECT_EQ( 4, col_0.size());
    EXPECT_EQ( 2, col_0.at( 1 ));
    EXPECT_EQ( 3, col_0[ 2 ]);
    EXPECT_EQ( 0, col_0.col() );
    EXPECT_EQ( mat_1, col_0.matrix() );
    EXPECT_EQ( &mat_1, &col_0.matrix() );

    // Multiple instances should compare equal
    EXPECT_EQ( mat_1.col( 0 ), mat_1.col( 0 ) );
    EXPECT_TRUE( mat_1.col( 0 ) == mat_1.col( 0 ) );

    // Multiple iterators should be equal
    EXPECT_EQ( mat_1.col( 0 ).begin(), mat_1.col( 0 ).begin() );
    EXPECT_TRUE( mat_1.col( 0 ).begin() == mat_1.col( 0 ).begin() );

    // Copy to and from vector
    std::vector<int> vec;
    vec = col_0;
    EXPECT_EQ( 4, vec[3] );
    EXPECT_EQ( std::vector<int>({ 1, 2, 3, 4 }), vec );
    EXPECT_TRUE( mat_1.col( 3 ) != mat_1.col( 0 ) );
    EXPECT_FALSE( mat_1.col( 3 ) == mat_1.col( 0 ) );
    mat_1.col( 3 ) = vec;
    EXPECT_TRUE( mat_1.col( 3 ) == mat_1.col( 0 ) );
    EXPECT_EQ( 4, mat_1( 3, 3 ) );
    EXPECT_EQ( std::vector<int>({ 1, 2, 3, 4 }), static_cast<std::vector<int>>( mat_1.col( 3 ) ));

    // Explicit cast
    vec = static_cast<std::vector<int>>( mat_1.col(1) );
    EXPECT_EQ( 7, vec[3] );
    mat_1.col( 3 ) = vec;
    EXPECT_EQ( 7, mat_1( 3, 3 ) );

    // Explicit conversion.
    vec = mat_1.col( 2 ).to_vector();
    EXPECT_EQ( 17, vec[3] );

    // Copy col
    auto col_3 = mat_1.col( 3 );
    EXPECT_EQ( 7, col_3[ 3 ] );
    col_3.assign( mat_1.col( 2 ) );
    EXPECT_EQ( 17, col_3[ 3 ] );

    // Bad vector and col copy
    auto const bad_vec = static_cast<std::vector<int>>( mat_2.col(0) );
    EXPECT_ANY_THROW( col_3 = bad_vec );
    EXPECT_ANY_THROW( col_3.assign( mat_2.col(0) ));
    EXPECT_EQ( 17, col_3[ 3 ] );

    // Use iterator
    auto col_2 = mat_1.col( 2 );
    EXPECT_EQ( 42, col_2[ 0 ] );
    std::sort( col_2.begin(), col_2.end() );
    EXPECT_EQ( 13, col_2[ 0 ] );

    // Iterator details
    auto col_1 = mat_1.col( 1 );
    auto const it_0 = col_1.begin();

    // Deref
    EXPECT_EQ( 10, *it_0 );

    // Copy and post incr
    auto it_1 = it_0;
    EXPECT_EQ( 10, *it_1 );
    EXPECT_EQ( 10, *(it_1++) );
    EXPECT_EQ( 9, *it_1 );

    // Pre incr
    it_1 = it_0;
    EXPECT_EQ( 10, *it_1 );
    EXPECT_EQ( 9, *(++it_1) );
    EXPECT_EQ( 9, *it_1 );

    // Offset and post decr
    auto it_2 = it_0 + 2;
    EXPECT_EQ( 8, *it_2 );
    EXPECT_EQ( 8, *(it_2--) );
    EXPECT_EQ( 9, *it_2 );

    // Pre decr
    EXPECT_EQ( 9, *it_2 );
    EXPECT_EQ( 10, *(--it_2) );
    EXPECT_EQ( 10, *it_2 );

    // Assign plus
    EXPECT_EQ( 8, *(it_2 += 2) );
    EXPECT_EQ( 8, *it_2 );

    // Assign minus
    EXPECT_EQ( 9, *(it_2 -= 1) );
    EXPECT_EQ( 9, *it_2 );

    // Offset
    auto it_3 = it_0 + 2;
    EXPECT_EQ( 8, *it_3 );
    it_3 = 2 + it_0;
    EXPECT_EQ( 8, *it_3 );
    EXPECT_EQ( 9, *it_2 );
    it_3 = it_2 - 1;
    EXPECT_EQ( 10, *it_3 );

    // Access
    EXPECT_EQ( 9, it_0[1] );
    EXPECT_EQ( 10, it_2[-1] );

    // Equality
    EXPECT_EQ( 10, *it_0 );
    EXPECT_EQ(  9, *it_2 );
    EXPECT_EQ( 10, *it_3 );
    EXPECT_EQ( it_0, it_3 );
    EXPECT_NE( it_2, it_3 );

    // Difference
    EXPECT_EQ( -1, it_3 - it_2 );
    EXPECT_EQ( 0, it_3 - it_0 );

    // Comparison
    EXPECT_TRUE(  it_0 == it_0 );
    EXPECT_TRUE(  it_0 == it_3 );
    EXPECT_FALSE( it_0 == it_2 );
    EXPECT_TRUE(  it_0 != it_2 );
    EXPECT_FALSE( it_0 != it_3 );
    EXPECT_FALSE( it_0 != it_0 );
    EXPECT_TRUE(  it_0 <= it_2 );
    EXPECT_TRUE(  it_0 <= it_3 );
    EXPECT_TRUE(  it_0 <= it_0 );
    EXPECT_FALSE( it_2 <= it_0 );
    EXPECT_TRUE(  it_0 <  it_2 );
    EXPECT_FALSE( it_0 <  it_3 );
    EXPECT_FALSE( it_0 <  it_0 );
    EXPECT_TRUE(  it_2 >  it_3 );
    EXPECT_FALSE( it_0 >  it_3 );
    EXPECT_FALSE( it_0 >  it_0 );
    EXPECT_TRUE(  it_2 >= it_3 );
    EXPECT_TRUE(  it_3 >= it_3 );
    EXPECT_FALSE( it_3 >= it_2 );

    // Some more...
    it_3 = it_0 + 2;
    auto const it_4 = ++it_3;
    EXPECT_EQ( 7, *it_3 );
    EXPECT_EQ( 7, *it_4 );
    EXPECT_EQ( it_3, it_4 );
    EXPECT_EQ( 0, it_3 - it_4 );
    EXPECT_EQ( 3, it_4 - it_0 );
    EXPECT_EQ( -3, it_0 - it_3 );
    EXPECT_EQ( 9, it_0[1] );
    EXPECT_EQ( 9, it_4[-2] );

    // for( auto& e : col_0 ) {
    //     LOG_DBG << e;
    // }
}
