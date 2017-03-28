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

#include "genesis/utils/math/random.hpp"

using namespace genesis;
using namespace utils;

TEST( Math, SelectWithoutReplacement )
{
    size_t const k = 2; // sample size
    size_t const n = 5; // population size
    size_t const r = 10000; // number of runs

    auto histogram = std::vector<size_t>( n, 0 );

    for( size_t i = 0; i < r; ++i ) {
        auto selection = select_without_replacement( k, n );
        for( auto const& s : selection ) {
            ++histogram[ s ];
        }
    }

    // We cannot really test the validity automatically, as it is random.
    // Thus, just do a simple test whether we generated the correct number of elements.
    size_t sum = 0;
    for( size_t i = 0; i < histogram.size(); ++i ) {
        // LOG_DBG << i << ": " << histogram[ i ];
        sum += histogram[ i ];
    }
    EXPECT_EQ( k * r, sum );
}
