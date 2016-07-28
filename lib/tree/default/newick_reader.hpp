#ifndef GENESIS_TREE_DEFAULT_NEWICK_READER_H_
#define GENESIS_TREE_DEFAULT_NEWICK_READER_H_

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
#include "tree/formats/newick/reader.hpp"
#include "utils/core/std.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Newick Reader Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class DefaultTreeNewickReaderMixin : public Base
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void element_to_node( NewickBrokerElement const& element, TreeNode& node ) override
    {
        Base::element_to_node(element, node);

        std::string name = element.name;

        // Insert default names if needed.
        if (name.empty() && use_default_names) {
            if (element.is_leaf) {
                name = default_leaf_name;
            } else if(element.depth == 0) {
                name = default_root_name;
            } else {
                name = default_internal_name;
            }
        }

        // Handle underscores/spaces.
        if (replace_name_underscores) {
            name = utils::replace_all(name, "_", " ");
        }

        node.reset_data( DefaultNodeData::create() );
        node.data<DefaultNodeData>().name = name;
    }

    virtual void element_to_edge( NewickBrokerElement const& element, TreeEdge& edge ) override
    {
        Base::element_to_edge(element, edge);
        edge.reset_data( DefaultEdgeData::create() );

        // We assume that the branch length is always the first (or only) value.
        // If there is an interpretation where this is not the case, it is best to introduce
        // an array index for this as a paramter of this class.
        if (element.values.size() > 0) {
            edge.data<DefaultEdgeData>().branch_length = std::stod(element.values[0]);
        } else {
            edge.data<DefaultEdgeData>().branch_length = 1.0;
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    // TODO for now, this is all public. use getters and setters instead, and outsource those
    // properties that belong to the (yet to create) superclass DefaultNewickMixinBase or so.

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

typedef DefaultTreeNewickReaderMixin<NewickReader> DefaultTreeNewickReader;

} // namespace tree
} // namespace genesis

#endif // include guard
