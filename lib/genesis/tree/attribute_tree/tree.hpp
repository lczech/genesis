#ifndef GENESIS_TREE_ATTRIBUTE_TREE_TREE_H_
#define GENESIS_TREE_ATTRIBUTE_TREE_TREE_H_

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/operators.hpp"

#include <map>
#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Alias for a Tree that stores TreeNode%s and TreeEdge%s with string attributes on them.
 */
using AttributeTree = Tree;

/**
 * @brief Alias for a TreeEdge of an @link AttributeTree AttributeTree@endlink.
 * See there for more information.
 */
using AttributeTreeEdge = TreeEdge;

/**
 * @brief Alias for a TreeLink of an @link AttributeTree AttributeTree@endlink.
 * See there for more information.
 */
using AttributeTreeLink = TreeLink;

/**
 * @brief Alias for a TreeNode of an @link AttributeTree AttributeTree@endlink.
 * See there for more information.
 */
using AttributeTreeNode = TreeNode;

/**
 * @brief Alias for the map type used by an #AttributeTree.
 *
 * We define this alias in the namespace instead of inside of a class, because it is used
 * in multiple places. Defining it here once allows to easily change the type in the future,
 * should that be needed.
 *
 * See AttributeTreeNodeData and AttributeTreeEdgeData for the data classes where this type
 * is used.
 */
using AttributeTreeMap = std::map<std::string, std::string>;

// =================================================================================================
//     Attribute Tree Node Data
// =================================================================================================

/**
 * @brief Data class for @link AttributeTreeNode AttributeTreeNodes@endlink.
 *
 * See @link AttributeTree AttributeTree@endlink for more information.
 */
class AttributeTreeNodeData : public DefaultNodeData
{
public:

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    virtual ~AttributeTreeNodeData() = default;

    // Move ctor and assignment.
    AttributeTreeNodeData( AttributeTreeNodeData&& )             = delete;
    AttributeTreeNodeData& operator= ( AttributeTreeNodeData&& ) = delete;

protected:

    AttributeTreeNodeData() = default;

    // Copy ctor and assignment.
    AttributeTreeNodeData( AttributeTreeNodeData const& )             = default;
    AttributeTreeNodeData& operator= ( AttributeTreeNodeData const& ) = default;

public:

    static std::unique_ptr< AttributeTreeNodeData > create()
    {
        return std::unique_ptr< AttributeTreeNodeData >( new AttributeTreeNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > recreate() const override
    {
        return std::unique_ptr< AttributeTreeNodeData >( new AttributeTreeNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< AttributeTreeNodeData >( new AttributeTreeNodeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief List of attributes stored on this node.
     */
    AttributeTreeMap attributes;

};

// =================================================================================================
//     Attribute Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for @link AttributeTreeEdge AttributeTreeEdges@endlink.
 *
 * See @link AttributeTree AttributeTree@endlink for more information.
 */
class AttributeTreeEdgeData : public DefaultEdgeData
{
public:

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    virtual ~AttributeTreeEdgeData() = default;

    // Move ctor and assignment.
    AttributeTreeEdgeData( AttributeTreeEdgeData&& )             = delete;
    AttributeTreeEdgeData& operator= ( AttributeTreeEdgeData&& ) = delete;

protected:

    AttributeTreeEdgeData() = default;

    // Copy ctor and assignment.
    AttributeTreeEdgeData( AttributeTreeEdgeData const& )             = default;
    AttributeTreeEdgeData& operator= ( AttributeTreeEdgeData const& ) = default;

public:

    static std::unique_ptr< AttributeTreeEdgeData > create()
    {
        return std::unique_ptr< AttributeTreeEdgeData >( new AttributeTreeEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > recreate() const override
    {
        return std::unique_ptr< AttributeTreeEdgeData >( new AttributeTreeEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< AttributeTreeEdgeData >( new AttributeTreeEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief List of attributes stored on this edge.
     */
    AttributeTreeMap attributes;

};

// =================================================================================================
//     Default Converter
// =================================================================================================

/**
 * @brief Helper function that takes a DefaultTree (or any Tree with Node and Edge data derived
 * from it) and turns its data into an AttributeTree, that is, a Tree with
 * @link AttributeTreeNodeData AttributeTreeNodeData@endlink and
 * @link AttributeTreeEdgeData AttributeTreeEdgeData@endlink.
 */
inline AttributeTree convert_default_tree_to_attribute_tree( DefaultTree const& source )
{
    return convert(
        source,
        [] ( tree::BaseNodeData const& node_data ) {
            auto attr_node = tree::AttributeTreeNodeData::create();
            auto const& orig_node = dynamic_cast< DefaultNodeData const& >( node_data );
            attr_node->name = orig_node.name;

            return attr_node;
        },
        [] ( tree::BaseEdgeData const& edge_data ) {
            auto attr_edge = tree::AttributeTreeEdgeData::create();
            auto const& orig_edge = dynamic_cast< DefaultEdgeData const& >( edge_data );
            attr_edge->branch_length = orig_edge.branch_length;

            return attr_edge;
        }
    );
}

} // namespace tree
} // namespace genesis

#endif // include guard
