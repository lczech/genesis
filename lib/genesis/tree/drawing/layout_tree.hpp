#ifndef GENESIS_TREE_DRAWING_LAYOUT_TREE_H_
#define GENESIS_TREE_DRAWING_LAYOUT_TREE_H_

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

#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/formats/svg/svg.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Alias for a tree::Tree used for a tree with information needed for tree drawing.
 */
using LayoutTree     = tree::Tree;

/**
* @brief Alias for tree::TreeNode used in a ::LayoutTree.
* See LayoutNodeData for the data stored on the nodes.
*/
using LayoutTreeNode = tree::TreeNode;

/**
 * @brief Alias for tree::TreeEdge used in a ::LayoutTree.
 * See LayoutEdgeData for the data stored on the edges.
 */
using LayoutTreeEdge = tree::TreeEdge;

/**
 * @brief Alias for tree::TreeLink used in a ::LayoutTree.
 */
using LayoutTreeLink = tree::TreeLink;

// =================================================================================================
//     Layout Tree Node Data
// =================================================================================================

/**
 * @brief Data class for ::LayoutTreeNode%s.
 */
class LayoutNodeData : public tree::DefaultNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~LayoutNodeData() = default;

    // Move ctor and assignment.
    LayoutNodeData( LayoutNodeData&& )             = delete;
    LayoutNodeData& operator= ( LayoutNodeData&& ) = delete;

protected:

    LayoutNodeData() = default;

    // Copy ctor and assignment.
    LayoutNodeData( LayoutNodeData const& )             = default;
    LayoutNodeData& operator= ( LayoutNodeData const& ) = default;

public:

    static std::unique_ptr< LayoutNodeData > create()
    {
        return std::unique_ptr< LayoutNodeData >( new LayoutNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > recreate() const override
    {
        return std::unique_ptr< LayoutNodeData >( new LayoutNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< LayoutNodeData >( new LayoutNodeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    double x;
    double y;

};

// =================================================================================================
//     Layout Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for LayoutTreeEdge%s.
 */
class LayoutEdgeData : public tree::DefaultEdgeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~LayoutEdgeData() = default;

    // Move ctor and assignment.
    LayoutEdgeData( LayoutEdgeData&& )             = delete;
    LayoutEdgeData& operator= ( LayoutEdgeData&& ) = delete;

protected:

    LayoutEdgeData() = default;

    // Copy ctor and assignment.
    LayoutEdgeData( LayoutEdgeData const& )             = default;
    LayoutEdgeData& operator= ( LayoutEdgeData const& ) = default;

public:

    static std::unique_ptr< LayoutEdgeData > create()
    {
        return std::unique_ptr< LayoutEdgeData >( new LayoutEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > recreate() const override
    {
        return std::unique_ptr< LayoutEdgeData >( new LayoutEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< LayoutEdgeData >( new LayoutEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    utils::SvgStroke stroke;
};

} // namespace tree
} // namespace genesis

#endif // include guard
