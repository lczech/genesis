#ifndef GENESIS_TREE_FUNCTION_EMD_TREE_H_
#define GENESIS_TREE_FUNCTION_EMD_TREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @ingroup tree
 */

#include <map>

namespace genesis {
namespace tree {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeEdge;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

template <class NodeDataType, class EdgeDataType>
class  TreeNode;

// =============================================================================
//     Emd Tree Node Data
// =============================================================================

/**
 * @brief Data class for @link EmdTreeNode EmdTreeNodes@endlink. Stores nothing.
 *
 * See @link EmdTree EmdTree@endlink for more information.
 */
class EmdTreeNodeData
{
public:

};

// =============================================================================
//     Emd Tree Edge Data
// =============================================================================

/**
 * @brief Data class for @link EmdTreeEdge EmdTreeEdges@endlink. Stores the branch length and
 * a list of masses with their positions along the edge.
 *
 * See @link EmdTree EmdTree@endlink for more information.
 */
class EmdTreeEdgeData
{
public:

    /**
     * @brief Length of this branch/edge.
     */
    double branch_length;

    /**
     * @brief List of masses stored on this branch, sorted by their position on the branch.
     *
     * This data member maps from a position on the branch to the mass at that position.
     * In order to be valid, the positions have to be in the interval [0.0, branch_length].
     * See @link validate( EmdTree const& tree, double valid_total_mass_difference )
     * validate() @endlink for a validation function.
     */
    std::map<double, double> masses;

};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

/**
 * @brief Tree type used to calculate the earth movers distance between to sets of masses
 * distributed on a Tree.
 *
 * See earth_movers_distance( EmdTree const& tree ) for more details on the purpose of this tree
 * type and on the earth movers distance in general.
 *
 * The branches of an EmdTree hold a list of @link EmdTreeEdgeData::masses masses@endlink, sorted
 * along their position on the branch.
 */
typedef Tree     <EmdTreeNodeData, EmdTreeEdgeData> EmdTree;

/**
 * @brief Edge type of a @link EmdTree EmdTree@endlink. See there for more information.
 */
typedef TreeEdge <EmdTreeNodeData, EmdTreeEdgeData> EmdTreeEdge;

/**
* @brief Link type of a @link EmdTree EmdTree@endlink. See there for more information.
*/
typedef TreeLink <EmdTreeNodeData, EmdTreeEdgeData> EmdTreeLink;

/**
* @brief Node type of a @link EmdTree EmdTree@endlink. See there for more information.
*/
typedef TreeNode <EmdTreeNodeData, EmdTreeEdgeData> EmdTreeNode;

} // namespace tree
} // namespace genesis

#endif // include guard
