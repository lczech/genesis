#ifndef GENESIS_PLACEMENT_FUNCTION_HELPER_H_
#define GENESIS_PLACEMENT_FUNCTION_HELPER_H_

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
 * @ingroup placement
 */

#include "genesis/placement/sample.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/utils/math/matrix.hpp"

#include <unordered_map>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

struct PqueryPlain;

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Return a mapping of `edge_num` integers to the corresponding PlacementTreeEdge object.
 *
 * In a valid `jplace` file, the `edge_nums` are in increasing order with a postorder traversal of
 * the tree. However, as Genesis does not need this constraint, we return a map here instead.
 */
std::unordered_map<int, PlacementTreeEdge*> edge_num_to_edge_map( PlacementTree const& tree );

/**
 * @brief Return a mapping of edge_num integers to the corresponding PlacementTreeEdge object.
 *
 * This function depends on the tree only and does not involve any pqueries. Thus, it forwards to
 * edge_num_to_edge_map( PlacementTree const& ). See there for details.
 */
std::unordered_map<int, PlacementTreeEdge*> edge_num_to_edge_map( Sample const& smp );

/**
 * @brief Return a mapping from each PlacementTreeEdge%s to the PqueryPlacement%s that
 * are placed on that edge.
 *
 * The result `vector` is indexed using PlacementTreeEdge::index(). For each entry, it contains
 * another `vector` that holds pointers to the PqueryPlacement%s of the Sample.
 *
 * If the optional parameter `only_max_lwr_placements` is set to `false` (default), each placement
 * in the Sample is added, not just the most likely ones. If set to `true`, only the PqueryPlacement
 * with the highest @link PqueryPlacement::like_weight_ratio like_weight_ratio@endlink is added.
 *
 * The result is invalidated when calling Pquery::add_placement() or other functions that change
 * the number of @link Pquery Pqueries@endlink or PqueryPlacement%s in the Sample.
 */
std::vector< std::vector< PqueryPlacement const* >> placements_per_edge(
    Sample const& smp,
    bool only_max_lwr_placements = false
);

/**
 * @brief Return a vector of all PqueryPlacement%s that are placed on the given PlacementTreeEdge.
 *
 * This functions iterates over all placements and collects those that are placed on the given
 * edge. In case that this is needed for multiple edges, it is faster to use
 * placements_per_edge( Sample ) instead.
 *
 * The result is invalidated when calling Pquery::add_placement() or other functions that change
 * the number of @link Pquery Pqueries@endlink or PqueryPlacement%s in the Sample.
 */
std::vector<PqueryPlacement const*> placements_per_edge(
    Sample            const& smp,
    PlacementTreeEdge const& edge
);

/**
 * @brief Return a vector that contains the number of PqueryPlacement%s per
 * @link ::PlacementTreeEdge edge@endlink of the @link ::PlacementTree tree@endlink of the Sample.
 *
 * The vector is indexed using the @link PlacementTreeEdge::index() index@endlink of the edges.
 */
std::vector<size_t> placement_count_per_edge( Sample const& sample );

utils::Matrix<size_t> placement_count_per_edge( SampleSet const& sample_set );

/**
 * @brief Return a vector that contains the sum of the weights of the PqueryPlacement%s per
 * @link ::PlacementTreeEdge edge@endlink of the @link ::PlacementTree tree@endlink of the Sample.
 *
 * The weight is measured in @link PqueryPlacement::like_weight_ratio `like_weight_ratio`@endlink.
 * The vector is indexed using the @link PlacementTreeEdge::index() index@endlink of the edges.
 */
std::vector<double> placement_weight_per_edge( Sample const& sample );

utils::Matrix<double> placement_weight_per_edge( SampleSet const& sample_set );

/**
 * @brief Return a plain representation of all pqueries of this map.
 *
 * This method produces a whole copy of all pqueries and their placements (though, not their names)
 * in a plain POD format. This format is meant for speeding up computations that need access to
 * the data a lot - which would require several pointer indirections in the normal representation
 * of the data.
 *
 * This comes of course at the cost of reduced flexibility, as all indices are fixed in the
 * plain data structre: changing a value here will not have any effect on the original data or
 * even on the values of the pqueries. Thus, most probably this will lead to corruption. Therefore,
 * this data structure is meant for reading only.
 */
std::vector<PqueryPlain> plain_queries( Sample const & smp );

// =================================================================================================
//     Verification
// =================================================================================================

/**
 * @brief Reset all edge nums of a ::PlacementTree.
 *
 * The `edge_num` property of the PlacementTreeEdge%s is defined by the `jplace` standard.
 * The values have to be assigned increasingly with a postorder traversal of the tree.
 * This function resets them so that this is established.
 *
 * See has_correct_edge_nums() to check whether the edge nums are already correct. This should
 * be the case for any valid `jplace` file.
 */
void reset_edge_nums( PlacementTree& tree );

/**
 * @brief Verify that the ::PlacementTree has no duplicate
 * @link PlacementEdgeData::edge_num() edge_nums@endlink and that they form consecutive
 * numbers starting from `0`.
 *
 * This function is very similar to has_correct_edge_nums(). However, instead of checking whether
 * the `edge_nums` are correctly assigned following a postorder traversal of the tree, as demanded
 * by the Jplace standard, this function simply checks whehter they are all unique, start at `0`
 * and continue consecutively without gaps.
 *
 * This is imporant for using the `edge_nums` as indices, for example.
 *
 * We offer this function, because Genesis can work with improperly assigned `edge_nums`, but
 * for some functions we need to ensure those properties. Generally, you should however prefer correct
 * `edge_nums` according to the standard, and use has_correct_edge_nums() to verify them.
 */
bool has_consecutive_edge_nums( PlacementTree const& tree );

/**
 * @brief Verify that the tree has correctly set edge nums.
 *
 * The `edge_num` property of the PlacementTreeEdge%s is defined by the `jplace` standard.
 * The values have to be assigned increasingly with a postorder traversal of the tree.
 * This function checks whether this is the case.
 *
 * See also has_consecutive_edge_nums() for a relaxed version of this function, which might also
 * be useful in some cases where the strict correct order according to the standard is not needed.
 */
bool has_correct_edge_nums( PlacementTree const& tree );

/**
 * @brief Validate the integrity of the pointers, references and data in a Sample object.
 *
 * Returns true iff everything is set up correctly. In case of inconsistencies, the function stops
 * and returns false on the first encountered error.
 *
 * If `check_values` is set to true, also a check on the validity of numerical values is done, for
 * example that the proximal_length is smaller than the corresponding branch_length.
 * If additionally `break_on_values` is set, validate() will stop on the first encountered invalid
 * value. Otherwise it will report all invalid values to the log stream.
 */
bool validate( Sample const& smp, bool check_values = false, bool break_on_values = false );

} // namespace placement
} // namespace genesis

#endif // include guard
