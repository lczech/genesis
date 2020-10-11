/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020

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

#include "genesis/population/functions/statistics.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Population, Statistics )
{
    // Run loop for some lower values.
    for( size_t p = 1; p < 50; ++p ) {
        for( size_t c = 1; c < 3 * p; ++c ) {
            EXPECT_FLOAT_EQ( n_base_matrix( c, p ), n_base( c, p ));
        }
    }

    // Also test some higher values. We request the higher coverage values first in order to
    // avoid recomputing the matrix as much as possible.
    EXPECT_FLOAT_EQ( n_base_matrix(  500,  100 ), n_base(  500,  100 ));
    EXPECT_FLOAT_EQ( n_base_matrix(  100,  100 ), n_base(  100,  100 ));
    EXPECT_FLOAT_EQ( n_base_matrix(   50,  100 ), n_base(   50,  100 ));
    EXPECT_FLOAT_EQ( n_base_matrix( 2000, 1000 ), n_base( 2000, 1000 ));
    EXPECT_FLOAT_EQ( n_base_matrix(  500, 1000 ), n_base(  500, 1000 ));
    EXPECT_FLOAT_EQ( n_base_matrix(  100, 1000 ), n_base(  100, 1000 ));
}
