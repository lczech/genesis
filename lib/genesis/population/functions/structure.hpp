#ifndef GENESIS_POPULATION_FUNCTIONS_STRUCTURE_H_
#define GENESIS_POPULATION_FUNCTIONS_STRUCTURE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @ingroup population
 */

#include "genesis/population/functions/pool_sample.hpp"
#include "genesis/population/pool_sample.hpp"
#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     F_ST Conventional
// =================================================================================================

/**
 * @brief Compute the SNP-based pi values used in f_st_conventional_pool()
 *
 * See there for details.
 */
std::tuple<double, double, double> f_st_conventional_pool_pi_snp(
    PoolSample const& p1, PoolSample const& p2
);

/**
 * @brief Compute the conventional F_ST statistic for pool-sequenced data,
 * following Kofler et al.
 */
template<class ForwardIterator1, class ForwardIterator2>
double f_st_conventional_pool( // get_conventional_fstcalculator
    size_t p1_poolsize, size_t p2_poolsize,
    ForwardIterator1 p1_begin, ForwardIterator1 p1_end,
    ForwardIterator2 p2_begin, ForwardIterator2 p2_end
) {
    // Edge and error cases
    if( p1_poolsize <= 1 || p2_poolsize <= 1 ) {
        throw std::invalid_argument( "Cannot run f_st_conventional_pool() with poolsizes <= 1" );
    }

    // Theta Pi values for the two populations and their combination
    double p1_pi_sum = 0.0;
    double p2_pi_sum = 0.0;
    double pp_pi_sum = 0.0;

    auto p1_it = p1_begin;
    auto p2_it = p2_begin;
    while( p1_it != p1_end && p2_it != p2_end ) {

        // Compute frequency based pi snps. The tuple returns p1, p2, pp, in that order.
        auto const pi_snps = f_st_conventional_pool_pi_snp( *p1_it, *p2_it );
        p1_pi_sum += std::get<0>( pi_snps );
        p2_pi_sum += std::get<1>( pi_snps );
        pp_pi_sum += std::get<2>( pi_snps );

        // Next pair of entries
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument(
            "In f_st_conventional_pool(): Provided ranges have different length."
        );
    }

    // Normalize by poolsize
    assert( p1_poolsize > 1 && p2_poolsize > 1 );
    size_t const pp_poolsize = std::min( p1_poolsize, p2_poolsize );
    p1_pi_sum *= static_cast<double>( p1_poolsize ) / static_cast<double>( p1_poolsize - 1 );
    p2_pi_sum *= static_cast<double>( p2_poolsize ) / static_cast<double>( p2_poolsize - 1 );
    pp_pi_sum *= static_cast<double>( pp_poolsize ) / static_cast<double>( pp_poolsize - 1 );

    // _calculateFstValues
    double const pp_avg = ( p1_pi_sum + p2_pi_sum ) / 2.0;
    return ( pp_pi_sum - pp_avg ) / pp_pi_sum;
}

} // namespace population
} // namespace genesis

#endif // include guard
