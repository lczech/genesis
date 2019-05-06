#ifndef GENESIS_PLACEMENT_PLACEMENT_TREE_H_
#define GENESIS_PLACEMENT_PLACEMENT_TREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/tree.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Alias for a tree::Tree used for a tree with information needed
 * for storing @link Pquery Pqueries@endlink. This kind of tree is used by Sample.
 *
 * A PlacementTree inherits the data from tree::CommonTree, that is,
 * it stores names for the nodes (usually those are taxa names) and branch lengths for the edges.
 *
 * In addition to that, each edge of this tree has a value `edge_num`. This is not the same as the
 * internally used @link tree::TreeEdge::index index @endlink property of tree edges. Instead, it is
 * a value defined by the `jplace` standard to identify edges. See Sample for more information.
 */
using PlacementTree = tree::Tree;

/**
* @brief Alias for tree::TreeNode used in a ::PlacementTree.
* See PlacementNodeData for the data stored on the nodes.
*/
using PlacementTreeNode = tree::TreeNode;

/**
 * @brief Alias for tree::TreeEdge used in a ::PlacementTree.
 * See PlacementEdgeData for the data stored on the edges.
 */
using PlacementTreeEdge = tree::TreeEdge;

/**
 * @brief Alias for tree::TreeLink used in a ::PlacementTree.
 */
using PlacementTreeLink = tree::TreeLink;

// =================================================================================================
//     Placement Tree Node Data
// =================================================================================================

/**
 * @brief Data class for ::PlacementTreeNode%s. Stores a node name.
 *
 * This class is derived from tree::CommonNodeData and currently adds no functionality on top
 * of this base class. See there for more information.
 * Also, see ::PlacementTree for information on how and where this class is used.
 */
class PlacementNodeData : public tree::CommonNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~PlacementNodeData() override = default;

    // Move ctor and assignment.
    PlacementNodeData( PlacementNodeData&& )             = delete;
    PlacementNodeData& operator= ( PlacementNodeData&& ) = delete;

protected:

    PlacementNodeData() = default;

    // Copy ctor and assignment.
    PlacementNodeData( PlacementNodeData const& )             = default;
    PlacementNodeData& operator= ( PlacementNodeData const& ) = default;

public:

    static std::unique_ptr< PlacementNodeData > create()
    {
        return std::unique_ptr< PlacementNodeData >( new PlacementNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > recreate() const override
    {
        return std::unique_ptr< PlacementNodeData >( new PlacementNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< PlacementNodeData >( new PlacementNodeData( *this ));
    }

};

// =================================================================================================
//     Placement Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for PlacementTreeEdge%s. Stores the branch
 * length of the edge, and the `edge_num`, as defined in the `jplace` standard.
 *
 * This class is derived from tree::CommonEdgeData. See there for more information.
 * Also, see ::PlacementTree for information on how and where this class is used.
 */
class PlacementEdgeData : public tree::CommonEdgeData
{
public:

    // -------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------

    /**
     * @brief Data type used for the edge_num() property of the edges in a PlacementTree.
     */
    using EdgeNumType = int;

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    virtual ~PlacementEdgeData() override = default;

    // Move ctor and assignment.
    PlacementEdgeData( PlacementEdgeData&& )             = delete;
    PlacementEdgeData& operator= ( PlacementEdgeData&& ) = delete;

protected:

    PlacementEdgeData() = default;

    // Copy ctor and assignment.
    PlacementEdgeData( PlacementEdgeData const& )             = default;
    PlacementEdgeData& operator= ( PlacementEdgeData const& ) = default;

public:

    static std::unique_ptr< PlacementEdgeData > create()
    {
        return std::unique_ptr< PlacementEdgeData >( new PlacementEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > recreate() const override
    {
        return std::unique_ptr< PlacementEdgeData >( new PlacementEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< PlacementEdgeData >( new PlacementEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Functions
    // -----------------------------------------------------

    /**
     * @brief Return the `edge_num` of this edge. This value is defined by the `jplace` standard.
     */
    EdgeNumType edge_num() const
    {
        return edge_num_;
    }

    /**
     * @brief Force to set the edge_num to a certain value.
     *
     * The edge_num is usually considered to be constant for the tree. It rarely needs to be set,
     * except for when constructing the object. Use with care.
     */
    void reset_edge_num( EdgeNumType val )
    {
        edge_num_ = val;
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    EdgeNumType edge_num_;
};

} // namespace placement
} // namespace genesis

#endif // include guard
