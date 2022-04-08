#ifndef GENESIS_POPULATION_FUNCTIONS_STRUCTURE_H_
#define GENESIS_POPULATION_FUNCTIONS_STRUCTURE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/functions/functions.hpp"
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
//     Compute Helper
// =================================================================================================

// Can only be used with C++14, as we _need_ generic lambda expressions (with `auto` type-specifier
// in the parameter list). Otherwise, the type that is actually needed for the range iterators
// in the fst_functor() call in the function is literally impossible to capture. That is because
// the type contains a TransformIterator, which has a lambda as one of its template parameters,
// which hence cannot be named or captured in any form other than a lambda or an auto context...
// See https://stackoverflow.com/a/4846597/4184258 for details. See the version history of this
// file (which is currently named `genesis/population/functions/structure.hpp`) for the previous
// instance of this function that also worked under C++11, by simply copy-pasting the contents of
// compute_pairwise_f_st() to the functions where it is used...
// That was ungly, and this function is currently not needed for our downstream tools
// (e.g., grenedalf, see https://github.com/lczech/grenedalf), so we just leave it active here
// when C++14 is used for compiling.

#if __cplusplus >= 201402L

/**
 * @brief Helper function to compute a pairwise F_ST statistic, for all pairs of ranges of
 * BaseCounts%s.
 *
 * The function is intended to be used as an internal helper for computing pairwise F_ST
 * for a set of BaseCounts%s along some region (e.g., a genomic Window).
 *
 * The function expects a range (for example, a Window over the genome) of iterators,
 * where each iterator dereferences to a `std::vector<BaseCounts>`. Each entry in the range is
 * used as one position in the genome contributing to F_ST. For all entries, the `vector` needs
 * to have the same number of entries.
 *
 * For example, a Window over Variant%s (`Window<Variant>`) can be turned into such a range
 * as follows
 *
 *     // `window` is of type Window<Variant> here
 *     for( auto const& window : window_iterator ) {
 *
 *         // Return the BaseCounts part of the Variants in the window.
 *         auto base_counts_range = utils::make_transform_range([&]( Variant const& var )
 *         -> std::vector<BaseCounts> const& {
 *             return var.samples;
 *         }, window);
 *
 *         // Call an fst computation on that.
 *         f_st_pool_kofler( poolsizes, base_counts_range.begin(), base_counts_range.end() );
 *     }
 *
 * Then, `base_counts_range.begin()` and `base_counts_range.end()` can be provided as @p begin
 * and @p end here, respectively. Typically, as this is a helper function, they are provided
 * by the functions that use this helper; however, they need this range as input themselves,
 * and the above can be used for that.
 *
 * Furthermore, a functor @p fst_functor is expected that conducts the actual computation of
 * the desired variant of F_ST. This functor is filled in by the functiones that are actually
 * using this helper.
 *
 * Then, for each pair `(i,j)` of samples, the range is iterated, and the respective entries
 * `i` and `j` of the vectors in the range are used to compute F_ST using the functor @p fst_functor
 * for this pair of samples, and stored in the resulting matrix at positions `(i,j)` and `(j,i)`.
 */
template<class ForwardIterator, typename FstFunctor>
utils::Matrix<double> compute_pairwise_f_st(
    ForwardIterator begin, ForwardIterator end,
    FstFunctor fst_functor
) {
    // With no data, return empty result.
    if( begin == end ) {
        return {};
    }

    // Now we know that there are entries in the rage. Use the first one to get the number of
    // base pair samples in the range. We later check that this is the same for each entry.
    // Use that size to initialize the resulting matrix.
    size_t const size = static_cast<std::vector<BaseCounts> const&>( *begin ).size();
    auto result = utils::Matrix<double>( size, size, 0.0 );

    // We use a lambda that returns a tranforming rage to select an entry at a given index
    // in the set of BaseCounts at the current iterator position.
    auto select_entry = [size]( ForwardIterator begin, ForwardIterator end, size_t index ){
        // Currently, in order to use Window here, we need to explicitly use std::vector<BaseCounts>
        // instead of the more generic T... Bit unfortunate, but good enough for now.
        // Will have to revisit later if we get to use cases where the BaseCounts are not stored
        // in a vector, but some other container.
        // using T = typename ForwardIterator::value_type;
        return utils::make_transform_range(
            [size, index]( std::vector<BaseCounts> const& samples ) -> BaseCounts const& {
                if( samples.size() != size ) {
                    throw std::runtime_error(
                        "In compute_pairwise_f_st(): The number of BaseCounts in the "
                        "provided range is not consistent throughout the iteration."
                    );
                }
                return samples[index];
            },
            begin, end
        );
    };

    // Loop over all pairs of entries, and compute f_st for each of these pairs.
    // That is, in the inner code of the two loops, we run the f_st function that takes
    // two ranges, providing it with a pair of indices for which we compute the value.
    for( size_t i = 0; i < size; ++i ) {
        for( size_t j = i + 1; j < size; ++j ) {
            auto range_i = select_entry( begin, end, i );
            auto range_j = select_entry( begin, end, j );
            auto const fst = fst_functor(
                i, j,
                range_i.begin(), range_i.end(),
                range_j.begin(), range_j.end()
            );
            result( i, j ) = fst;
            result( j, i ) = fst;
        }
    }

    return result;
}

