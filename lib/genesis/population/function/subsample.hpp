#ifndef GENESIS_POPULATION_FUNCTION_SUBSAMPLE_H_
#define GENESIS_POPULATION_FUNCTION_SUBSAMPLE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Subscaling and rescaling
// =================================================================================================

/**
 * @brief Transform a SampleCounts @p sample by sub-scaling the base counts (`A`, `C`, `G`, `T`, as
 * well as `N` and `D`) to sum up to @p max_coverage if @p max_coverage is exceeded for the sample.
 *
 * If the sum of counts (that is, SampleCounts::a_count, SampleCounts::c_count, SampleCounts::g_count,
 * SampleCounts::t_count, SampleCounts::n_count, and SampleCounts::d_count) exceeds the given
 * @p max_coverage, all counts are scaled proportionally so that their sum is @p max_coverage.
 * If the sum is below @p max_coverage, nothing happens.
 *
 * This transformation is used to limit the max coverage without filtering out the sample completely.
 * This is for instance useful when computing diversity estimators, which have a runtime and memory
 * cost that depends on the coverage. Hence, sub-scaling can reduce the overall runtime and memory
 * usage, without significantly altering the results.
 *
 * @see rescale_counts()
 * @see subsample_counts_with_replacement()
 * @see subsample_counts_without_replacement()
 */
void subscale_counts(
    SampleCounts& sample,
    size_t max_coverage
);

/**
 * @copydoc subscale_counts( SampleCounts&, size_t )
 *
 * This overload acts on all Variant::samples in the given @p variant.
 */
void subscale_counts(
    Variant& variant,
    size_t max_coverage
);

/**
 * @brief Transform a SampleCounts @p sample by re-scaling the base counts (`A`, `C`, `G`, `T`, as
 * well as `N` and `D`) to sum up to @p max if @p max_coverage is exceeded for the sample.
 *
 * This is identical to subscale_counts(), but performs the transformation regardless of whether
 * the sum of counts exceeds the specified coverage. In other words, this simply performs a
 * linear re-scaling of the counts so that they sum to the given @p target_coverage.
 *
 * @see subscale_counts()
 */
void rescale_counts(
    SampleCounts& sample,
    size_t target_coverage
);

/**
 * @copydoc rescale_counts( SampleCounts&, size_t )
 *
 * This overload acts on all Variant::samples in the given @p variant.
 */
void rescale_counts(
    Variant& variant,
    size_t target_coverage
);

// =================================================================================================
//     Subsampling and resampling with replacement
// =================================================================================================

/**
 * @brief Transform a SampleCounts @p sample by subsampling the nucleotide counts (`A`, `C`, `G`, `T`,
 * as well as `N` and `D`) _with_ replacement to sum up to @p max if @p max_coverage is exceeded
 * for the sample.
 *
 * If the sum of nucleotide counts (that is, SampleCounts::a_count, SampleCounts::c_count,
 * SampleCounts::g_count, SampleCounts::t_count, SampleCounts::n_count, and SampleCounts::d_count) exceeds
 * the given @p max_coverage, the counts are resampled _with_ replacement so that their sum is the
 * given @p max_coverage. This uses
 * @link ::genesis::utils::multinomial_distribution() multinomial_distribution()@endlink
 * for the sampling. If the count sum is below, nothing is done.
 *
 * @see resample_counts()
 * @see subscale_counts()
 * @see subsample_counts_without_replacement()
 */
void subsample_counts_with_replacement(
    SampleCounts& sample,
    size_t max_coverage
);

/**
 * @copydoc subsample_counts_with_replacement( SampleCounts&, size_t )
 *
 * This overload acts on all Variant::samples in the given @p variant.
 */
void subsample_counts_with_replacement(
    Variant& variant,
    size_t max_coverage
);

/**
 * @brief Resample all counts in a SampleCounts @p sample to a new @p target_coverage.
 *
 * This samples _with_ replacement from a multinomial_distribution distrubtion based on the
 * previous counts of the @p sample. This is the same as subsample_counts_with_replacement(),
 * but performs the resampling regardless of whether the sum of counts exceeds the specified
 * coverage.
 *
 * The function can be seen as a way of creating in-silico replicates of a given population sample.
 * There is no equivalent _without_ replacement, as those could not sample more counts than there
 * are in the original population anyway - meaning that subsample_counts_without_replacement()
 * already coveres that distribution.
 */
void resample_counts(
    SampleCounts& sample,
    size_t target_coverage
);

/**
 * @copydoc resample_counts( SampleCounts&, size_t )
 *
 * This overload acts on all Variant::samples in the given @p variant.
 */
void resample_counts(
    Variant& variant,
    size_t target_coverage
);

// =================================================================================================
//     Subsampling without replacement
// =================================================================================================

/**
 * @brief Transform a SampleCounts @p sample by subsampling the nucleotide counts (`A`, `C`, `G`, `T`,
 * as well as `N` and `D`) _without_ replacement to sum up to @p max if @p max_coverage is exceeded
 * for the sample.
 *
 * If the sum of nucleotide counts (that is, SampleCounts::a_count, SampleCounts::c_count,
 * SampleCounts::g_count, SampleCounts::t_count, SampleCounts::n_count, and SampleCounts::d_count) exceeds
 * the given @p max_coverage, the counts are resampled _without_ replacement so that their sum is
 * the given @p max_coverage. This uses
 * @link ::genesis::utils::multivariate_hypergeometric_distribution() multivariate_hypergeometric_distribution()@endlink
 * for the sampling. If the count sum is below, nothing is done.
 *
 * @see subscale_counts()
 * @see subsample_counts_with_replacement()
 */
void subsample_counts_without_replacement(
    SampleCounts& sample,
    size_t max_coverage
);

/**
 * @copydoc subsample_counts_without_replacement( SampleCounts&, size_t )
 *
 * This overload acts on all Variant::samples in the given @p variant.
 */
void subsample_counts_without_replacement(
    Variant& variant,
    size_t max_coverage
);

} // namespace population
} // namespace genesis

#endif // include guard
