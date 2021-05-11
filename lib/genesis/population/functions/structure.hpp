#ifndef GENESIS_POPULATION_FUNCTIONS_STRUCTURE_H_
#define GENESIS_POPULATION_FUNCTIONS_STRUCTURE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
 * @ingroup population
 */

#include "genesis/population/functions/variant.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/transform_iterator.hpp"

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
 * @brief Compute the SNP-based Theta Pi values used in f_st_conventional_pool()
 *
 * See there for details. The tuple returns Theta Pi for an individual position, which is simply
 * the heterozygosity() at this position, for both samples @p p1 and @p p2, as well as their
 * combined (average frequency) heterozygosity, in that order.
 */
std::tuple<double, double, double> f_st_conventional_pool_pi_snp(
    BaseCounts const& p1, BaseCounts const& p2
);

/**
 * @brief Compute the conventional F_ST statistic for pool-sequenced data,
 * following Kofler et al, for two ranges of BaseCounts%s.
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

        // Skip invalid entries than can happen when less than two of [ACGT] have counts > 0
        // in one of the BaseCounts samples.
        if(
            std::isfinite( std::get<0>( pi_snps )) &&
            std::isfinite( std::get<1>( pi_snps )) &&
            std::isfinite( std::get<2>( pi_snps ))
        ) {
            // If we are here, both p1 and p2 have counts. Let's assert.
            assert( p1_it->a_count + p1_it->c_count + p1_it->g_count + p1_it->t_count > 0 );
            assert( p2_it->a_count + p2_it->c_count + p2_it->g_count + p2_it->t_count > 0 );

            // Now add them to the tally.
            p1_pi_sum += std::get<0>( pi_snps );
            p2_pi_sum += std::get<1>( pi_snps );
            pp_pi_sum += std::get<2>( pi_snps );
        } else {
            // If we are here, at least one of the two inputs has one or fewer counts in [ACGT],
            // otherwise, the results would have been finite. Let's assert this.
            assert(
                ( p1_it->a_count + p1_it->c_count + p1_it->g_count + p1_it->t_count <= 1 ) ||
                ( p2_it->a_count + p2_it->c_count + p2_it->g_count + p2_it->t_count <= 1 )
            );
        }

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

/**
 * @brief Compute the conventional F_ST statistic for pool-sequenced data,
 * following Kofler et al, for all pairs of ranges of BaseCounts%s.
 *
 * The function is intended to be used for computing pairwise F_ST for a set of BaseCounts%s along
 * some region (e.g., a genomic Window).
 *
 * This function expects a range (for example, a Window over the genome) of iterators,
 * where each iterator dereferences to a `std::vector<BaseCounts>`. Each entry in the range is
 * used as one position in the genome contributing to F_ST. For all entries, the `vector` needs
 * to have the same number of entries, and that number has also to be the same as the size of
 * the given @p poolsizes vector.
 *
 * Then, for each pair `(i,j)` of pool samples, the range is iterated, and the respective entries
 * `i` and `j` of the vectors in the range are used to compute F_ST for this pair of samples,
 * and stored in the resulting matrix at positions `(i,j)` and `(j,i)`.
 */
