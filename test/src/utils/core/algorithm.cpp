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
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/core/algorithm.hpp"

#include <vector>

using namespace genesis::utils;

TEST( Algorithm, SortIndices )
{
    auto const list = std::vector<int>({ 3, 100, -1, 0, 13, 42, -32 });

    auto exp_less_order = list;
    std::sort( exp_less_order.begin(), exp_less_order.end() );
    auto act_less_order = sort_indices( list.begin(), list.end() );

    for( size_t i = 0; i < act_less_order.size(); ++i ) {
        EXPECT_EQ( exp_less_order[i], list[ act_less_order[i] ] );
    }

    auto exp_greater_order = list;
    std::sort( exp_greater_order.begin(), exp_greater_order.end(), std::greater<int>() );
    auto act_greater_order = sort_indices( list.begin(), list.end(), std::greater<int>() );

    for( size_t i = 0; i < act_greater_order.size(); ++i ) {
        EXPECT_EQ( exp_greater_order[i], list[ act_greater_order[i] ] );
    }
}
