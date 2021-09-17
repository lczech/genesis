#ifndef GENESIS_POPULATION_FUNCTIONS_FILTER_TRANSFORM_H_
#define GENESIS_POPULATION_FUNCTIONS_FILTER_TRANSFORM_H_

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/variant.hpp"

#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     General Filtering
// =================================================================================================

/**
 * @brief Select how to filter Variant%s based on whether a predicate is true for any or for all
 * of the BaseCounts samples in the Variant.
 */
enum class VariantFilterType
{
    /**
     * @brief The filter returns `true` if _any_ of the BaseCounts samples in the Variant return
     * `true` for a given predicate.
     */
    kAny,

    /**
     * @brief The filter returns `true` if _all_ of the BaseCounts samples in the Variant return
     * `true` for a given predicate.
     */
    kAll
};

/**
 * @brief
 *
 * See status() for details.
 */
bool filter_is_snp(
    Variant const& variant,
    VariantFilterType type,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
);

// =================================================================================================
//     Filtering by count
// =================================================================================================

// bool filter_by_min_maf_count( BaseCounts const& sample, size_t min_count );
//
// bool filter_by_min_maf_count( Variant const& variant, size_t min_count, VariantFilterType type );
//
// bool filter_by_max_count( BaseCounts const& sample, size_t max_count );
//
// bool filter_by_max_count( Variant const& variant, size_t max_count, VariantFilterType type );

// =================================================================================================
//     Transforming by count
// =================================================================================================

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p min_count is not reached for that nucleotide.
 *
 * This transformation is used as a type of quality control. All nucleotide counts (that is,
 * BaseCounts::a_count, BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count) that are
 * below the given @p min_count are set to zero.
 */
void transform_by_min_count( BaseCounts& sample, size_t min_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * of its samples to zero if @p min_count is not reached for that nucleotide.
 */
void transform_by_min_count( Variant& variant, size_t min_count );

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p max_count is exceeded for that nucleotide.
 *
 * This transformation is used as a type of quality control. All nucleotide counts (that is,
 * BaseCounts::a_count, BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count) that are
 * above the given @p max_count are set to zero.
 */
void transform_by_max_count( BaseCounts& sample, size_t max_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * of its samples to zero if @p max_count is exceeded for that nucleotide.
 */
void transform_by_max_count( Variant& variant, size_t max_count );

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p min_count is not reached or if @p max_count is exceeded for that nucleotide.
 *
 * This is the same as running transform_by_min_count() and transform_by_max_count() individually.
 */
void transform_by_min_max_count( BaseCounts& sample, size_t min_count, size_t max_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`) of its samples
 * to zero if @p min_count is not reached or if @p max_count is exceeded for that nucleotide.
 */
void transform_by_min_max_count( Variant& variant, size_t min_count, size_t max_count );

} // namespace population
} // namespace genesis

#endif // include guard
