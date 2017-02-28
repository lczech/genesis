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

#include <sstream>

#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/operators.hpp"

using namespace genesis;
using namespace utils;

TEST(Matrix, Copy)
{
    auto m1 = Matrix<int>(2, 2, {1, 2, 3, 4});

    auto m2 = m1;
    auto m3(m1);

    EXPECT_EQ(m1, m2);
    EXPECT_EQ(m1, m3);

    EXPECT_TRUE(m1 == m2);
    EXPECT_TRUE(m1 == m3);

    EXPECT_FALSE(m1 != m2);
    EXPECT_FALSE(m1 != m3);

    m1(0, 1) = 9;

    EXPECT_NE(m1, m2);
    EXPECT_NE(m1, m3);

    EXPECT_FALSE(m1 == m2);
    EXPECT_FALSE(m1 == m3);

    EXPECT_TRUE(m1 != m2);
    EXPECT_TRUE(m1 != m3);

    EXPECT_EQ(9, m1(0, 1));
    EXPECT_EQ(2, m2(0, 1));
    EXPECT_EQ(2, m3(0, 1));
}

TEST(Matrix, InitializerList)
{
    EXPECT_THROW(
        Matrix<int>(2, 2, {1, 2, 3}),
        std::length_error
    );

    EXPECT_NO_THROW(
        Matrix<int>(2, 2, {1, 2, 3, 4})
    );

    EXPECT_THROW(
        Matrix<int>(2, 2, {1, 2, 3, 4, 5}),
        std::length_error
    );
}

TEST(Matrix, Access)
{
    auto m = Matrix<int>(2, 2, {1, 2, 3, 4});

    EXPECT_EQ(2, m.rows());
    EXPECT_EQ(2, m.cols());

    EXPECT_EQ(2, m(0, 1));

    EXPECT_THROW(
        m.at(0, 2),
        std::out_of_range
    );
}

TEST( Matrix, SwapRowsCols )
{
    auto m = Matrix<int>(4, 4, {
         1,  2,  3,  4,
         5,  6,  7,  8,
         9, 10, 11, 12,
        13, 14, 15, 16
    });

    auto mre = Matrix<int>(4, 4, {
         1,  2,  3,  4,
        13, 14, 15, 16,
         9, 10, 11, 12,
         5,  6,  7,  8
    });

    swap_rows( m, 1, 3 );
    EXPECT_EQ( mre, m );

    auto mce = Matrix<int>(4, 4, {
         2,  1,  3,  4,
        14, 13, 15, 16,
        10,  9, 11, 12,
         6,  5,  7,  8
    });

    swap_cols( m, 1, 0 );
    EXPECT_EQ( mce, m );
}

TEST(Matrix, Ostream)
{
    auto m = Matrix<int>(2, 2, {1, 2, 3, 4});
    std::ostringstream s;
    s << m;
    EXPECT_EQ("1 2\n3 4\n", s.str());
}
