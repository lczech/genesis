#ifndef GENESIS_POPULATION_FUNCTIONS_STATISTICS_H_
#define GENESIS_POPULATION_FUNCTIONS_STATISTICS_H_

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
#include "genesis/utils/containers/simple_cache.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Diversity Estimates
// =================================================================================================

double heterozygosity( PoolSample const& sample );

// =================================================================================================
//     Theta Pi
// =================================================================================================

double theta_pi_pool_denominator( size_t poolsize, size_t min_allele_count, size_t nucleotide_count );

template<class ForwardIterator>
double theta_pi(
    ForwardIterator begin, ForwardIterator end
) {
    double pi_sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        pi_sum += heterozygosity( *it );
    }
    return pi_sum;
}

template<class ForwardIterator>
double theta_pi_pool( // get_pi_calculator
    ForwardIterator begin,
    ForwardIterator end,
    size_t poolsize,        // n
    size_t min_allele_count // b
) {
    double pi_sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        double pi_snp = heterozygosity( *it );
        pi_snp /= theta_pi_pool_denominator( poolsize, min_allele_count, nucleotide_sum( *it ) );
        pi_sum += pi_snp;
    }
    return pi_sum;
}

// =================================================================================================
//     Cache Access
// =================================================================================================

#ifdef DEBUG

genesis::utils::SimpleCache<double, size_t, size_t, size_t>& amnm_cache();
genesis::utils::SimpleCache<double, size_t, size_t, size_t>& theta_pi_pool_denominator_cache();

#endif // DEBUG

} // namespace population
} // namespace genesis

#endif // include guard
