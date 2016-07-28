#ifndef GENESIS_TREE_DEFAULT_NEWICK_WRITER_H_
#define GENESIS_TREE_DEFAULT_NEWICK_WRITER_H_

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

#include "tree/default/tree.hpp"
#include "tree/formats/newick/element.hpp"
#include "tree/formats/newick/writer.hpp"
#include "utils/core/std.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Newick Writer Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class DefaultTreeNewickWriterMixin : public Base
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

public:

    void enable_names( bool value )
    {
        enable_names_ = value;
    }

    bool enable_names ()
    {
        return enable_names_;
    }

    void enable_branch_lengths( bool value )
    {
        enable_branch_lengths_ = value;
    }

    bool enable_branch_lengths()
    {
        return enable_branch_lengths_;
    }

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void node_to_element( TreeNode const& node, NewickBrokerElement& element ) override
    {
        Base::node_to_element(node, element);

        if (enable_names_) {
            std::string name = node.data<DefaultNodeData>().name;

            // Handle spaces/underscores.
            if (replace_name_underscores) {
                name = utils::replace_all(name, " ", "_");
            } else {
                if (std::string::npos != name.find(" ")) {
                    name = "\"" + name + "\"";
                }
            }

            // Filter out default names if needed.
            if (use_default_names             && (
                name == default_leaf_name     ||
                name == default_internal_name ||
                name == default_root_name
            )) {
                name = "";
            }

            element.name = name;
        }
    }

    virtual void edge_to_element( TreeEdge const& edge, NewickBrokerElement& element ) override
    {
        Base::edge_to_element( edge, element );

        if (enable_branch_lengths_) {
            auto const& edge_data = edge.data<DefaultEdgeData>();
            auto bl = utils::to_string_rounded( edge_data.branch_length, branch_length_precision );
            element.values.insert (element.values.begin(), bl );
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    // TODO for now, this is all public. use getters and setters instead, and outsource those
    // properties that belong to the (yet to create) superclass DefaultNewickMixinBase or so.

    /**
     * @brief The precision used for printing the `branch_length` floating point numbers.
     */
    int branch_length_precision = 6;

    std::string default_leaf_name     = "Leaf_Node";
    std::string default_internal_name = "Internal_Node";
    std::string default_root_name     = "Root_Node";

    /**
     * @brief If set to true, nodes that are named using one of the default names are written
     * without names.
     *
     * This option effictively reverses the effect of the same option in the
     * DefaultTreeNewickReaderMixin. Thus, when both are set to the same value, the resulting
     * Newick file contains the same names.
     *
     * The default names can be set using #default_leaf_name, #default_internal_name and
     * #default_root_name. They are used both when parsing and printing a Newick file.
     */
    bool        use_default_names = false;

    bool        replace_name_underscores = true;

private:

    bool enable_names_          = true;
    bool enable_branch_lengths_ = false;

    // bool print_comments       = false;
    // bool print_tags           = false;
};

// =================================================================================================
//     Default Tree Newick Writer
// =================================================================================================

typedef DefaultTreeNewickWriterMixin<NewickWriter> DefaultTreeNewickWriter;

} // namespace tree
} // namespace genesis

#endif // include guard
