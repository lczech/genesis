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
 * @brief Data class for @link EmdTreeNode EmdTreeNodes @endlink. Stores nothing.
 */
class EmdTreeNodeData
{
public:

};

// =============================================================================
//     Emd Tree Edge Data
// =============================================================================

/**
 * @brief Data class for @link EmdTreeEdge EmdTreeEdges @endlink. Stores the branch length and
 * a list of masses with their positions along the edge.
 */
class EmdTreeEdgeData
{
public:

    double branch_length;

    std::map<double, double> masses;

};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

/**
 * @brief Short name for a Tree using the Emd data types EmdTreeNodeData and EmdTreeEdgeData.
 */
typedef Tree     <EmdTreeNodeData, EmdTreeEdgeData> EmdTree;

typedef TreeEdge <EmdTreeNodeData, EmdTreeEdgeData> EmdTreeEdge;
typedef TreeLink <EmdTreeNodeData, EmdTreeEdgeData> EmdTreeLink;
typedef TreeNode <EmdTreeNodeData, EmdTreeEdgeData> EmdTreeNode;

} // namespace tree
} // namespace genesis

#endif // include guard
