#ifndef GENESIS_TREE_DRAWING_RECTANGULAR_TREE_H_
#define GENESIS_TREE_DRAWING_RECTANGULAR_TREE_H_

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
 * @ingroup tree
 */

#include "genesis/tree/drawing/layout_tree.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Alias for a tree::Tree used for a tree with information needed for tree drawing.
 */
using RectangularTree     = tree::Tree;

/**
* @brief Alias for tree::TreeNode used in a ::RectangularTree.
* See RectangularNodeData for the data stored on the nodes.
*/
using RectangularTreeNode = tree::TreeNode;

/**
 * @brief Alias for tree::TreeEdge used in a ::RectangularTree.
 * See RectangularEdgeData for the data stored on the edges.
 */
using RectangularTreeEdge = tree::TreeEdge;

/**
 * @brief Alias for tree::TreeLink used in a ::RectangularTree.
 */
using RectangularTreeLink = tree::TreeLink;

// =================================================================================================
//     Rectangular Tree Node Data
// =================================================================================================

/**
 * @brief Data class for ::RectangularTreeNode%s.
 */
class RectangularNodeData : public tree::LayoutNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~RectangularNodeData() = default;

    // Move ctor and assignment.
    RectangularNodeData( RectangularNodeData&& )             = delete;
    RectangularNodeData& operator= ( RectangularNodeData&& ) = delete;

protected:

    RectangularNodeData() = default;

    // Copy ctor and assignment.
    RectangularNodeData( RectangularNodeData const& )             = default;
    RectangularNodeData& operator= ( RectangularNodeData const& ) = default;

public:

    static std::unique_ptr< RectangularNodeData > create()
    {
        return std::unique_ptr< RectangularNodeData >( new RectangularNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > recreate() const override
    {
        return std::unique_ptr< RectangularNodeData >( new RectangularNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< RectangularNodeData >( new RectangularNodeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    int parent_index = -1;

    double children_min_y = -1.0;
    double children_max_y = -1.0;

};

// =================================================================================================
//     Rectangular Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for RectangularTreeEdge%s.
 */
class RectangularEdgeData : public tree::LayoutEdgeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~RectangularEdgeData() = default;

    // Move ctor and assignment.
    RectangularEdgeData( RectangularEdgeData&& )             = delete;
    RectangularEdgeData& operator= ( RectangularEdgeData&& ) = delete;

protected:

    RectangularEdgeData() = default;

    // Copy ctor and assignment.
    RectangularEdgeData( RectangularEdgeData const& )             = default;
    RectangularEdgeData& operator= ( RectangularEdgeData const& ) = default;

public:

    static std::unique_ptr< RectangularEdgeData > create()
    {
        return std::unique_ptr< RectangularEdgeData >( new RectangularEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > recreate() const override
    {
        return std::unique_ptr< RectangularEdgeData >( new RectangularEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< RectangularEdgeData >( new RectangularEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

};

} // namespace tree
} // namespace genesis

#endif // include guard
