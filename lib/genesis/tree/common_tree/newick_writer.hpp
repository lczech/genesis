#ifndef GENESIS_TREE_COMMON_TREE_NEWICK_WRITER_H_
#define GENESIS_TREE_COMMON_TREE_NEWICK_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/formats/newick/element.hpp"
#include "genesis/tree/formats/newick/writer.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Common Tree Newick Writer Plugin
// =================================================================================================

/**
 * @brief Provide a set of plugin functions for NewickWriter to write a #CommonTree.
 */
class CommonTreeNewickWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = CommonTreeNewickWriterPlugin;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    CommonTreeNewickWriterPlugin() = default;
    virtual ~CommonTreeNewickWriterPlugin() = default;

    CommonTreeNewickWriterPlugin(CommonTreeNewickWriterPlugin const&) = default;
    CommonTreeNewickWriterPlugin(CommonTreeNewickWriterPlugin&&)      = default;

    CommonTreeNewickWriterPlugin& operator= (CommonTreeNewickWriterPlugin const&) = default;
    CommonTreeNewickWriterPlugin& operator= (CommonTreeNewickWriterPlugin&&)      = default;

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
     * CommonTreeNewickReaderPlugin::use_default_names( bool ).
     * Thus, when the default names are set to the same values as in the reader plugin, reading a
     * Newick tree and then writing it again should yield the same names in the Newick tree again.
     *
     * Common is `false`. In this case, all node names are written to the Newick tree, indepentenly
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
     * @brief Return whether currently this plugin replaces characters that are invalid in the
     * Newick file format with underscores.
     *
     * See the setter replace_invalid_chars( bool ) for details.
     */
    bool replace_invalid_chars() const
    {
        return replace_invalid_chars_;
    }

    /**
     * @brief Set whether to replace all characters that are invalid in the
     * Newick file format in names with underscores ('_').
     *
     * This is the reverse of CommonTreeNewickReaderPlugin::replace_name_underscores(),
     * in that it replaces spaces with underscores,
     * but also handles other invalid characters `:;()[],={}"` that cannot be used in Newick labels,
     * see http://evolution.genetics.washington.edu/phylip/newicktree.html.
     * See also NewickWriter for similar options.
     *
     * It is activated by default, as this makes sure that other software can handle the names.
     * However, as these characters cannot be part of names in Newick, if it is deactivated
     * (set to `false`), all names that contain any of these characters are instead wrapped in
     * quotation marks by the NewickWriter, see NewickWriter::quotation_marks( std::string const& )
     * for details. This does not work well with many other tools which cannot read these quoted
     * names; we hence recommend to keept it at default, and replace all invalid characters.
     */
    self_type& replace_invalid_chars( bool value )
    {
        replace_invalid_chars_ = value;
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
     * Common is 6.
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
            std::string name = node.data<CommonNodeData>().name;

            // Handle spaces/underscores.
            if( replace_invalid_chars_ ) {
                static const std::string invalids( " :;()[],={}\"" );
                for( auto& c : name ) {
                    if( invalids.find(c) != std::string::npos ) {
                        c = '_';
                    }
                }
                // name = utils::replace_all(name, " ", "_");
            }

            // Filter out default names if needed.
            if( use_default_names_ && (
                ( is_leaf( node )  && name == default_leaf_name_  ) ||
                ( is_inner( node ) && name == default_inner_name_ ) ||
                ( is_root( node )  && name == default_root_name_  )
            )) {
                name = "";
            }

            element.name = name;
        }
    }

    void edge_to_element( TreeEdge const& edge, NewickBrokerElement& element ) const
    {
        if (enable_branch_lengths_) {
            auto const& edge_data = edge.data<CommonEdgeData>();
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

    bool        use_default_names_     = false;
    bool        replace_invalid_chars_ = true;

    bool enable_names_          = true;
    bool enable_branch_lengths_ = true;

};

// =================================================================================================
//     Common Tree Newick Writer
// =================================================================================================

/**
 * @brief Write default Newick trees, i.e., trees with names and branch lengths.
 *
 * This class is a convenience wrapper that combines a NewickWriter with a
 * CommonTreeNewickWriterPlugin. It is intended to be used for standard use cases, and writes a
 * Newick tree from a Tree with CommonNodeData and CommonEdgeData at its nodes and edges.
 *
 * It is also possible to register additional plugins on top of this class.
 *
 * Behind the curtain, this class derives from both NewickWriter and CommonTreeNewickWriterPlugin.
 * This is a bit ugly, but we use it for simplicity. This allows to use an instance as if it was
 * a writer (i.e., call `to_...` functions), but also change the plugin settings in a natural
 * way.
 */
class CommonTreeNewickWriter
    : public NewickWriter
    , public CommonTreeNewickWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    CommonTreeNewickWriter()
    {
        register_with( *this );
    }
};

} // namespace tree
} // namespace genesis

#endif // include guard
