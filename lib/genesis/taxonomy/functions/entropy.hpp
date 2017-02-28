#ifndef GENESIS_TAXONOMY_FUNCTIONS_ENTROPY_H_
#define GENESIS_TAXONOMY_FUNCTIONS_ENTROPY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/functions/entropy_data.hpp"

#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Prune Settings
// =================================================================================================

/**
 * @brief Store settings for the Taxonomy pruning algorithm prune_by_entropy().
 */
struct PruneByEntropySettings
{
    /**
     * @brief Minimal size of a sub-taxonomy of the pruned Taxonomy. Default is 0.
     *
     * If the parameter `min_subtaxonomy_size` is set to a value > 0, the algorithm
     * prevents sub-taxonomies from becoming smaller than this threshold. Instead of pruning at
     * such a small sub-taxonomy, it is fully expanded. This avoids ending up with overly many small
     * sub-taxonomies with just a few leaf taxa inside them.
     */
    size_t min_subtaxonomy_size = 0;

    /**
     * @brief Maximal size of a sub-taxonomy of the pruned Taxonomy. Default is 0.
     *
     * If the parameter `max_subtaxonomy_size` is set to a value > 0, an additional
     * preprocessing step is executed, which ensures that in the resulting pruned Taxonomy, no Taxon
     * has more than this size many leaf Taxa. This is useful if the pruned Taxonomy is used for
     * some multilevel algorithms, which first take the pruned "overview" Taxonomy, and then might
     * recurse into the smaller "sub" Taxonomies. In such cases, those sub taxonomies might need to
     * not exceed a certain size, thus this option.
     */
    size_t max_subtaxonomy_size = 0;

    /**
     * @brief Minimum level of the Taxa that are considered inside for pruning. Default is 0.
     *
     * If this paramter is set to a value > 0, all @link Taxon Taxa@endlink with a level lower than
     * the given value are automatically considered to be inside the pruned taxonomy. In other words,
     * any Taxon up to (and including) the given level in the Taxonomy will be included after
     * pruning. Only Taxa with higher levels can be pruned then, i.e., receive pruning status
     * `outside`. Taxa with exaclty the level can either be `inside` or `border`.
     *
     * This option is useful to ensure that the low levels (high ranks) of the Taxonomy are
     * always included after pruning. Otherwise, it might happen that the pruning algorithm prunes
     * away whole sub-taxonomies that somehow have a low total entropy, but still should not
     * be simply pruned away.
     */
    size_t min_border_level = 0;

    /**
     * @brief Allow some approximation in order to get closer to the target pruning size.
     *
     * If the parameter `allow_approximation` (default is `false`) is set to `true`,
     * we also allow to split up a border Taxon that has not the currently highest entropy of all
     * border Taxa, as long as this brings us closer to the target size.
     */
    bool   allow_approximation  = false;
};

// =================================================================================================
//     Prune Functions
// =================================================================================================

/**
 * @brief Prune a Taxonomy so that the result (approximately) contains a desired number of "leaf"
 * @link Taxon Taxa@endlink, using the entropy of the Taxa as indicator where to prune.
 *
 * The function takes a Taxonomy with data type EntropyTaxonData and a target size which indicates
 * the desired number of "leaf" Taxa after pruning the Taxonomy. In the pruned Taxonomy, some Taxa
 * are considered as belonging to the Taxonomy (have status EntropyTaxonData::PruneStatus::kInside
 * or EntropyTaxonData::PruneStatus::kBorder), while others (deeper in the Taxonomy) are excluded
 * (have status EntropyTaxonData::PruneStatus::kOutside). The number of border taxa (or "leaves")
 * of the included Taxa then is aimed to be as close as possible to the target size.
 *
 * That means, this function sets the @link EntropyTaxonData::status status@endlink of the
 * @link Taxon Taxa@endlink, but does not remove any Taxa. All Taxa with status
 * EntropyTaxonData::PruneStatus::kOutside are then considered to be pruned from the taxonomy.
 *
 * Example: The Taxonomy
 *
 *     Tax_1
 *         Tax_2
 *             Tax_3
 *             Tax_4
 *         Tax_5
 *             Tax_6
 *     Tax_7
 *         Tax_8
 *         Tax_9
 *
 * contains 5 "leaf" taxa, i.e., `Tax_3`, `Tax_4`, `Tax_6`, `Tax_8` and `Tax_9`. If we want to
 * prune it with a target size of 3, we might end up with either
 *
 *     Tax_1
 *         Tax_2
 *         Tax_5
 *     Tax_7
 *
 * or
 *
 *     Tax_1
 *     Tax_7
 *         Tax_8
 *         Tax_9
 *
 * as both contain 3 "leaves": `Tax_2`, `Tax_5` and `Tax_7` in the former case and `Tax_1`, `Tax_8`
 * and `Tax_9` in the latter. Which of those two is used depends on the entropies of the Taxa.
 *
 * In the former case, `Tax_1` is considered inside, `Tax_2`, `Tax_5` and `Tax_7` are border, and
 * all other taxa are outside of the pruned Taxonomy. In the latter case, `Tax_7` is inside,
 * `Tax_1`, `Tax_8` and `Tax_9` are border, and again all others are outside.
 *
 * It is not always possible to prune a Taxonomy in a way the we exaclty hit the target size. The
 * function then ends at a number of border Taxa that is closest (either below or above the target
 * size).
 *
 * In order to decide which Taxa to set to inside (i.e., not include as leaves, but further resolve
 * into their children), we use the entropies of the Taxa: We choose to split up at a current border
 * Taxon with the highest entropy value, as long as this brings us closer to the target size.
 *
 * This means that the above case where we had two possible ways of splitting should be rare, as
 * the entropies will rarely be identical with real world data sets. If this happens nonetheless,
 * it is random which of the Taxa with equal entropy will be used.
 *
 * In order to control further settings, see PruneByEntropySettings.
 */
