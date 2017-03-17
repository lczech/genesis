#ifndef GENESIS_TREE_DRAWING_CIRCULAR_TREE_H_
#define GENESIS_TREE_DRAWING_CIRCULAR_TREE_H_

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
using CircularTree     = tree::Tree;

/**
* @brief Alias for tree::TreeNode used in a ::CircularTree.
* See CircularNodeData for the data stored on the nodes.
*/
using CircularTreeNode = tree::TreeNode;

/**
 * @brief Alias for tree::TreeEdge used in a ::CircularTree.
 * See CircularEdgeData for the data stored on the edges.
 */
using CircularTreeEdge = tree::TreeEdge;

/**
 * @brief Alias for tree::TreeLink used in a ::CircularTree.
 */
using CircularTreeLink = tree::TreeLink;

// =================================================================================================
//     Circular Tree Node Data
// =================================================================================================

/**
 * @brief Data class for ::CircularTreeNode%s.
 */
class CircularNodeData : public tree::LayoutNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~CircularNodeData() = default;

    // Move ctor and assignment.
    CircularNodeData( CircularNodeData&& )             = delete;
    CircularNodeData& operator= ( CircularNodeData&& ) = delete;

protected:

    CircularNodeData() = default;

    // Copy ctor and assignment.
    CircularNodeData( CircularNodeData const& )             = default;
    CircularNodeData& operator= ( CircularNodeData const& ) = default;

public:

    static std::unique_ptr< CircularNodeData > create()
    {
        return std::unique_ptr< CircularNodeData >( new CircularNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > recreate() const override
    {
        return std::unique_ptr< CircularNodeData >( new CircularNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< CircularNodeData >( new CircularNodeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    int parent_index = -1;

    double children_min_a = -1.0;
    double children_max_a = -1.0;

    double r = -1.0;
    double a = -1.0;

};

// =================================================================================================
//     Circular Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for CircularTreeEdge%s.
 */
class CircularEdgeData : public tree::LayoutEdgeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~CircularEdgeData() = default;

    // Move ctor and assignment.
    CircularEdgeData( CircularEdgeData&& )             = delete;
    CircularEdgeData& operator= ( CircularEdgeData&& ) = delete;

protected:

    CircularEdgeData() = default;

    // Copy ctor and assignment.
    CircularEdgeData( CircularEdgeData const& )             = default;
    CircularEdgeData& operator= ( CircularEdgeData const& ) = default;

public:

    static std::unique_ptr< CircularEdgeData > create()
    {
        return std::unique_ptr< CircularEdgeData >( new CircularEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > recreate() const override
    {
        return std::unique_ptr< CircularEdgeData >( new CircularEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< CircularEdgeData >( new CircularEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

};

} // namespace tree
} // namespace genesis

#endif // include guard
