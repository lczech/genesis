/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief Testing Matrix class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <ctime>
#include <sstream>

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"

using namespace genesis;
using namespace utils;

TEST(Matrix, Transpose)
{
    // Empty
    auto e = Matrix<int>();
    EXPECT_EQ( Matrix<int>(), transpose(e) );
    transpose_inplace( e );
    EXPECT_EQ( Matrix<int>(), e );

    // Single entry
    auto s = Matrix<int>( 1, 1, {1} );
    EXPECT_EQ( Matrix<int>( 1, 1, {1} ), transpose(s) );
    transpose_inplace( s );
    EXPECT_EQ( Matrix<int>( 1, 1, {1} ), s );

    {
        // Multiple entries symmetrical
        auto m = Matrix<int>(2, 2, {1, 2, 3, 4});
        auto t = Matrix<int>(2, 2, {1, 3, 2, 4});

        // With new allocations
        EXPECT_EQ( t, transpose( m ));
        EXPECT_EQ( m, transpose( t ));
        EXPECT_EQ( m, transpose( t ));
        EXPECT_EQ( m, transpose( transpose( m )));
        EXPECT_EQ( t, transpose( transpose( t )));

        // Inplace transpose
        auto n = m;
        transpose_inplace( n );
        EXPECT_EQ( t, n );
        transpose_inplace( n );
        EXPECT_EQ( m, n );
    }

    {
        // Multiple entries asymmetrical
        auto m = Matrix<int>(2, 3, {1, 2, 3, 4, 5, 6});
        auto t = Matrix<int>(3, 2, {1, 4, 2, 5, 3, 6});

        // With new allocations
        EXPECT_EQ( t, transpose( m ));
        EXPECT_EQ( m, transpose( t ));
        EXPECT_EQ( m, transpose( t ));
        EXPECT_EQ( m, transpose( transpose( m )));
        EXPECT_EQ( t, transpose( transpose( t )));

        // Inplace transpose
        auto n = m;
        transpose_inplace( n );
        EXPECT_EQ( t, n );
        transpose_inplace( n );
        EXPECT_EQ( m, n );
    }
}

TEST(Matrix, TransposeFuzzy)
{
    std::srand(std::time(nullptr));

    // Run for some iterations so that we do not exceed 0.5s roughly.
    size_t const max_size = 1024;
    size_t const max_val = 1024;
    size_t const max_it = 50;
    for( size_t i = 0; i < max_it; ++i ) {

        // Size of the bitvector
        size_t const m = std::rand() % max_size;
        size_t const n = std::rand() % max_size;
        auto mat = Matrix<int>( m, n );

        // Set some random entries
        for( size_t r = 0; r < m; ++r ) {
            for( size_t c = 0; c < n; ++c ) {
                mat( r, c ) = std::rand() % max_val;
            }
        }

        // Do some wild back and forth with different methods.
        // LOG_TIME << "start " << "m==" << m << ", n==" << n;
        auto const tr1 = transpose( mat );
        // LOG_TIME << "transpose 1";
        transpose_inplace( mat );
        // LOG_TIME << "transpose_inplace 1";
        EXPECT_EQ( tr1, mat ) << "m==" << m << ", n==" << n;

        // ... and back again.
        // LOG_TIME << "comp 1";
        auto const tr2 = transpose( mat );
        // LOG_TIME << "transpose 2";
        transpose_inplace( mat );
        // LOG_TIME << "transpose_inplace";
        EXPECT_EQ( tr2, mat ) << "m==" << m << ", n==" << n;
        // LOG_TIME << "comp 2";
    }
}