#endif // __cplusplus >= 201402L

// =================================================================================================
//     F_ST Pool Kofler
// =================================================================================================

/**
 * @brief Compute the SNP-based Theta Pi values used in f_st_pool_kofler().
 *
 * See there for details. The tuple returns Theta Pi for an individual position, which is simply
 * the heterozygosity() at this position, for both samples @p p1 and @p p2, as well as their
 * combined (average frequency) heterozygosity, in that order.
 */
std::tuple<double, double, double> f_st_pool_kofler_pi_snp(
    BaseCounts const& p1, BaseCounts const& p2
);

/**
 * @brief Compute the F_ST statistic for pool-sequenced data of Kofler et al
 * as used in PoPoolation2, for two ranges of BaseCounts%s.
 *
 * The approach is called the "classical" or "conventional" estimator in PoPoolation2 [1],
 * and follows Hartl and Clark [2].
 *
 * > [1] **PoPoolation2: identifying differentiation between populations
 * > using sequencing of pooled DNA samples (Pool-Seq).**<br />
 * > Kofler R, Pandey RV, Schlotterer C.<br />
 * > Bioinformatics, 2011, 27(24), 3435–3436. https://doi.org/10.1093/bioinformatics/btr589
 *
 * > [2] **Principles of Population Genetics.**<br />
 * > Hartl DL, Clark AG.<br />
 * > Sinauer, 2007.
 */
