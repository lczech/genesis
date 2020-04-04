#ifndef GENESIS_TREE_COMMON_TREE_TREE_H_
#define GENESIS_TREE_COMMON_TREE_TREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/tree/tree.hpp"

#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Alias for a Tree with data types CommonNodeData and CommonEdgeData
 */
using CommonTree     = Tree;

/**
 * @brief Alias for a TreeEdge of a ::CommonTree. See there for more information.
 */
using CommonTreeEdge = TreeEdge;

/**
 * @brief Alias for a TreeLink of a ::CommonTree. See there for more information.
 */
using CommonTreeLink = TreeLink;

/**
 * @brief Alias for a TreeNode of a ::CommonTree. See there for more information.
 */
using CommonTreeNode = TreeNode;

// =================================================================================================
//     Common Tree Node Data
// =================================================================================================

/**
 * @brief Common class containing the commonly needed data for tree nodes.
 *
 * The Tree class can use all data for its nodes that derive from BaseNodeData.
 * In most cases, nodes will contain the node's name and edges will contain a branch length.
 *
 * This class is a default for node data, and can be derived from if a node name is wanted. It is
 * however not necessary -- so if there is no need for node names, you can provide your own,
 * class for tree, as long as it also derives from BaseNodeData.
 */
class CommonNodeData : public BaseNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~CommonNodeData() override = default;

    // Move ctor and assignment.
    CommonNodeData( CommonNodeData&& )             = delete;
    CommonNodeData& operator= ( CommonNodeData&& ) = delete;

protected:

    CommonNodeData() = default;

    // Copy ctor and assignment.
    CommonNodeData( CommonNodeData const& )             = default;
    CommonNodeData& operator= ( CommonNodeData const& ) = default;

public:

    static std::unique_ptr< CommonNodeData > create()
    {
        return std::unique_ptr< CommonNodeData >( new CommonNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > recreate() const override
    {
        return std::unique_ptr< CommonNodeData >( new CommonNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< CommonNodeData >( new CommonNodeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief Name of the node.
     *
     * In case it is a leaf, this is usually the name of the taxon represented by the node.
     */
    std::string name;

};

// =================================================================================================
//     Common Tree Edge Data
// =================================================================================================

/**
 * @brief Common class containing the commonly needed data for tree edges.
 *
 * This class is the equivalent of CommonNodeData for the tree edges. It stores a branch length
 * for each edge. For more information, see CommonNodeData.
 *
 * If you for example do not need a single branch length, but multiple ones (e.g., when working with
 * partitions), simply do not derive from this class, but provide your own implementation.
 */
class CommonEdgeData : public BaseEdgeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~CommonEdgeData() override = default;

    // Move ctor and assignment.
    CommonEdgeData( CommonEdgeData&& )             = delete;
    CommonEdgeData& operator= ( CommonEdgeData&& ) = delete;

protected:

    CommonEdgeData() = default;

    // Copy ctor and assignment.
    CommonEdgeData( CommonEdgeData const& )             = default;
    CommonEdgeData& operator= ( CommonEdgeData const& ) = default;

public:

    static std::unique_ptr< CommonEdgeData > create()
    {
        return std::unique_ptr< CommonEdgeData >( new CommonEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > recreate() const override
    {
        return std::unique_ptr< CommonEdgeData >( new CommonEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< CommonEdgeData >( new CommonEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief Branch length of the edge.
     *
     * Initialized to 0.0. This allows to add default-created edges to the Tree without changing
     * any metrics related to the branch length.
     */
    double branch_length = 0.0;

};

} // namespace tree
} // namespace genesis

#endif // include guard
