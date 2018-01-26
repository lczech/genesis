#ifndef GENESIS_TREE_DRAWING_LAYOUT_TREE_H_
#define GENESIS_TREE_DRAWING_LAYOUT_TREE_H_

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
 *
 * It is derived from tree::DefaultNodeData, so that it already contains a
 * @link tree::DefaultNodeData::name name@endlink field.
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

    /**
     * @brief Distance of the node to the root.
     *
     * For rectangular trees, this is the x coordinate,
     * while for circular trees, this is the radius.
     * This is normalized so that the node furthest away from the root has a distance of 1.
     */
    double distance = -1.0;

    /**
     * @brief Position of the node along the second axis.
     *
     * As phylogenies usually only need to display branch length, the second dimension
     * is only necessary so that the lines of the branches are actually distinguishable
     * from each other.
     * This value is used for that purpose. That is, for rectangular trees, this is the y coordiante,
     * while for circular trees, this is the angle.
     * It is normalized so that the largest spreading (for the last node of the tree) is 1.
     */
    double spreading = -1.0;

    int parent_index = -1;

    utils::SvgGroup shape;

};

// =================================================================================================
//     Layout Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for LayoutTreeEdge%s.
 *
 * It is derived from tree::DefaultEdgeData, so that it already contains a
 * @link tree::DefaultEdgeData::branch_length branch_length@endlink field.
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

    utils::SvgStroke spreading_stroke;
    utils::SvgStroke distance_stroke;

    utils::SvgGroup shape;

};

} // namespace tree
} // namespace genesis

#endif // include guard
