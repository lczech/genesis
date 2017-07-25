/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include <vector>

#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/operators.hpp"

using namespace genesis;
using namespace utils;

TEST(Matrix, MultiplicationMM)
{
    auto a = Matrix<double>( 2, 3, {
        3, 2, 1,
        1, 0, 2
    });
    auto b = Matrix<double>( 3, 2, {
        1, 2,
        0, 1,
        4, 0
    });
    auto r = Matrix<double>( 2, 2, {
        7, 8,
        9, 2
    });

    EXPECT_EQ( r, matrix_multiplication( a, b ));
}

TEST(Matrix, MultiplicationVM)
{
    auto a = std::vector<double>({ 1, 2, 3 });
    auto b = Matrix<double>( 3, 3, {
        2, 1, 3,
        3, 3, 2,
        4, 1, 2
    });
    auto r = std::vector<double>({ 20, 10, 13 });

    EXPECT_EQ( r, matrix_multiplication( a, b ));
}

TEST(Matrix, MultiplicationMV)
{
    auto a = Matrix<double>( 3, 3, {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    });
    auto b = std::vector<double>({ 2, 1, 3 });
    auto r = std::vector<double>({ 13, 31, 49 });

    EXPECT_EQ( r, matrix_multiplication( a, b ));
}
