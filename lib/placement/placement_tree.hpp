#ifndef GENESIS_PLACEMENT_PLACEMENT_TREE_H_
#define GENESIS_PLACEMENT_PLACEMENT_TREE_H_

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
 * @ingroup placement
 */

#include <string>

#include "tree/default/tree.hpp"
#include "tree/tree.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Node Data
// =================================================================================================

/**
 * @brief Data class for @link PlacementTreeNode PlacementTreeNodes@endlink. Stores a node name.
 *
 * This class is derived from tree::DefaultTreeNodeData and currently adds no functionality on top
 * of this base class. See there for more information.
 * Also, see ::PlacementTree for information on how and where this class is used.
 */
class PlacementTreeNodeData : public tree::DefaultTreeNodeData
{
public:

};

// =================================================================================================
//     Placement Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for @link PlacementTreeEdge PlacementTreeEdges@endlink. Stores the branch
 * length of the edge, and the `edge_num`, as defined in the `jplace` standard.
 *
 * This class is derived from tree::DefaultTreeEdgeData. See there for more information.
 * Also, see ::PlacementTree for information on how and where this class is used.
 */
class PlacementTreeEdgeData : public tree::DefaultTreeEdgeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementTreeEdgeData &other) const
    {
        // TODO add a comparison of pqueries as well ?! is that good?!
        return other.branch_length == branch_length && other.edge_num_ == edge_num_;
    }

    inline bool operator != (const PlacementTreeEdgeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    inline std::string dump() const
    {
        return "Length: " + std::to_string(branch_length) + "\tEdge Num: " + std::to_string(edge_num_);
    }

    int edge_num() const;
    void reset_edge_num( int val );

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    int edge_num_;
};

// =================================================================================================
//     Definitions and Typedefs
// =================================================================================================

/**
 * @brief Type of @link tree::TreeEdge TreeEdge @endlink used for a ::PlacementTree.
 * See PlacementTreeEdgeData for the data stored on the edges.
 */
typedef tree::TreeEdge <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeEdge;

/**
* @brief Type of @link tree::TreeLink TreeLink @endlink used for a ::PlacementTree.
*/
typedef tree::TreeLink <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeLink;

/**
* @brief Type of @link tree::TreeNode TreeNode @endlink used for a ::PlacementTree.
* See PlacementTreeNodeData for the data stored on the nodes.
*/
typedef tree::TreeNode <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeNode;

/**
* @brief Type of @link tree::Tree Tree @endlink used for storing a tree with information needed for
* storing @link Pquery Pqueries@endlink. This tree type is used by Sample.
*
* This type of tree inherits the data of @link tree::DefaultTree DefaultTree@endlink, that is,
* it stores names for the nodes (usually those are taxa names) and branch lengths for the edges.
*
* In addition to that, each edge of this tree has a value `edge_num`. This is not the same as the
* internally used @link tree::TreeEdge::index index @endlink property of tree edges. Instead, it is
* a value defined by the `jplace` standard to identify edges. See Sample for more information.
*/
typedef tree::Tree     <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTree;

} // namespace placement
} // namespace genesis

#endif // include guard
