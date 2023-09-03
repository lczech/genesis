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

#include "genesis/utils/math/ranking.hpp"

using namespace genesis;
using namespace utils;

TEST( Math, RankingNFirstElements )
{
    auto const empty = std::vector<double>{};
    auto const values = std::vector<double>{ 6.0, -1.0, 4.0, 0.0, -5.0, -3.0, 2.0 };

    // Edge cases
    {
        // No elements selected
        auto const res_0 = n_first_elements(
            values.begin(), values.end(), 0
        );
        EXPECT_EQ( 0, res_0.size() );

        // Empty input
        auto const res_e = n_first_elements(
            empty.begin(), empty.end(), 5
        );
        EXPECT_EQ( 0, res_e.size() );

        // Empty input and no elements
        auto const res_e0 = n_first_elements(
            empty.begin(), empty.end(), 0
        );
        EXPECT_EQ( 0, res_e0.size() );
    }

    // Non-abs values
    {
        // Smaller n than input, smallest values
        auto const res_lt_3 = n_first_elements(
            values.begin(), values.end(), 3
        );
        auto const exp_lt_3 = std::vector<double>{ -5.0, -3.0, -1.0 };
        EXPECT_EQ( exp_lt_3, res_lt_3 );

        // Smaller n than input, largest values
        auto const res_gt_3 = n_first_elements(
            values.begin(), values.end(), 3, std::greater<double>{}
        );
        auto const exp_gt_3 = std::vector<double>{ 6.0, 4.0, 2.0 };
        EXPECT_EQ( exp_gt_3, res_gt_3 );

        // Larger n than input, smallest values
        auto const res_lt_8 = n_first_elements(
            values.begin(), values.end(), 8
        );
        auto const exp_lt_8 = std::vector<double>{ -5.0, -3.0, -1.0, 0.0, 2.0, 4.0, 6.0 };
        EXPECT_EQ( exp_lt_8, res_lt_8 );

        // Larger n than input, largest values
        auto const res_gt_8 = n_first_elements(
            values.begin(), values.end(), 8, std::greater<double>{}
        );
        auto const exp_gt_8 = std::vector<double>{ 6.0, 4.0, 2.0, 0.0, -1.0, -3.0, -5.0  };
        EXPECT_EQ( exp_gt_8, res_gt_8 );
    }

    // Abs values
    {
        // Absolute comparison functions
        auto abs_less = []( double l, double r ){
            return std::abs( l ) < std::abs( r );
        };
        auto abs_greater = []( double l, double r ){
            return std::abs( l ) > std::abs( r );
        };

        // Smaller n than input, smallest values
        auto const res_lt_3 = n_first_elements(
            values.begin(), values.end(), 3, abs_less
        );
        auto const exp_lt_3 = std::vector<double>{ 0.0, -1.0, 2.0 };
        EXPECT_EQ( exp_lt_3, res_lt_3 );

        // Smaller n than input, largest values
        auto const res_gt_3 = n_first_elements(
            values.begin(), values.end(), 3, abs_greater
        );
        auto const exp_gt_3 = std::vector<double>{ 6.0, -5.0, 4.0 };
        EXPECT_EQ( exp_gt_3, res_gt_3 );

        // Larger n than input, smallest values
        auto const res_lt_8 = n_first_elements(
            values.begin(), values.end(), 8, abs_less
        );
        auto const exp_lt_8 = std::vector<double>{ 0.0, -1.0, 2.0, -3.0, 4.0, -5.0, 6.0 };
        EXPECT_EQ( exp_lt_8, res_lt_8 );

        // Larger n than input, largest values
        auto const res_gt_8 = n_first_elements(
            values.begin(), values.end(), 8, abs_greater
        );
        auto const exp_gt_8 = std::vector<double>{ 6.0, -5.0, 4.0, -3.0, 2.0, -1.0, 0.0 };
        EXPECT_EQ( exp_gt_8, res_gt_8 );
    }
}