template<class ForwardIterator1, class ForwardIterator2>
double f_st_pool_kofler( // get_conventional_fstcalculator
    size_t p1_poolsize, size_t p2_poolsize,
    ForwardIterator1 p1_begin, ForwardIterator1 p1_end,
    ForwardIterator2 p2_begin, ForwardIterator2 p2_end
) {
    // Edge and error cases
    if( p1_poolsize <= 1 || p2_poolsize <= 1 ) {
        throw std::invalid_argument( "Cannot run f_st_pool_kofler() with poolsizes <= 1" );
    }

    // Theta Pi values for the two populations and their combination
    double p1_pi_sum = 0.0;
    double p2_pi_sum = 0.0;
    double pp_pi_sum = 0.0;

    // Iterate the two ranges in parallel. Each iteration is one position in the genome.
    // In each iteration, p1_it and p2_it point at BaseCounts objects containing nucleotide counts.
    auto p1_it = p1_begin;
    auto p2_it = p2_begin;
    while( p1_it != p1_end && p2_it != p2_end ) {

        // Compute frequency based pi snps. The tuple returns p1, p2, pp, in that order.
        auto const pi_snps = f_st_pool_kofler_pi_snp( *p1_it, *p2_it );

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

        // Next pair of entries.
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument(
            "In f_st_pool_kofler(): Provided ranges have different length."
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

#if __cplusplus >= 201402L

/**
 * @copydoc f_st_pool_kofler( size_t, size_t, ForwardIterator1, ForwardIterator1, ForwardIterator2, ForwardIterator2 )
 *
 * This version of the function computes F_ST for all pairs of a given input range of iterators.
 *
 * @see See compute_pairwise_f_st() for the expected input range specification.
 */
template<class ForwardIterator>
utils::Matrix<double> f_st_pool_kofler(
    std::vector<size_t> const& poolsizes,
    ForwardIterator begin, ForwardIterator end
) {
    return compute_pairwise_f_st(
        begin, end,
        [&]( size_t i, size_t j, auto p1_begin, auto p1_end, auto p2_begin, auto p2_end ){
            if( i >= poolsizes.size() || j >= poolsizes.size() ) {
                throw std::runtime_error(
                    "In f_st_pool_kofler(): Provided ranges have different lengths that "
                    "are not identical to the number of poolsizes provided."
                );
            }
            return f_st_pool_kofler(
                poolsizes[i], poolsizes[j],
                p1_begin, p1_end, p2_begin, p2_end
            );
        }
    );
}

#endif // __cplusplus >= 201402L

// =================================================================================================
//     F_ST Pool Karlsson
// =================================================================================================

/**
 * @brief Compute the numerator `N_k` and denominator `D_k`  needed for the asymptotically unbiased
 * F_ST estimator of Karlsson et al (2007).
 *
 * See f_st_pool_karlsson() for details. The function expects sorted base counts for the
 * two samples of which we want to compute F_ST, which are produced by sorted_average_base_counts().
 */
std::pair<double, double> f_st_pool_karlsson_nkdk(
    std::pair<SortedBaseCounts, SortedBaseCounts> const& sample_counts
);

/**
 * @brief Compute the F_ST statistic for pool-sequenced data of Karlsson et al
 * as used in PoPoolation2, for two ranges of BaseCounts%s.
 *
 * The approach is called the "asymptotically unbiased" estimator in PoPoolation2 [1],
 * and follows Karlsson et al [2].
 *
 * > [1] **PoPoolation2: identifying differentiation between populations
 * > using sequencing of pooled DNA samples (Pool-Seq).**<br />
 * > Kofler R, Pandey RV, Schlotterer C.<br />
 * > Bioinformatics, 2011, 27(24), 3435–3436. https://doi.org/10.1093/bioinformatics/btr589
 *
 * > [2] **Efficient mapping of mendelian traits in dogs through genome-wide association.**<br />
 * > Karlsson EK, Baranowska I, Wade CM, Salmon Hillbertz NHC, Zody MC, Anderson N, Biagi TM,
 * > Patterson N, Pielberg GR, Kulbokas EJ, Comstock KE, Keller ET, Mesirov JP, Von Euler H,
 * > Kämpe O, Hedhammar Å, Lander ES, Andersson G, Andersson L, Lindblad-Toh K.<br />
 * > Nature Genetics, 2007, 39(11), 1321–1328. https://doi.org/10.1038/ng.2007.10
 */
template<class ForwardIterator1, class ForwardIterator2>
double f_st_pool_karlsson( // get_asymptunbiased_fstcalculator
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
        auto const counts = sorted_average_base_counts( *p1_it, *p2_it );
        auto const nkdk = f_st_pool_karlsson_nkdk( counts );
        sum_nk += nkdk.first;
        sum_dk += nkdk.second;

        // Next pair of entries
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument(
            "In f_st_pool_karlsson(): Provided ranges have different length."
        );
    }

    return sum_nk / sum_dk;
}

#if __cplusplus >= 201402L

/**
 * @copydoc f_st_pool_karlsson( ForwardIterator1, ForwardIterator1, ForwardIterator2, ForwardIterator2 )
 *
 * This version of the function computes F_ST for all pairs of a given input range of iterators.
 *
 * @see See compute_pairwise_f_st() for the expected input range specification.
 */
template<class ForwardIterator>
utils::Matrix<double> f_st_pool_karlsson(
    ForwardIterator begin, ForwardIterator end
) {
    return compute_pairwise_f_st(
        begin, end,
        [&]( size_t i, size_t j, auto p1_begin, auto p1_end, auto p2_begin, auto p2_end ){
            (void) i;
            (void) j;
            return f_st_pool_karlsson(
                p1_begin, p1_end, p2_begin, p2_end
            );
        }
    );
}

#endif // __cplusplus >= 201402L

// =================================================================================================
//     F_ST Pool Spence
// =================================================================================================

/**
 * @brief Compute the SNP-based Theta Pi values used in f_st_pool_spence().
 *
 * The function returns pi within, between, and total, in that order.
 * See f_st_pool_spence() for details.
 */
std::tuple<double, double, double> f_st_pool_spence_pi_snp(
    size_t p1_poolsize, size_t p2_poolsize,
    BaseCounts const& p1, BaseCounts const& p2
);

/**
 * @brief Compute the unbiased F_ST statistic for pool-sequenced data of Spence et al,
 * for two ranges of BaseCounts%s.
 *
 * This is our novel approach for estimating F_ST, using pool-sequencing corrected estimates
 * of Pi within, Pi between, and Pi total, to compute F_ST following the definitions of
 * Nei [1] and Hudson [2], respectively. These are returned here as a pair in that order.
 *
 * > [1] **Analysis of Gene Diversity in Subdivided Populations.**<br />
 * > Nei M.<br />
 * > Proceedings of the National Academy of Sciences, 1973, 70(12), 3321–3323.
 * > https://doi.org/10.1073/PNAS.70.12.3321
 *
 * > [2] **Estimation of levels of gene flow from DNA sequence data.**<br />
 * > Hudson RR, Slatkin M, Maddison WP.<br />
 * > Genetics, 1992, 132(2), 583–589. https://doi.org/10.1093/GENETICS/132.2.583
 */
template<class ForwardIterator1, class ForwardIterator2>
std::pair<double, double> f_st_pool_spence(
    size_t p1_poolsize, size_t p2_poolsize,
    ForwardIterator1 p1_begin, ForwardIterator1 p1_end,
    ForwardIterator2 p2_begin, ForwardIterator2 p2_end
) {
    // Edge and error cases
    if( p1_poolsize <= 1 || p2_poolsize <= 1 ) {
        throw std::invalid_argument( "Cannot run f_st_pool_spence() with poolsizes <= 1" );
    }

    // Sums over the window of pi within, between, total.
    double pi_w_sum = 0.0;
    double pi_b_sum = 0.0;
    double pi_t_sum = 0.0;

    // Iterate the two ranges in parallel. Each iteration is one position in the genome.
    // In each iteration, p1_it and p2_it point at BaseCounts objects containing nucleotide counts.
    auto p1_it = p1_begin;
    auto p2_it = p2_begin;
    while( p1_it != p1_end && p2_it != p2_end ) {

        // Compute pi values for the snp.
        // The tuple `pi_snp` returns pi within, between, and total, in that order.
        auto const pi_snp = f_st_pool_spence_pi_snp( p1_poolsize, p2_poolsize, *p1_it, *p2_it );

        // Skip invalid entries than can happen when less than two of [ACGT] have
        // counts > 0 in one of the BaseCounts samples.
        if(
            std::isfinite( std::get<0>( pi_snp )) &&
            std::isfinite( std::get<1>( pi_snp )) &&
            std::isfinite( std::get<2>( pi_snp ))
        ) {
            // If we are here, both p1 and p2 have counts. Let's assert.
            assert( p1_it->a_count + p1_it->c_count + p1_it->g_count + p1_it->t_count > 0 );
            assert( p2_it->a_count + p2_it->c_count + p2_it->g_count + p2_it->t_count > 0 );

            // Now add them to the tally.
            pi_w_sum += std::get<0>( pi_snp );
            pi_b_sum += std::get<1>( pi_snp );
            pi_t_sum += std::get<2>( pi_snp );
        } else {
            // If we are here, at least one of the two inputs has one or fewer counts in [ACGT],
            // otherwise, the results would have been finite. Let's assert this.
            assert(
                ( p1_it->a_count + p1_it->c_count + p1_it->g_count + p1_it->t_count <= 1 ) ||
                ( p2_it->a_count + p2_it->c_count + p2_it->g_count + p2_it->t_count <= 1 )
            );
        }

        // Next pair of entries.
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument(
            "In f_st_pool_spence(): Provided ranges have different length."
        );
    }

    // Final computation of our two FST estimators, using Nei and Hudson, respectively.
    double const fst_nei = 1.0 - ( pi_w_sum / pi_t_sum );
    double const fst_hud = 1.0 - ( pi_w_sum / pi_b_sum );
    return { fst_nei, fst_hud };
}

#if __cplusplus >= 201402L

/**
 * @brief Compute an unbiased F_ST estimator for pool-sequenced data,
 * following Spence et al, for all pairs of ranges of BaseCounts%s.
 *
 * @copydetails f_st_pool_spence( std::vector<size_t> const&, ForwardIterator, ForwardIterator )
 *
 * @see See compute_pairwise_f_st() for the expected input range specification.
 */
template<class ForwardIterator>
utils::Matrix<double> f_st_pool_spence(
    std::vector<size_t> const& poolsizes,
    ForwardIterator begin, ForwardIterator end
) {
    return compute_pairwise_f_st(
        begin, end,
        [&]( size_t i, size_t j, auto p1_begin, auto p1_end, auto p2_begin, auto p2_end ){
            if( i >= poolsizes.size() || j >= poolsizes.size() ) {
                throw std::runtime_error(
                    "In f_st_pool_spence(): Provided ranges have different lengths that "
                    "are not identical to the number of poolsizes provided."
                );
            }
            return f_st_pool_spence(
                poolsizes[i], poolsizes[j],
                p1_begin, p1_end, p2_begin, p2_end
            );
        }
    );
}

#endif // __cplusplus >= 201402L

} // namespace population
} // namespace genesis

#endif // include guard