void prune_by_entropy(
    Taxonomy&              taxonomy,
    size_t                 target_taxonomy_size,
    PruneByEntropySettings settings = {}
);

/**
 * @brief Expand the leaves of a pruned Taxonomy if their sub-taxonomies are smaller than the
 * given threshold.
 *
 * This function takes a Taxonomy with EntropyTaxonData on its @link Taxon Taxa@endlink and
 * looks for taxa with status @link EntropyTaxonData::PruneStatus::kBorder kBorder@endlink
 * which have fewer than the threshold many leaves. If so, this sub-taxonomy is expaneded.
 * This is, it is turned into taxa with status
 * @link EntropyTaxonData::PruneStatus::kInside kInside@endlink for inner taxa and
 * @link EntropyTaxonData::PruneStatus::kBorder kBorder@endlink for leaf taxa.
 */
void expand_small_subtaxonomies(
    Taxonomy& taxonomy,
    size_t    min_subtaxonomy_size
);

/**
 * @brief Return the number of @link Taxon Taxa@endlink that have a certain
 * @link EntropyTaxonData::PruneStatus prune status@endlink.
 */
size_t count_taxa_with_prune_status(
    Taxonomy const&               taxonomy,
    EntropyTaxonData::PruneStatus status
);

/**
 * @brief Remove the children of all @link Taxon Taxa@endlink that are pruned, i.e, that have
 * @link EntropyTaxonData::PruneStatus prune status@endlink `==`
 * @link EntropyTaxonData::PruneStatus::kOutside kOutside@endlink.
 *
 * The function does not validate the status before. Use validate_pruned_taxonomy() if you are
 * unsure whether the status is correct for all Taxa.
 */
void remove_pruned_taxonomy_children( Taxonomy& taxonomy );

/**
 * @brief Print a Taxonomy, highlighting those @link Taxon Taxa@endlink that are the pruning border,
 * i.e., where we cut off the sub-taxa, and print their entropies next to them.
 */
std::string print_pruned_taxonomy(    Taxonomy const& taxonomy );

/**
 * @brief Validate that the pruning status of a Taxonomy is valid.
 *
 * This function expects the @link Taxon Taxa@endlink of the Taxonomy to have data type
 * EntropyTaxonData. It then checks whether the pruning states are all correctly set.
 *
 * That means:
 *
 *   * Taxa with status @link EntropyTaxonData::PruneStatus::kInside kInside@endlink can only have
 *     children of the same status or of @link EntropyTaxonData::PruneStatus::kBorder kBorder@endlink.
 *   * Taxa with status @link EntropyTaxonData::PruneStatus::kBorder kBorder@endlink can only have
 *     children of status @link EntropyTaxonData::PruneStatus::kOutside kOutside@endlink.
 *   * Taxa with status @link EntropyTaxonData::PruneStatus::kOutside kOutside@endlink can only have
 *     children of the same status.
 *
 * If any of those conditions is not met, an information about the faulty Taxon is written to
 * LOG_INFO, and the function returns `false`.
 */
bool        validate_pruned_taxonomy( Taxonomy const& taxonomy );

} // namespace taxonomy
} // namespace genesis

#endif // include guard
