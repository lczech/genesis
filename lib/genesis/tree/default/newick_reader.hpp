#ifndef GENESIS_TREE_DEFAULT_NEWICK_READER_H_
#define GENESIS_TREE_DEFAULT_NEWICK_READER_H_

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
#include "genesis/tree/formats/newick/element.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Newick Reader Plugin
// =================================================================================================

/**
 * @brief
 */
class DefaultTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    DefaultTreeNewickReaderPlugin() = default;
    virtual ~DefaultTreeNewickReaderPlugin() = default;

    DefaultTreeNewickReaderPlugin(DefaultTreeNewickReaderPlugin const&) = default;
    DefaultTreeNewickReaderPlugin(DefaultTreeNewickReaderPlugin&&)      = default;

    DefaultTreeNewickReaderPlugin& operator= (DefaultTreeNewickReaderPlugin const&) = default;
    DefaultTreeNewickReaderPlugin& operator= (DefaultTreeNewickReaderPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void element_to_node( NewickBrokerElement const& element, TreeNode& node ) const
    {
        std::string name = element.name;

        // Insert default names if needed.
        if( name.empty() && use_default_names ) {
            if( element.rank() == 0 ) {
                name = default_leaf_name;
            } else if( element.depth == 0 ) {
                name = default_root_name;
            } else {
                name = default_internal_name;
            }
        }

        // Handle underscores/spaces.
        if( replace_name_underscores ) {
            name = utils::replace_all(name, "_", " ");
        }

        node.data<DefaultNodeData>().name = name;
    }

    void element_to_edge( NewickBrokerElement const& element, TreeEdge& edge ) const
    {
        // We assume that the branch length is always the first (or only) value.
        // If there is an interpretation where this is not the case, it is best to introduce
        // an array index for this as a paramter of this class.
        if( element.values.size() > 0 ) {
            edge.data<DefaultEdgeData>().branch_length = std::stod( element.values[0] );
        } else {
            edge.data<DefaultEdgeData>().branch_length = 1.0;
        }
    }

    void register_with( NewickReader& reader ) const
    {
        // Set node data creation function.
        reader.create_node_data_plugin = []( TreeNode& node ){
            node.reset_data( DefaultNodeData::create() );
        };

        // Set edge data creation function.
        reader.create_edge_data_plugin = []( TreeEdge& edge ){
            edge.reset_data( DefaultEdgeData::create() );
        };

        // Add node manipulation functions.
        reader.element_to_node_plugins.push_back(
            [&]( NewickBrokerElement const& element, TreeNode& node ) {
                element_to_node( element, node );
            }
        );

        // Add edge manipulation functions.
        reader.element_to_edge_plugins.push_back(
            [&]( NewickBrokerElement const& element, TreeEdge& edge ) {
                element_to_edge( element, edge );
            }
        );

        // Alternative version using bind.
        // reader.element_to_edge_plugins.push_back(
        //     std::bind(
        //         &DefaultTreeNewickReaderPlugin::element_to_edge,
        //         this,
        //         std::placeholders::_1,
        //         std::placeholders::_2
        //     )
        // );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    // TODO for now, this is all public. use getters and setters instead, and outsource those
    // properties that belong to the (yet to create) superclass DefaultNewickPluginBase or so.

    std::string default_leaf_name     = "Leaf_Node";
    std::string default_internal_name = "Internal_Node";
    std::string default_root_name     = "Root_Node";

    /**
     * @brief If set to true, unnamed nodes are named using one of the default names.
     *
     * The default names can be set using #default_leaf_name, #default_internal_name and
     * #default_root_name.
     */
    bool        use_default_names = false;

    bool        replace_name_underscores = false;

};

// =================================================================================================
//     Default Tree Newick Reader
// =================================================================================================

/**
 * @brief Read default Newick trees, i.e., trees with names and branch lengths.
 *
 * This class is a convenience wrapper that combines a NewickReader with a
 * DefaultTreeNewickReaderPlugin. It is intended to be used for standard use cases, and produces a
 * Tree with DefaultNodeData and DefaultEdgeData at its nodes and edges.
 *
 * It is also possible to register additional plugins on top of this class.
 *
 * Behind the curtain, this class derives from both NewickReader and DefaultTreeNewickReaderPlugin.
 * This is a bit ugly, but we use it for simplicity. This allows to use an instance as if it was
 * a reader (i.e., call `from_...` functions), but also change the plugin settings in a natural
 * way.
 */
class DefaultTreeNewickReader
    : public NewickReader
    , public DefaultTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    DefaultTreeNewickReader()
    {
        // This is mindfuck. We derive from two classes - the function register_with() calls
        // the plugin function of DefaultTreeNewickReaderPlugin, and uses our own inherited
        // NewickReader instance as argument. Thus, it registeres its own plugin part with its own
        // reader part.
        register_with( *this );
    }
};

} // namespace tree
} // namespace genesis

#endif // include guard
