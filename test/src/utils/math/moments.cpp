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
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/math/moments.hpp"

#include<limits>

using namespace genesis::utils;

TEST( Math, Moments )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  Moments({ }).mean() );
    EXPECT_DOUBLE_EQ( 0.0,  Moments({ }).stddev() );

    // One value.
    EXPECT_DOUBLE_EQ(  1.0,  Moments({ 1.0 }).mean() );
    EXPECT_DOUBLE_EQ(  0.0,  Moments({ 1.0 }).stddev() );
    EXPECT_DOUBLE_EQ(  5.0,  Moments({ 5.0 }).mean() );
    EXPECT_DOUBLE_EQ(  0.0,  Moments({ 5.0 }).stddev() );

    // Two values.
    EXPECT_DOUBLE_EQ( 5.0,  Moments({ 2.0, 8.0 }).mean() );
    EXPECT_DOUBLE_EQ( 3.0,  Moments({ 2.0, 8.0 }).stddev() );
    EXPECT_DOUBLE_EQ( 6.0,  Moments({ 6.0, 6.0 }).mean() );
    EXPECT_DOUBLE_EQ( 0.0,  Moments({ 6.0, 6.0 }).stddev() );

    // Three values.
    EXPECT_DOUBLE_EQ( 7.0,                Moments({ 4.0, 6.0, 11.0 }).mean() );
    EXPECT_DOUBLE_EQ( 2.9439202887759488, Moments({ 4.0, 6.0, 11.0 }).stddev() );
}