template<class ForwardIterator>
utils::Matrix<double> f_st_conventional_pool(
    std::vector<size_t> const& poolsizes,
    ForwardIterator begin, ForwardIterator end
) {
    auto result = utils::Matrix<double>( poolsizes.size(), poolsizes.size(), 0.0 );

    // We use a lambda that returns a tranforming rage to select an entry at a given index
    // in the set of BaseCounts at the current iterator position.
    auto const pss = poolsizes.size();
    auto select_entry = [pss]( ForwardIterator begin, ForwardIterator end, size_t index ){
        // Currently, in order to use Window here, we need to explicitly use std::vector<BaseCounts>
        // instead of the more generic T... Bit unfortunate, but good enough for now.
        // Will have to revisit later if we get to use cases where the BaseCounts are not stored
        // in a vector, but some other container.
        // using T = typename ForwardIterator::value_type;
        return utils::make_transform_range(
            [pss, index]( std::vector<BaseCounts> const& pools ) -> BaseCounts const& {
                if( pools.size() != pss ) {
                    throw std::runtime_error(
                        "In f_st_conventional_pool(): Provided number of poolsizes (" +
                        std::to_string( pss ) + ") is not equal to the number of BaseCounts (" +
                        std::to_string( pools.size() ) + ") at some point in the iteration"
                    );
                }
                return pools[index];
            },
            begin, end
        );
    };

    // Loop over all pairs of entries, and compute f_st for each of these pairs.
    // That is, in the inner code of the two loops, we run the f_st function that takes
    // two ranges, providing it with a pair of indices for which we compute the value.
    for( size_t i = 0; i < poolsizes.size(); ++i ) {
        for( size_t j = i + 1; j < poolsizes.size(); ++j ) {
            auto range_i = select_entry( begin, end, i );
            auto range_j = select_entry( begin, end, j );
            auto const fst = f_st_conventional_pool(
                poolsizes[i], poolsizes[j],
                range_i.begin(), range_i.end(),
                range_j.begin(), range_j.end()
            );
            result( i, j ) = fst;
            result( j, i ) = fst;
        }
    }

    return result;
}

/**
 * @brief Compute the conventional F_ST statistic for pool-sequenced data,
 * following Kofler et al, for all pairs of ranges of BaseCounts%s.
 *
 * This is a shortcut for
 * @link f_st_conventional_pool( std::vector<size_t> const&, ForwardIterator, ForwardIterator ) f_st_conventional_pool@endlink,
 * but instead of taking a vector of the sizes of each pool, it uses the number of samples
 * and a fix poolsize that is used for all samples.
 */
template<class ForwardIterator>
utils::Matrix<double> f_st_conventional_pool(
    size_t number_of_samples, size_t poolsizes,
    ForwardIterator begin, ForwardIterator end
) {
    // With no data, return empty result
    if( begin == end ) {
        return {};
    }

    // With data: get the number of samples, and use that to fill a vector with identical pool sizes.
    // Then, use the other version of this function to compute the result.
    auto const ps = std::vector<size_t>( number_of_samples, poolsizes );
    return f_st_conventional_pool( ps, begin, end );
}

// =================================================================================================
//     F_ST Asymptotically Unbiased (Karlsson)
// =================================================================================================

/**
 * @brief Helper struct for the `a_1`, `a_2`, `n_1`, and `n_2` values needed for
 * f_st_asymptotically_unbiased().
 */
struct FstAN
{
    double a_1 = 0.0;
    double n_1 = 0.0;
    double a_2 = 0.0;
    double n_2 = 0.0;
};

/**
 * @brief Comparison operator equals for FstAN structs.
 */
inline bool operator ==( FstAN const& f1, FstAN const& f2 )
{
    return f1.a_1 == f2.a_1 && f1.n_1 == f2.n_1 && f1.a_2 == f2.a_2 && f1.n_2 == f2.n_2;
}

/**
 * @brief Compute the `a` and `n` values needed for the asymptotically unbiased F_ST estimator
 * of Karlsson et al.
 *
 * See f_st_asymptotically_unbiased() for details.
 */
FstAN f_st_asymptotically_unbiased_a1n1a2n2( BaseCounts const& p1, BaseCounts const& p2 );

/**
 * @brief Compute the `N_k` and `D_k` values needed for the asymptotically unbiased F_ST estimator
 * of Karlsson et al.
 *
 * See f_st_asymptotically_unbiased() for details.
 */
std::pair<double, double> f_st_asymptotically_unbiased_nkdk( FstAN const& fstan );

/**
 * @brief Compute the asymptotically unbiased F_ST estimator of Karlsson et al.
 *
 * This follows the implementation in PoPoolation2 by Kofler et al.
 */
