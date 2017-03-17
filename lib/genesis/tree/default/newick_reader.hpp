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
 * @brief Provide a set of plugin functions for NewickReader to read a #DefaultTree.
 */
class DefaultTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = DefaultTreeNewickReaderPlugin;

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
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Get the default branch length used when there is none given for an edge.
     */
    double default_branch_length() const
    {
        return default_branch_length_;
    }

    /**
     * @brief Set the default branch length used when there is none given for an edge.
     *
     * By default, this is `1.0`.
     */
    self_type& default_branch_length( double value )
    {
        default_branch_length_ = value;
        return *this;
    }

    /**
     * @brief Get the default named used when there is none given for a leaf node.
     */
    std::string const& default_leaf_name() const
    {
        return default_leaf_name_;
    }

    /**
     * @brief Set the default named used when there is none given for a leaf node.
     */
    self_type& default_leaf_name( std::string const& value )
    {
        default_leaf_name_ = value;
        return *this;
    }

    /**
     * @brief Get the default named used when there is none given for an inner node.
     */
    std::string const& default_inner_name() const
    {
        return default_inner_name_;
    }

    /**
     * @brief Set the default named used when there is none given for an inner node.
     */
    self_type& default_inner_name( std::string const& value )
    {
        default_inner_name_ = value;
        return *this;
    }

    /**
     * @brief Get the default named used when there is none given for the root node.
     */
    std::string const& default_root_name() const
    {
        return default_root_name_;
    }

    /**
     * @brief Set the default named used when there is none given for the root node.
     */
    self_type& default_root_name( std::string const& value )
    {
        default_root_name_ = value;
        return *this;
    }

    /**
     * @brief Shorthand to set the default names for leaf, inner and root node at once, to one
     * value.
     */
    self_type& set_default_names( std::string const& value )
    {
        default_leaf_name_ = value;
        default_inner_name_ = value;
        default_root_name_ = value;
        return *this;
    }

    /**
     * @brief Return whether currently default names are activated in this plugin.
     *
     * See the setter use_default_names( bool ) for details.
     */
    bool use_default_names() const
    {
        return use_default_names_;
    }

    /**
     * @brief Set whether to replace unnamed nodes with a default name.
     *
     * Default is `false`. In this case, nodes without names in the Newick tree are simply unnamed,
     * i.e., their name is the emptry string.
     *
     * If set to `true`, unnamed nodes are named using one of the default names:
     *
     *  * Leaf nodes are named using default_leaf_name().
     *  * Inner nodes are named using default_inner_name().
     *  * The root node is named using default_root_name().
     *
     * These default names can be changed by using default_leaf_name( std::string const& ),
     * default_inner_name( std::string const& ) and default_root_name( std::string const& ),
     * or by using set_default_names( std::string const& ) to set all three at once.
     */
    self_type& use_default_names( bool value )
    {
        use_default_names_ = value;
        return *this;
    }

    /**
     * @brief Return whether currently this plugin replaces underscores with spaces.
     *
     * See the setter replace_name_underscores( bool ) for details.
     */
    bool replace_name_underscores() const
    {
        return replace_name_underscores_;
    }

    /**
     * @brief Set whether to replace all underscores ('_') in names with spaces (' ').
     *
     * This is demanded by the original definition of the Newick format, see
     * http://evolution.genetics.washington.edu/phylip/newicktree.html
     * However, because under most common circumstances, it is more confusing than helpful, we
     * decided to deactivate this by default. Thus, the default for this setting is `false`.
     */
    self_type& replace_name_underscores( bool value )
    {
        replace_name_underscores_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void element_to_node( NewickBrokerElement const& element, TreeNode& node ) const
    {
        std::string name = element.name;

        // Insert default names if needed.
        if( name.empty() && use_default_names_ ) {
            if( element.is_leaf() ) {
                name = default_leaf_name_;
            } else if( element.is_root() ) {
                name = default_root_name_;
            } else {
                name = default_inner_name_;
            }
        }

        // Handle underscores/spaces.
        if( replace_name_underscores_ ) {
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
            edge.data<DefaultEdgeData>().branch_length = default_branch_length_;
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

private:

    double default_branch_length_ = 1.0;

    std::string default_leaf_name_  = "Leaf_Node";
    std::string default_inner_name_ = "Inner_Node";
    std::string default_root_name_  = "Root_Node";

    bool        use_default_names_        = false;
    bool        replace_name_underscores_ = false;

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
