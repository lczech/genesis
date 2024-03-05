#ifndef GENESIS_POPULATION_FUNCTIONS_SUBSAMPLE_H_
#define GENESIS_POPULATION_FUNCTIONS_SUBSAMPLE_H_

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/variant.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Subsampling and Subscaling
// =================================================================================================

/**
 * @brief Transform a BaseCounts @p sample by subscaling the nucleotide counts (`A`, `C`, `G`, `T`)
 * to sum up to @p max if @p max_coverage is exceeded for the sample.
 *
 * If the sum of nucleotide counts (that is, BaseCounts::a_count, BaseCounts::c_count,
 * BaseCounts::g_count, and BaseCounts::t_count) exceeds the given @p max_coverage, all counts
 * are scaled proportionally so that their sum is the given @p max_coverage.
 *
 * This transformation is used to limit the max coverage without filtering out the sample completely.
 * This is for instance useful when computing diversity estimators, which have a runtime and memory
 * cost that depends on the coverage. Hence, subscaling can reduce the overall runtime and memory
 * usage, without significantly altering the results.
 */
void transform_subscale_max_coverage(
    BaseCounts& sample,
    size_t max_coverage
);

/**
 * @copydoc transform_subscale_max_coverage( BaseCounts&, size_t, bool )
 *
 * This overload acts on all Variant::samples in the given @p variant.
 */
void transform_subscale_max_coverage(
    Variant& variant,
    size_t max_coverage
);

} // namespace population
} // namespace genesis

#endif // include guard
