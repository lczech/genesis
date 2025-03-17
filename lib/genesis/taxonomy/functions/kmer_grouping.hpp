#ifndef GENESIS_TAXONOMY_FUNCTIONS_KMER_GROUPING_H_
#define GENESIS_TAXONOMY_FUNCTIONS_KMER_GROUPING_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/functions/kmer_data.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/output_target.hpp"
#include "genesis/utils/math/bitvector.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Taxon Count Grouping
// =================================================================================================

// -------------------------------------------------------------------------
//     Settings
// -------------------------------------------------------------------------

/**
 * @brief Settings for group_by_taxon_sizes()
 *
 * This is mainly used to control the two limits we offer to construct the taxonomic groups:
 * By number of sequences, and their total length. It can of course happen that the accessions
 * in a single taxon exceed these limits - in those cases, the taxon becomes its own large group,
 * and we have to live with its size, no matter what it is, as we do not further sub-split taxon
 * groups in the current implementation.
 */
struct TaxonGroupingLimits
{
    /**
     * @brief Limit for the number of sequences (accessions) that are put into a group.
     */
    size_t max_group_num_sequences = 0;

    /**
     * @brief Limit for the total sum of sequence lengths of all accessions that are put into a group.
     */
    size_t max_group_sum_seq_lengths = 0;

    /**
     * @brief Merge sibling taxa.
     *
     * If set (by default), taxa that do not exceed the limits are merged with their siblings,
     * such that no limit is exceeded. This merging only applies to taxa that have the same parent.
     * If deactivated, small taxa will become their own group, which will lead to a large number of
     * groups.
     */
    bool merge_sibling_taxa = true;
};

/**
 * @brief Settings for group_with_target_number_of_groups()
 *
 * This is used for the binary search to approximate a given target number of groups of taxa.
 * Note that only one of the two `initial_` settings for one of the two limits of TaxonGroupingLimits
 * can be non-zero. This will then be the variable that is changed in between iterations of the
 * search to approximate the target number of groups.
 */
struct TaxonGroupingSearchParams
{
    /**
     * @brief Target number of groups of taxa to approximate.
     */
    size_t target_group_count = 0;

    /**
     * @brief Approximate the number of groups by changing the maximum number of sequences per group,
     * starting at this value.
     *
     * If set to non-zero, this is the starting value to evaluate the number of groups
     * resulting from group_by_taxon_sizes() when running the grouping with a limit on the number
     * of sequences per group.
     */
    size_t initial_group_num_sequences = 0;

    /**
     * @brief Approximate the number of groups by changing the maximum combined length of sequences
     * per group, starting at this value.
     *
     * If set to non-zero, this is the starting value to evaluate the number of groups
     * resulting from group_by_taxon_sizes() when running the grouping with a limit on the combined
     * length of sequences per group.
     */
    size_t initial_group_sum_seq_lengths = 0;

    /**
     * @brief See TaxonGroupingLimits. Repeated here for ease of usage.
     */
    bool merge_sibling_taxa = true;
};

// -------------------------------------------------------------------------
//     Functions
// -------------------------------------------------------------------------

/**
 * @brief Construct groups of taxa based on the counts of sequences and their lengths.
 *
 * This uses the taxonomy to guide the grouping, and builds them following the constraints
 * of the provided settings (maximum number of sequences per group, and/or maximum combined length
 * of all sequences in a group). The result is an assignemnt of each grouped taxon to a group,
 * where however taxa on the higher ranks might be unassigned, if they are too big (by either of the
 * provided maximum limits) to form a group of their own. In other words, higher ranks build the
 * "trunk" of the groups, and leaves of that trunk form groups that (as best as possible) stay
 * within the given size limits.
 *
 * We also try to combine sibling taxa into groups if their combined sizes are within the limits.
 * This is done in order to reduce the number of groups as far as possible (within the given limits).
 * Note that it can happen that a single taxon exceeds the given limits. In that case, it will form
 * a group of its own, with no siblings combined.
 *
 * The prerequisite of the function is that the Taxonomy has data type KmerTaxonData, and that
 * the KmerTaxonData::num_sequences and KmerTaxonData::sum_seq_lengths values are set.
 */
void group_by_taxon_sizes( TaxonGroupingLimits const& settings, Taxonomy& tax );

/**
 * @brief Construct groups of taxa based on the counts of sequences or their lengths,
 * such that a given number of groups is approximately reached.
 *
 * This runs a binary search on group_by_taxon_sizes(), trying to get the resulting number of groups
 * as close as possible to the target size. This might not be completely feasible, and the algorithm
 * will stop if no changes in the limits get closer to the target size.
 *
 * This can only run by either considering the number of sequences *or* their total length as the
 * variable that is changed in between iterations of the search. In the first phase of the algorithm,
 * that variable is modified to find the boundaries of the search that include the target number of
 * groups. In the second phase, the algorithm then runs a binary search within those boundaries to
 * find the best fitting values.
 *
 * The function returns the value used for the limit that led to the final grouping. That value
 * is either a max number of sequences per group, or max combined length of sequences per group,
 * depending on which initial value was set in TaxonGroupingSearchParams.
 */
size_t group_with_target_number_of_groups( TaxonGroupingSearchParams const& params, Taxonomy& tax );

} // namespace taxonomy
} // namespace genesis

#endif // include guard
