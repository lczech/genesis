#ifndef GENESIS_TREE_DEFAULT_TREE_H_
#define GENESIS_TREE_DEFAULT_TREE_H_

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

#include "tree/tree.hpp"

#include <string>

namespace genesis {
namespace tree {

// =============================================================================
//     Typedefs
// =============================================================================

/**
 * @brief Alias for a Tree with data types DefaultNodeData and DefaultEdgeData
 */
using DefaultTree     = Tree;

/**
 * @brief Alias for a TreeEdge of a ::DefaultTree. See there for more information.
 */
using DefaultTreeEdge = TreeEdge;

/**
 * @brief Alias for a TreeLink of a ::DefaultTree. See there for more information.
 */
using DefaultTreeLink = TreeLink;

/**
 * @brief Alias for a TreeNode of a ::DefaultTree. See there for more information.
 */
using DefaultTreeNode = TreeNode;

// =================================================================================================
//     Default Tree Node Data
// =================================================================================================

/**
 * @brief Default class containing the commonly needed data for tree nodes.
 *
 * The Tree class can use all data for its nodes that derive from BaseNodeData.
 * In most cases, nodes will contain the node's name and edges will contain a branch length.
 *
 * This class is a default for node data, and can be derived from if a node name is wanted. It is
 * however not necessary -- so if there is no need for node names, you can provide your own,
 * class for tree, as long as it also derives from BaseNodeData.
 */
class DefaultNodeData : public BaseNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~DefaultNodeData() = default;

    // Move ctor and assignment.
    DefaultNodeData( DefaultNodeData&& )             = delete;
    DefaultNodeData& operator= ( DefaultNodeData&& ) = delete;

protected:

    DefaultNodeData() = default;

    // Copy ctor and assignment.
    DefaultNodeData( DefaultNodeData const& )             = default;
    DefaultNodeData& operator= ( DefaultNodeData const& ) = default;

public:

    static std::unique_ptr< DefaultNodeData > create()
    {
        return std::unique_ptr< DefaultNodeData >( new DefaultNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< DefaultNodeData >( new DefaultNodeData( *this ));
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
//     Default Tree Edge Data
// =================================================================================================

/**
 * @brief Default class containing the commonly needed data for tree edges.
 *
 * This class is the equivalent of DefaultNodeData for the tree edges. It stores a branch length
 * for each edge. For more information, see DefaultNodeData.
 *
 * If you for example do not need a single branch length, but multiple ones (e.g., when working with
 * partitions), simply do not derive from this class, but provide your own implementation.
 */
class DefaultEdgeData : public BaseEdgeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~DefaultEdgeData() = default;

    // Move ctor and assignment.
    DefaultEdgeData( DefaultEdgeData&& )             = delete;
    DefaultEdgeData& operator= ( DefaultEdgeData&& ) = delete;

protected:

    DefaultEdgeData() = default;

    // Copy ctor and assignment.
    DefaultEdgeData( DefaultEdgeData const& )             = default;
    DefaultEdgeData& operator= ( DefaultEdgeData const& ) = default;

public:

    static std::unique_ptr< DefaultEdgeData > create()
    {
        return std::unique_ptr< DefaultEdgeData >( new DefaultEdgeData() );
    };

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< DefaultEdgeData >( new DefaultEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief Branch length of the edge.
     *
     * Defaults to 1.0.
     */
    double branch_length = 1.0;

};

} // namespace tree
} // namespace genesis

#endif // include guard
