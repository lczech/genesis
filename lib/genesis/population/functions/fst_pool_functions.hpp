#ifndef GENESIS_POPULATION_FUNCTIONS_FST_POOL_FUNCTIONS_H_
#define GENESIS_POPULATION_FUNCTIONS_FST_POOL_FUNCTIONS_H_

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
 * @ingroup population
 */

#include "genesis/population/functions/fst_pool_karlsson.hpp"
#include "genesis/population/functions/fst_pool_kofler.hpp"
#include "genesis/population/functions/fst_pool_unbiased.hpp"
#include "genesis/population/functions/fst_pool_processor.hpp"
#include "genesis/population/functions/functions.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/transform_iterator.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
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
 * @copydoc FstPoolCalculatorKofler
 */
template<class ForwardIterator1, class ForwardIterator2>
double f_st_pool_kofler( // get_conventional_fstcalculator
    size_t p1_poolsize, size_t p2_poolsize,
    ForwardIterator1 p1_begin, ForwardIterator1 p1_end,
    ForwardIterator2 p2_begin, ForwardIterator2 p2_end
) {
    // Edge and error cases
    if( p1_poolsize <= 1 || p2_poolsize <= 1 ) {
        return std::numeric_limits<double>::quiet_NaN();
        // throw std::invalid_argument( "Cannot run f_st_pool_kofler() with poolsizes <= 1" );
    }

    // Init the calculator.
    FstPoolCalculatorKofler calc{ p1_poolsize, p2_poolsize };

    // Iterate the two ranges in parallel. Each iteration is one position in the genome.
    // In each iteration, p1_it and p2_it point at BaseCounts objects containing nucleotide counts.
    auto p1_it = p1_begin;
    auto p2_it = p2_begin;
    while( p1_it != p1_end && p2_it != p2_end ) {
        calc.process( *p1_it, *p2_it );
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument(
            "In f_st_pool_kofler(): Provided ranges have different length."
        );
    }

    // Compute the final result.
    return calc.get_result();
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
 * @copydoc FstPoolCalculatorKarlsson
 */
template<class ForwardIterator1, class ForwardIterator2>
double f_st_pool_karlsson( // get_asymptunbiased_fstcalculator
    ForwardIterator1 p1_begin, ForwardIterator1 p1_end,
    ForwardIterator2 p2_begin, ForwardIterator2 p2_end
) {
    using namespace genesis::utils;

    // Init the calculator.
    FstPoolCalculatorKarlsson calc{};

    // Iterate both ranges, summing up N_k and D_k for all their entries.
    auto p1_it = p1_begin;
    auto p2_it = p2_begin;
    while( p1_it != p1_end && p2_it != p2_end ) {
        calc.process( *p1_it, *p2_it );
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument(
            "In f_st_pool_karlsson(): Provided ranges have different length."
        );
    }

    return calc.get_result();
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
//     F_ST Pool Unbiased (Spence)
// =================================================================================================

/**
 * @copydoc FstPoolCalculatorUnbiased
 */
template<class ForwardIterator1, class ForwardIterator2>
std::pair<double, double> f_st_pool_unbiased(
    size_t p1_poolsize, size_t p2_poolsize,
    ForwardIterator1 p1_begin, ForwardIterator1 p1_end,
    ForwardIterator2 p2_begin, ForwardIterator2 p2_end
) {
    // Edge and error cases
    if( p1_poolsize <= 1 || p2_poolsize <= 1 ) {
        return {
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN()
        };
        // throw std::invalid_argument( "Cannot run f_st_pool_unbiased() with poolsizes <= 1" );
    }

    // Init the calculator.
    FstPoolCalculatorUnbiased calc{ p1_poolsize, p2_poolsize };

    // Iterate the two ranges in parallel. Each iteration is one position in the genome.
    // In each iteration, p1_it and p2_it point at BaseCounts objects containing nucleotide counts.
    auto p1_it = p1_begin;
    auto p2_it = p2_begin;
    while( p1_it != p1_end && p2_it != p2_end ) {
        calc.process( *p1_it, *p2_it );
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument(
            "In f_st_pool_unbiased(): Provided ranges have different length."
        );
    }

    return calc.get_result_pair();
}

#if __cplusplus >= 201402L

/**
 * @brief Compute an unbiased F_ST estimator for pool-sequenced data,
 * using the Nei variant of the estimator, for all pairs of ranges of BaseCounts%s.
 *
 * See f_st_pool_unbiased() for details on the method.
 * We here need to offer two variants of the pairwise compute helper, as there are two estimator
 * variants. See f_st_pool_unbiased_hudson() for the other variant.
 *
 * @see See compute_pairwise_f_st() for the expected input range specification.
 */
template<class ForwardIterator>
utils::Matrix<double> f_st_pool_unbiased_nei(
    std::vector<size_t> const& poolsizes,
    ForwardIterator begin, ForwardIterator end
) {
    return compute_pairwise_f_st(
        begin, end,
        [&]( size_t i, size_t j, auto p1_begin, auto p1_end, auto p2_begin, auto p2_end ){
            if( i >= poolsizes.size() || j >= poolsizes.size() ) {
                throw std::runtime_error(
                    "In f_st_pool_unbiased_nei(): Provided ranges have different lengths that "
                    "are not identical to the number of poolsizes provided."
                );
            }
            return f_st_pool_unbiased(
                poolsizes[i], poolsizes[j],
                p1_begin, p1_end, p2_begin, p2_end
            ).first;
        }
    );
}

/**
 * @brief Compute an unbiased F_ST estimator for pool-sequenced data,
 * using the Hudson variant of the estimator, for all pairs of ranges of BaseCounts%s.
 *
 * See f_st_pool_unbiased() for details on the method.
 * We here need to offer two variants of the pairwise compute helper, as there are two estimator
 * variants. See f_st_pool_unbiased_nei() for the other variant.
 *
 * @see See compute_pairwise_f_st() for the expected input range specification.
 */
template<class ForwardIterator>
utils::Matrix<double> f_st_pool_unbiased_hudson(
    std::vector<size_t> const& poolsizes,
    ForwardIterator begin, ForwardIterator end
) {
    return compute_pairwise_f_st(
        begin, end,
        [&]( size_t i, size_t j, auto p1_begin, auto p1_end, auto p2_begin, auto p2_end ){
            if( i >= poolsizes.size() || j >= poolsizes.size() ) {
                throw std::runtime_error(
                    "In f_st_pool_unbiased_hudson(): Provided ranges have different lengths that "
                    "are not identical to the number of poolsizes provided."
                );
            }
            return f_st_pool_unbiased(
                poolsizes[i], poolsizes[j],
                p1_begin, p1_end, p2_begin, p2_end
            ).second;
        }
    );
}

#endif // __cplusplus >= 201402L

} // namespace population
} // namespace genesis

#endif // include guard
