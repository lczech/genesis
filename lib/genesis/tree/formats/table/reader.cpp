/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/formats/table/reader.hpp"

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Table Reader
// =================================================================================================

struct TreeTableHelpers
{
    // Map from node names to their node index in the tree.
    std::unordered_map<std::string, size_t> name_to_index;

    // Keep track of which node names appears as children and parents.
    std::unordered_set<std::string> child_names;
    std::unordered_set<std::string> parent_names;
};

void make_tree_from_parents_table_add_edge_(
    TreeTableHelpers& helpers,
    std::string const& child_name,
    std::string const& parent_name,
    Tree& tree
) {
    // Each child can only have a single parent.
    if( helpers.child_names.count( child_name ) > 0 ) {
        throw std::runtime_error(
            "Node name \"" + child_name + "\" appears multiple times as a child node"
        );
    }
    helpers.child_names.insert( child_name );
    helpers.parent_names.insert( parent_name );

    // Shortcut to tree containers.
    auto& links = tree.expose_link_container();
    auto& nodes = tree.expose_node_container();
    auto& edges = tree.expose_edge_container();

    // Helper function to add a node to the tree and to the map.
    auto add_named_node_ = [&]( std::string const& node_name )
    {
        // Get the index that the node will have in the tree.
        auto const node_index = nodes.size();

        // Create a new node. For now, we use CommonNodeData for simplicty.
        // We set all properties except for the link, which is done later.
        auto new_node = utils::make_unique< TreeNode >();
        new_node->reset_index( node_index );
        new_node->reset_data( CommonNodeData::create() );
        new_node->data<CommonNodeData>().name = node_name;

        // Finally, add everything to the tree and store it in the map.
        nodes.push_back( std::move( new_node ));
        helpers.name_to_index[ node_name ] = node_index;

        return node_index;
    };

    // See if there is a parent with the name already.
    // If not, we create it first, in the tree and in the map.
    bool is_new_parent = false;
    if( helpers.name_to_index.count( parent_name ) == 0 ) {
        add_named_node_( parent_name );
        is_new_parent = true;
    }

    // Same for the child.
    bool is_new_child = false;
    if( helpers.name_to_index.count( child_name ) == 0 ) {
        add_named_node_( child_name );
        is_new_child = true;
    }

    // Now we can get both nodes from the map.
    auto* parent_node = &tree.node_at( helpers.name_to_index[ parent_name ]);
    auto* child_node  = &tree.node_at( helpers.name_to_index[ child_name ]);

    // At this point, we have the nodes, but not the links and the edge.
    // and the nodes are missing the respective pointers to those.

    // First create the new elements we need, to have all pointers.
    auto parent_link = utils::make_unique< TreeLink >();
    auto child_link  = utils::make_unique< TreeLink >();
    auto new_edge    = utils::make_unique< TreeEdge >();

    // At the parent, we make a new link to connect to the child node.
    // If we just created the parent, this is the first link of the parent,
    // so we need special treatment.
    parent_link->reset_index( links.size() );
    if( is_new_parent ) {
        parent_link->reset_next( parent_link.get() );
        parent_node->reset_primary_link( parent_link.get() );
    } else {
        parent_link->reset_next( &parent_node->primary_link().next() );
        parent_node->primary_link().reset_next( parent_link.get() );
    }
    parent_link->reset_outer( child_link.get() );
    parent_link->reset_node( parent_node );
    parent_link->reset_edge( new_edge.get() );

    // Now the child link. Similar to the parent, but we additionally need to reset
    // its primary link to the new edge, in case that it existed as a singular
    // edge before, in order to connect it in the correct direction.
    child_link->reset_index( links.size() + 1 );
    if( is_new_child ) {
        child_link->reset_next( child_link.get() );
        child_node->reset_primary_link( child_link.get() );
    } else {
        child_link->reset_next( &child_node->primary_link().next() );
        child_node->primary_link().reset_next( child_link.get() );
        child_node->reset_primary_link( child_link.get() );
    }
    child_link->reset_outer( parent_link.get() );
    child_link->reset_node( child_node );
    child_link->reset_edge( new_edge.get() );

    // Set up the new edge to connect the node to its parent.
    new_edge->reset_index( edges.size() );
    new_edge->reset_primary_link( parent_link.get() );
    new_edge->reset_secondary_link( child_link.get() );
    new_edge->reset_data( CommonEdgeData::create() );

    // Finally move everything to the tree.
    links.push_back( std::move( parent_link ));
    links.push_back( std::move( child_link ));
    edges.push_back( std::move( new_edge ));
}

Tree make_tree_from_parents_table(
    std::vector<std::string> const& child_names,
    std::vector<std::string> const& parent_names
) {
    if( child_names.size() != parent_names.size() ) {
        throw std::runtime_error(
            "Cannot create tree from parents table with different number of entries in columns"
        );
    }
    if( child_names.empty() ) {
        return Tree();
    }

    // Resulting tree.
    Tree tree;

    // Make a hash table from node names to their node index in the tree.
    // Use that to iterate through, once, and create all nodes.
    TreeTableHelpers helpers;
    for( size_t i = 0; i < parent_names.size(); ++i ) {
        make_tree_from_parents_table_add_edge_(
            helpers, child_names[i], parent_names[i], tree
        );
    }

    // We now have a tree with all elements set up correctly, but not the root.
    // We just check all nodes for the root propterty, and set the tree root to that.
    // If multiple nodes fit that, we have an error, as that's a forest.
    // That's hence a data integrity check.
    std::vector<std::string> root_nodes;
    for( auto& node : tree.nodes() ) {
        if( is_root( node )) {
            tree.reset_root_link( &node.primary_link() );
            root_nodes.push_back( node.data<CommonNodeData>().name );
        }
    }
    if( root_nodes.size() != 1 ) {
        throw std::invalid_argument(
            "Provided list of child and parent nodes does not form a singe tree, but a forest with " +
            std::to_string( root_nodes.size() ) + " root nodes: " + utils::join( root_nodes )
        );
    }
    assert( tree.node_count() == child_names.size() + 1 );

    // // We now want to find all names that are only parents, but not also children.
    // // These are the root nodes. If there is more than one, this is an error.
    // // This hence also serves as a sanity check for the tree root.
    // size_t root_node_index = tree.node_count();
    // for( auto const& parent : helpers.parent_names ) {
    //     if( helpers.child_names.count( parent ) == 0 ) {
    //         // Found a parent.
    //         if( root_node_index != tree.node_count() ) {
    //             throw std::runtime_error(
    //                 "Node name lists contain multiple parent nodes."
    //             );
    //         }
    //         assert( helpers.name_to_index.count( parent ) > 0 );
    //         root_node_index = helpers.name_to_index[ parent ];
    //     }
    // }
    // assert( tree.root_node().index() == root_node_index );

    return tree;
}

} // namespace tree
} // namespace genesis