template<class ForwardIterator1, class ForwardIterator2>
double f_st_asymptotically_unbiased( // get_asymptunbiased_fstcalculator
    ForwardIterator1 p1_begin, ForwardIterator1 p1_end,
    ForwardIterator2 p2_begin, ForwardIterator2 p2_end
) {
    using namespace genesis::utils;

    // Result value.
    double sum_nk = 0.0;
    double sum_dk = 0.0;

    // Iterate both ranges, summing up N_k and D_k for all their entries.
    auto p1_it = p1_begin;
    auto p2_it = p2_begin;
    while( p1_it != p1_end && p2_it != p2_end ) {

        // Get intermediate values and add them up.
        auto const anan = f_st_asymptotically_unbiased_a1n1a2n2( *p1_it, *p2_it );
        auto const nkdk = f_st_asymptotically_unbiased_nkdk( anan );
        sum_nk += nkdk.first;
        sum_dk += nkdk.second;

        // Next pair of entries
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument(
            "In f_st_asymptotically_unbiased(): Provided ranges have different length."
        );
    }

    return sum_nk / sum_dk;
}

/**
 * @brief Compute the asymptotically unbiased F_ST estimator of Karlsson et al,
 * for all pairs of ranges of BaseCounts%s.
 *
 * The function is intended to be used for computing pairwise F_ST for a set of BaseCounts%s along
 * some region (e.g., a genomic Window).
 *
 * This function expects a range (for example, a Window over the genome) of iterators,
 * where each iterator dereferences to a `std::vector<BaseCounts>`. Each entry in the range is
 * used as one position in the genome contributing to F_ST. For all entries, the `vector` needs
 * to have the same number of entries.
 *
 * Then, for each pair `(i,j)` of pool samples, the range is iterated, and the respective entries
 * `i` and `j` of the vectors in the range are used to compute F_ST for this pair of samples,
 * and stored in the resulting matrix at positions `(i,j)` and `(j,i)`.
 */
template<class ForwardIterator>
utils::Matrix<double> f_st_asymptotically_unbiased(
    ForwardIterator begin, ForwardIterator end
) {
    // With no data, return empty result
    if( begin == end ) {
        return {};
    }

    // Now we now that there are entries in the rage. Use the first one to get the number of
    // pool samples in the ragen. We later check that this is the same for each entry in the range.
    // Use that size to initialize the resulting matrix.
    size_t const ps = static_cast<std::vector<BaseCounts> const&>( *begin ).size();
    auto result = utils::Matrix<double>( ps, ps, 0.0 );

    // We use a lambda that returns a tranforming rage to select an entry at a given index
    // in the set of BaseCounts at the current iterator position.
    auto select_entry = [ps]( ForwardIterator begin, ForwardIterator end, size_t index ){
        return utils::make_transform_range(
            [ps, index]( std::vector<BaseCounts> const& pools ) -> BaseCounts const& {
                if( pools.size() != ps ) {
                    throw std::runtime_error(
                        "In f_st_asymptotically_unbiased(): The number of BaseCounts in the "
                        "provided range is not consistent"
                    );
                }
                return pools[index];
            },
            begin, end
        );
    };

    // Loop over all pairs of entries, and compute f_st for each of these pairs.
    // That is, in the inner code of the two loops, we run the f_st function that takes
    // two ranges, providing it with a pair of indices for which we compute the value.
    for( size_t i = 0; i < ps; ++i ) {
        for( size_t j = i + 1; j < ps; ++j ) {
            auto range_i = select_entry( begin, end, i );
            auto range_j = select_entry( begin, end, j );
            auto const fst = f_st_asymptotically_unbiased(
                range_i.begin(), range_i.end(),
                range_j.begin(), range_j.end()
            );
            result( i, j ) = fst;
            result( j, i ) = fst;
        }
    }

    return result;
}

} // namespace population
} // namespace genesis

#endif // include guard
