#ifndef GENESIS_TREE_DEFAULT_NEWICK_WRITER_H_
#define GENESIS_TREE_DEFAULT_NEWICK_WRITER_H_

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
#include "genesis/tree/formats/newick/writer.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Newick Writer Plugin
// =================================================================================================

/**
 * @brief Provide a set of plugin functions for NewickWriter to write a #DefaultTree.
 */
class DefaultTreeNewickWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = DefaultTreeNewickWriterPlugin;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    DefaultTreeNewickWriterPlugin() = default;
    virtual ~DefaultTreeNewickWriterPlugin() = default;

    DefaultTreeNewickWriterPlugin(DefaultTreeNewickWriterPlugin const&) = default;
    DefaultTreeNewickWriterPlugin(DefaultTreeNewickWriterPlugin&&)      = default;

    DefaultTreeNewickWriterPlugin& operator= (DefaultTreeNewickWriterPlugin const&) = default;
    DefaultTreeNewickWriterPlugin& operator= (DefaultTreeNewickWriterPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Get the named used to filter out a leaf node name.
     */
    std::string const& default_leaf_name() const
    {
        return default_leaf_name_;
    }

    /**
     * @brief Set the named used to filter out a leaf node name.
     */
    self_type& default_leaf_name( std::string const& value )
    {
        default_leaf_name_ = value;
        return *this;
    }

    /**
     * @brief Get the named used to filter out an inner node name.
     */
    std::string const& default_inner_name() const
    {
        return default_inner_name_;
    }

    /**
     * @brief Set the named used to filter out an inner node name.
     */
    self_type& default_inner_name( std::string const& value )
    {
        default_inner_name_ = value;
        return *this;
    }

    /**
     * @brief Get the named used to filter out the root node name.
     */
    std::string const& default_root_name() const
    {
        return default_root_name_;
    }

    /**
     * @brief Set the named used to filter out the root node name.
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
     * @brief Set whether to replace default named nodes with an empty string.
     *
     * This setting activates the "reverse" operation of
     * DefaultTreeNewickReaderPlugin::use_default_names( bool ).
     * Thus, when the default names are set to the same values as in the reader plugin, reading a
     * Newick tree and then writing it again should yield the same names in the Newick tree again.
     *
     * Default is `false`. In this case, all node names are written to the Newick tree, indepentenly
     * of whether they match the default names.
     *
     * If set to `true`, a node that has one of the default names will result in an empty node
     * name in the Newick tree:
     *
     *  * Leaf nodes with a name equal to default_leaf_name().
     *  * Inner nodes with a name equal to default_inner_name().
     *  * The root node with a name equal to default_root_name().
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
     * @brief Return whether currently this plugin replaces spaces with underscores.
     *
     * See the setter replace_name_spaces( bool ) for details.
     */
    bool replace_name_spaces() const
    {
        return replace_name_spaces_;
    }

    /**
     * @brief Set whether to replace all spaces (' ') in names with underscores ('_').
     *
     * This is the reverse of DefaultTreeNewickReaderPlugin::replace_name_underscores().
     * It is activated by default, as it does no harm on already existing underscores.
     * However, as spaces cannot be part of names in Newick, if it is deactivated (set to `false`),
     * all names that contain spaces are instead wrapped in quotation marks by the NewickWriter,
     * see NewickWriter::quotation_marks( std::string const& ) for details.
     */
    self_type& replace_name_spaces( bool value )
    {
        replace_name_spaces_ = value;
        return *this;
    }

    /**
     * @brief Set whether to write node names at all.
     *
     * If disabled, no names are written for any node.
     */
    self_type& enable_names( bool value )
    {
        enable_names_ = value;
        return *this;
    }

    /**
     * @brief Get whether currently any node names are written at all.
     */
    bool enable_names() const
    {
        return enable_names_;
    }

    /**
     * @brief Set whether to write branch lengths.
     */
    self_type& enable_branch_lengths( bool value )
    {
        enable_branch_lengths_ = value;
        return *this;
    }

    /**
     * @brief Get whether currently any branch lengths are written.
     */
    bool enable_branch_lengths() const
    {
        return enable_branch_lengths_;
    }

    /**
     * @brief Get the currently set maximum precision (in number of digits) used for printing the
     * `branch_length` floating point numbers.
     */
    int branch_length_precision() const
    {
        return branch_length_precision_;
    }

    /**
     * @brief Set the maximum precision (in number of digits) used for printing the `branch_length`
     * floating point numbers.
     *
     * Default is 6.
     */
    self_type& branch_length_precision( int value )
    {
        branch_length_precision_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void node_to_element( TreeNode const& node, NewickBrokerElement& element ) const
    {
        if (enable_names_) {
            std::string name = node.data<DefaultNodeData>().name;

            // Handle spaces/underscores.
            if( replace_name_spaces_ ) {
                name = utils::replace_all(name, " ", "_");
            }

            // Filter out default names if needed.
            if( use_default_names_ && (
                ( node.is_leaf()  && name == default_leaf_name_  ) ||
                ( node.is_inner() && name == default_inner_name_ ) ||
                ( node.is_root()  && name == default_root_name_  )
            )) {
                name = "";
            }

            element.name = name;
        }
    }

    void edge_to_element( TreeEdge const& edge, NewickBrokerElement& element ) const
    {
        if (enable_branch_lengths_) {
            auto const& edge_data = edge.data<DefaultEdgeData>();
            auto bl = utils::to_string_rounded( edge_data.branch_length, branch_length_precision_ );
            element.values.insert (element.values.begin(), bl );
        }
    }

    void register_with( NewickWriter& writer ) const
    {
        // Add node functions.
        writer.node_to_element_plugins.push_back(
            [&]( TreeNode const& node, NewickBrokerElement& element ) {
                node_to_element( node, element );
            }
        );

        // Add edge functions.
        writer.edge_to_element_plugins.push_back(
            [&]( TreeEdge const& edge, NewickBrokerElement& element ) {
                edge_to_element( edge, element );
            }
        );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    int branch_length_precision_ = 6;

    std::string default_leaf_name_  = "Leaf_Node";
    std::string default_inner_name_ = "Inner_Node";
    std::string default_root_name_  = "Root_Node";

    bool        use_default_names_   = false;
    bool        replace_name_spaces_ = true;

    bool enable_names_          = true;
    bool enable_branch_lengths_ = true;

};

// =================================================================================================
//     Default Tree Newick Writer
// =================================================================================================

/**
 * @brief Write default Newick trees, i.e., trees with names and branch lengths.
 *
 * This class is a convenience wrapper that combines a NewickWriter with a
 * DefaultTreeNewickWriterPlugin. It is intended to be used for standard use cases, and writes a
 * Newick tree from a Tree with DefaultNodeData and DefaultEdgeData at its nodes and edges.
 *
 * It is also possible to register additional plugins on top of this class.
 *
 * Behind the curtain, this class derives from both NewickWriter and DefaultTreeNewickWriterPlugin.
 * This is a bit ugly, but we use it for simplicity. This allows to use an instance as if it was
 * a writer (i.e., call `to_...` functions), but also change the plugin settings in a natural
 * way.
 */
class DefaultTreeNewickWriter
    : public NewickWriter
    , public DefaultTreeNewickWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    DefaultTreeNewickWriter()
    {
        register_with( *this );
    }
};

} // namespace tree
} // namespace genesis

#endif // include guard
