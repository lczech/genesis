#ifndef GENESIS_TAXONOMY_FUNCTIONS_KMER_H_
#define GENESIS_TAXONOMY_FUNCTIONS_KMER_H_

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
 * @brief Given a @p taxonomy with data entries of type KmerTaxonData, propagate and accumulate
 * count values towards the root.
 *
 * This function takes the KmerTaxonData::num_sequences and KmerTaxonData::sum_seq_lengths of each
 * taxon, and accumulates them towards the root, storing the data in KmerTaxonData::clade_num_sequences
 * and KmerTaxonData::clade_sum_seq_lengths respectively. In other words, after this functions,
 * each clade contains the number of sequences and their total length (in number of bases)
 * of itself and all its children, stored in the two `total` values of KmerTaxonData.
 *
 * The given @p taxonomy is assumed to have data type KmerTaxonData, and throws an exception
 * otherwise. See reset_taxonomy_data() to set this data type for all Taxa in a Taxonomy.
 */
void accumulate_taxon_sizes( Taxonomy& tax );

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

/**
 * @brief Count the number of groups that were constructed by group_by_taxon_sizes().
 */
size_t count_taxon_groups( Taxonomy const& tax );

/**
 * @brief For each Taxon group constructed with group_by_taxon_sizes(), get a pointer to the
 * highest-ranking taxa within that group.
 *
 * This function operates on a Taxonomy of KmerTaxonData that has been processed with
 * group_by_taxon_sizes() to group taxa based on counts of sequences and/or their length.
 * This results in taxon groups, as well as inner (expanded) taxa that are not part of a group.
 *
 * This function returns for each group a pointer to the list of highest rank taxa of the Taxonomy
 * that are assigned to that group. This can be multiple, in case that
 * TaxonGroupingLimits::merge_sibling_taxa was used when grouping the taxa; all of them will be
 * sibling though, i.e., they are all children of the same parent taxon.
 *
 * The resulting vector is indexed by the KmerTaxonData::group_index of each group.
 * That is, for one group (at the indices of the vector), all highest-ranking (sibling) taxa
 * that belong to that group form the entries of the sub-vector at that index.
 */
std::vector<std::vector<Taxon const*>> taxononmy_group_taxa_list( Taxonomy const& tax );

// =================================================================================================
//     User Reporting
// =================================================================================================

/**
 * @brief Take a Taxonomy with group annotations, and produce a new Taxonomy consisting of
 * just the trunk Taxa.
 *
 * This takes the result of group_by_taxon_sizes(), and turns all group taxa into pseudo-taxa named
 * after the group numbers (`group_x`). Mostly meant for visual inspection of the result of the
 * grouping, in form of a taxonomy, which can also be turned into a tree via taxonomy_to_tree()
 * for visualization purposes.
 */
Taxonomy grouped_taxonomy_trunk( Taxonomy const& tax );

/**
 * @brief Take a pseudo-taxonomy with group taxa, and produce a report of the result.
 *
 * This takes the result of grouped_taxonomy_trunk(), and produces a text report for user output,
 * summarizing some stats of the grouping.
 */
std::string grouped_taxonomy_report( Taxonomy const& tax );

// =================================================================================================
//     Json Format
// =================================================================================================

/**
 * @brief Data of a single group, as a POD.
 *
 * The functions for grouping of a Taxonomy, group_by_taxon_sizes() and
 * group_with_target_number_of_groups(), produce annotations of the Taxonomy in form of KmerTaxonData
 * for each Taxon. We can store the data in a json file with write_taxonomy_grouping_to_json(),
 * which is meant for inspection of the results of grouping.
 *
 * When reading back this json data, we however might not always want to reconstruct all data on
 * the Taxonomy where it came from; instead, for some use cases, we might just be interested in the
 * data per group, without the underlying Taxonomy. To this end, read_taxonomy_grouping_from_json()
 * reads such a file again, and produces a vector of elements containing the group data.
 *
 * This data structure captures the data there. See write_taxonomy_grouping_to_json() for details.
 */
struct TaxonomyGroupData
{
    size_t group_index;
    size_t num_sequences;
    size_t sum_seq_lengths;
    std::vector<std::string> taxa;
};

/**
 * @brief Write the resulting list of groups of taxonomic grouping to json.
 *
 * This function is meant to write the result of group_by_taxon_sizes() in a human-readable form
 * for downstream processing and analysis. It simply produces a list of the groups produced,
 * where each group has an index, and a list of all (top-most) taxonomic paths that it contains.
 */
void write_taxonomy_grouping_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target
);

/**
 * @brief Read data back in that was written by write_taxonomy_grouping_to_json().
 *
 * See TaxonomyGroupData for a description of the resuling POD struct per taxonomic group.
 */
std::vector<TaxonomyGroupData> read_taxonomy_grouping_from_json(
    std::shared_ptr<utils::BaseInputSource> source
);

/**
 * @brief Write a Taxonomy with KmerTaxonData, in our internal Json format.
 *
 * This is meant to store some data for later, and for visual inspection of the results.
 * By default, we just write the basic counts per taxon. However, @p with_group_data also
 * writes the result of group_by_taxon_sizes() to the file. If then additionally @p only_trunk
 * is set, we only write the "inner" taxa, i.e., the ones that are expanded and are thus not part
 * of a group, and the first (top-most) taxa that are grouped. All taxa below the group are then
 * omitted. This means, the resulting json will only represent the scaffold/trunk of the taxonomy
 * of groups, which is most relevant for inspection, while leaving out the details of all clades
 * that are grouped.
 */
void write_kmer_taxonomy_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target,
    bool with_group_data = false,
    bool only_trunk = false
);

/**
 * @brief Read a Taxonomy with KmerTaxonData, from our internal Json format.
 *
 * This reads back the data produced by write_kmer_taxonomy_to_json(). See there for details.
 * Note that if write_kmer_taxonomy_to_json() was called with `only_trunk`, only the
 * scaffold/trunk of the taxonomy will be read as well.
 */
Taxonomy read_kmer_taxonomy_from_json(
    std::shared_ptr<utils::BaseInputSource> source
);

} // namespace taxonomy
} // namespace genesis

#endif // include guard
