#ifndef GENESIS_PLACEMENT_FUNCTION_DISTANCES_H_
#define GENESIS_PLACEMENT_FUNCTION_DISTANCES_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/placement/placement_tree.hpp"
#include "genesis/utils/containers/matrix.hpp"

#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class Tree;

    class TreeEdge;
    class TreeNode;

    class DefaultNodeData;
    class DefaultEdgeData;

    using DefaultTree = Tree;
}

namespace placement {

    class Pquery;
    class PqueryPlacement;
    struct PqueryPlain;

    class PlacementEdgeData;
    class PlacementNodeData;

    class Sample;
    class SampleSet;
}

namespace placement {

// =================================================================================================
//     Pquery to Pquery Distances
// =================================================================================================

/**
 * @brief Calculate the weighted distance between two plain pqueries. It is mainly a helper
 * method for distance calculations (e.g., pairwise distance, variance).
 *
 * For each placement in the two pqueries, a distance is calculated, and their weighted sum is
 * returned. Weighing is done using the mass of placements in both pqueries.
 *
 * The distance between two placements is calculated as the shortest path between them. This
 * includes the their position on the branches, and - if specified - the pendant_length of both.
 * There are three cases that might occur:
 *
 *   1. **Both placements are on the same branch.**
 *      In this case, their distance is caluclated as their difference in proximal_lengths
 *      (plus the sum of their pendant_lengths, if specified).
 *
 *   2. **The path between the placements includes the root.**
 *      The distance of a placement from its neighbouring nodes is usually given in form of the
 *      proximal_length, which is the distance of the placement to the node (at the end of its
 *      branch) that lies in direction of the root. Thus, there is an implicit notion of a root,
 *      that we need to consider. If the path between two placements contains the root, we can
 *      directly calculate their distance as the distance between the two promixal nodes plus
 *      proximal_lengths (and possibly pendant_lengths) of both placements. We call this the
 *      promixal-promixal case.
 *
 *   3. **The root is not part of the path between the placements.**
 *      This case means that one of the two placements lies on the path between the other placement
 *      and the root -- thus, the path between the placements does not contain the root.
 *      The distance between the placements cannot be calculated using the proximal_lengths
 *      directly, but we need to get the distal_length (away from the root) of the inner placement
 *      first. This is simply the difference between branch_length and proximal_length of that
 *      placement. Of course, this case comes in two flavours, because both placements can be the
 *      inner or outer one. They are called proximal-distal case and distal-proximal case,
 *      respectively.
 *
 * The first case is easy to detect by comparing the edge nums. However, distinguishing between the
 * latter two cases is expensive, as it involves finding the path to the root for both placements.
 * To speed this up, we instead use a distance matrix that is calculated in the beginning of any
 * algorithm using this method and contains the pairwise distances between all nodes of the tree.
 * Using this, we do not need to find paths between placements, but simply go to the nodes at the
 * end of the branches of the placements and do a lookup for those nodes.
 *
 * With this technique, we can calculate the distances between the placements for all
 * three cases (promixal-promixal, proximal-distal and distal-proximal) cheaply. The wanted distance
 * is then simply the minimum of those three distances. This is correct, because the two wrong cases
 * will always produce an overestimation of the distance.
 *
 * This distance is normalized using the `like_weight_ratio` of both placements, before
 * summing it up to calculate the total distance between the pqueries.
 *
 * The Matrix @p node_distances has to come from tree::node_branch_length_distance_matrix().
 */
double pquery_distance(
    PqueryPlain const&           pquery_a,
    PqueryPlain const&           pquery_b,
    utils::Matrix<double> const& node_distances,
    bool                         with_pendant_length = false
);

/**
 * @brief Calculate the weighted distance between two @link Pquery Pqueries@endlink,
 * in branch length units, as the pairwise distance between their PqueryPlacement%s, and using
 * the `like_weight_ratio` for weighing.
 *
 * The Matrix @p node_distances has to come from tree::node_branch_length_distance_matrix().
 */
double pquery_distance(
    Pquery const&                pquery_a,
    Pquery const&                pquery_b,
    utils::Matrix<double> const& node_distances,
    bool                         with_pendant_length = false
);

/**
 * @brief Calculate the distance between two PqueryPlacement%s, using their positin on the
 * tree::TreeEdge%s, measured in branch length units.
 *
 * The Matrix @p node_distances has to come from tree::node_branch_length_distance_matrix().
 */
double placement_distance(
    PqueryPlacement const&       place_a,
    PqueryPlacement const&       place_b,
    utils::Matrix<double> const& node_distances
);

/**
 * @brief Calculate the weighted discrete distance between two @link Pquery Pqueries@endlink,
 * measured as the pairwise distance in number of nodes between between their PqueryPlacement%s,
 * and using the `like_weight_ratio` for weighing.
 *
 * The Matrix @p node_path_lengths has to come from tree::node_path_length_matrix().
 */
double pquery_path_length_distance(
    Pquery const&                pquery_a,
    Pquery const&                pquery_b,
    utils::Matrix<size_t> const& node_path_lengths
);

/**
 * brief Calculate the discrete distance between two PqueryPlacement%s, using their positin on the
 * tree::TreeEdge%s, measured in number of nodes between the placement locations.
 *
 * That is, two PqueryPlacement%s on the same edge have a distance of zero, on neighbouring edges
 * a distance of 1 (as there is one node in between), and so on.
 *
 * The Matrix @p node_path_lengths has to come from tree::node_path_length_matrix().
 */
size_t placement_path_length_distance(
    PqueryPlacement const&       place_a,
    PqueryPlacement const&       place_b,
    utils::Matrix<size_t> const& node_path_lengths
);

// =================================================================================================
//     Pquery to Tree Element Distances
// =================================================================================================

/**
 * @brief Calculate the weighted distance between the PqueryPlacement%s of a Pquery and a
 * tree::TreeNode, in branch length units, using the `like_weight_ratio` of the PqueryPlacement%s
 * for weighing.
 *
 * The Matrix @p node_distances has to come from tree::node_branch_length_distance_matrix().
 */
double pquery_distance(
    Pquery const&                pquery,
    tree::TreeNode const&        node,
    utils::Matrix<double> const& node_distances
);

/**
 * @brief Calculate the distance in branch length units between a PqueryPlacement and a
 * tree::TreeNode.
 *
 * The Matrix @p node_distances has to come from tree::node_branch_length_distance_matrix().
 */
double placement_distance(
    PqueryPlacement const&       placement,
    tree::TreeNode const&        node,
    utils::Matrix<double> const& node_distances
);

// /**
//  * @brief Calculate the weighted discrete distance between the PqueryPlacement%s of a Pquery and a
//  * tree::TreeNode, in number of nodes, using the `like_weight_ratio` of the PqueryPlacement%s
//  * for weighing.
//  *
//  * The Matrix @p node_path_lengths has to come from tree::node_path_length_matrix().
//  */
// double pquery_path_length_distance(
//     Pquery const&                pquery,
//     tree::TreeNode const&        node,
//     utils::Matrix<size_t> const& node_path_lengths
// );
//
// /**
//  * @brief
//  *
//  * The Matrix @p node_path_lengths has to come from tree::node_path_length_matrix().
//  */
// size_t placement_path_length_distance(
//     PqueryPlacement const&       placement,
//     tree::TreeNode const&        node,
//     utils::Matrix<size_t> const& node_path_lengths
// );

/**
 * @brief Calculate the weighted discrete distance between the PqueryPlacement%s of a Pquery and a
 * tree::TreeEdge, in number of nodes between them, using the `like_weight_ratio` of the
 * PqueryPlacement%s for weighing.
 *
 * The Matrix @p edge_path_lengths has to come from tree::edge_path_length_matrix().
 */
double pquery_path_length_distance(
    Pquery const&                pquery,
    tree::TreeEdge const&        edge,
    utils::Matrix<size_t> const& edge_path_lengths
);

/**
 * @brief Calculate the discrete distance from a PqueryPlacement to an edge, measured as the
 * number of nodes between them.
 *
 * The Matrix @p edge_path_lengths has to come from tree::edge_path_length_matrix().
 */
size_t placement_path_length_distance(
    PqueryPlacement const&       placement,
    tree::TreeEdge const&        edge,
    utils::Matrix<size_t> const& edge_path_lengths
);

} // namespace placement
} // namespace genesis

#endif // include guard
