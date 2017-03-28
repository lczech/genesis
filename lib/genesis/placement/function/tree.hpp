#ifndef GENESIS_PLACEMENT_FUNCTION_TREE_H_
#define GENESIS_PLACEMENT_FUNCTION_TREE_H_

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

#include <string>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree{
    class Tree;
}

namespace placement {

class Sample;

// =================================================================================================
//     Placement Tree Functions
// =================================================================================================

/**
 * @brief Produce a @link tree::Tree Tree@endlink where each PqueryPlacement of a Sample is turned
 * into an @link tree::TreeEdge Edge@endlink.
 *
 * The function takes the original @link PlacementTree Tree@endlink of the Sample, and for each
 * PqueryPlacement of the Sample, it attaches a new leaf @link tree::TreeNode Node@endlink to it,
 * positioned according to its @link PqueryPlacement::proximal_length proximal_length@endlink and
 * @link PqueryPlacement::pendant_length pendant_length@endlink. The resulting Tree is useful to
 * get an overview of the distribution of placements. It is mainly intended to view a few placements.
 * For large Samples, it might be a bit cluttered.
 *
 * Similar trees are produced by
 * [RAxML EPA](http://sco.h-its.org/exelixis/web/software/epa/index.html), where the file is called
 * `RAxML_labelledTree`, and by the [pplacer](http://matsen.fhcrc.org/pplacer/)
 * [guppy](http://matsen.github.io/pplacer/generated_rst/guppy.html)
 * [`tog` command](http://matsen.github.io/pplacer/generated_rst/guppy_tog.html#guppy-tog).
 * Both programs differ in the exact way the the placements are added as edges. To control this
 * behaviour, use the @p fully_resolve parameter.
 *
 * <b>Parameter `fully_resolve == false`</b>
 *
 * If @p fully_resolve is set to `false` (default), all placements at one edge are collected as
 * children of one central base edge:
 *
 * ![Multifurcating labelled tree.](placement/labelled_tree_multifurcating.png)
 *
 * This method is similar to the way RAxML produces a labelled tree.
 *
 * The base edge is positioned on the original edge at the average
 * @link PqueryPlacement::proximal_length proximal_length@endlink of the placements.
 * The base edge has a multifurcation if there are more than two placements on the edge.
 *
 * The @link PqueryPlacement::pendant_length pendant_length@endlink of the placements is used
 * to calculate the @link tree::DefaultEdgeData::branch_length branch_length@endlink of the new
 * placement edges. This calculation subtracts the shortest `pendant_length` of the placemetns on
 * the edge, so that the base edge is maximally "moved" towards the placement edges. This also
 * implies that at least one of the placement edges has `branch_length == 0.0`.
 * Furthermore, the placements are sorted by their `pendant_length`.
 *
 * Using this method, the new nodes of the resulting tree are easier to distinguish and collapse,
 * as all placements are collected under the base edge. However, this comes at the cost of losing
 * the detailled information of the `proximal_length` of the placements. If you want to keep this
 * information, use `fully_resolve == true` instead.
 *
 * <b>Parameter `fully_resolve == true`</b>
 *
 * If @p fully_resolve is set to `true`, the placements are turned into single leaf nodes:
 *
 * ![Fully resolved labelled tree.](placement/labelled_tree_fully_resolved.png)
 *
 * This method is similar to the way guppy `tog` produces a labelled tree.
 *
 * The original edge is splitted into separate parts where each placement edge is attached.
 * The `branch_lengths` between those parts are calculated using the
 * @link PqueryPlacement::proximal_length proximal_length@endlink of the placements, while the
 * `branch_lengths` of the placement edges use their
 * @link PqueryPlacement::pendant_length pendant_length@endlink.
 *
 * Using this method gives maximum information, but results in a more crowded tree. The new
 * placement edges are "sorted" along the original edge by their `proximal_length`. For this reason
 * in the example image above, "Query 2" is closer to "Node A" then "Query 1": it has a
 * higher `proximal_length`. This information was lost in the multifurcating tree from above.
 *
 * <b>Further Details</b>
 *
 * For edges that contain only a single placement, both versions of @p fully_resolve behave the
 * same. In this case, the placement is simply attached using its `proximal_length` and
 * `pendant_length`.
 *
 * @link Pquery Pqueries@endlink with multiple PqueryName%s are treated as if each name is a
 * separate placement, i.e., for each of them, a new (identical) edge is added to the Tree.
 * If using `fully_resolve ==  true`, this results in a `branch_length` of 0.0 between the
 * nodes of those placements.
 *
 * @param sample        Input Sample to get the @link PlacementTree Tree@endlink and
 *                      PqueryPlacement%s from.
 * @param fully_resolve Control in which way multiple placements at one edge are turned into new
 *                      edges. See above for details.
 * @param name_prefix   Specify a prefix to be added to all new leaf @link tree::TreeNode Nodes@endlink
 *                      (the ones that represent Placements). This is useful if a PqueryName also
 *                      occurs as a @link tree::DefaultNodeData::name name@endlink in the original
 *                      tree. By defalult, empty. In order to get the same naming as labelled trees
 *                      produces by RAxML, use `QUERY___`.
 *
 * @return A @link tree::DefaultTree DefaultTree@endlink, i.e., a tree with node names and branch
 * lengths.
 */
tree::Tree labelled_tree(
    Sample const&      sample,
    bool               fully_resolve = false,
    std::string const& name_prefix   = ""
);

/**
 * @brief Produce a @link tree::Tree Tree@endlink where each PqueryPlacement of a Sample is turned
 * into an @link tree::TreeEdge Edge@endlink.
 *
 * This function is an extension of labelled_tree( Sample const&, bool, std::string const& ) that
 * takes a custom Tree instead of using the one of the Sample. This allows to produce a labelled
 * Tree that can contain other data at its Nodes and Edges. This Tree has to be topologically
 * identical to the @link Sample::tree() Sample Tree@endlink.
 *
 * Furthermore, the data of the provided Tree needs to derived from
 * @link tree::DefaultNodeData DefaultNodeData@endlink and
 * @link tree::DefaultEdgeData DefaultEdgeData@endlink. This data is then copied to the resulting
 * Tree. The edge data of edges where new placement edges are added is kept that the topmost edge,
 * i.e., the one that is closest to the root.
 */
tree::Tree labelled_tree(
    Sample const&      sample,
    tree::Tree const&  tree,
    bool               fully_resolve = false,
    std::string const& name_prefix   = ""
);

} // namespace placement
} // namespace genesis

#endif // include guard
