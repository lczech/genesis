#ifndef GENESIS_TREE_DEFAULT_PHYLOXML_WRITER_H_
#define GENESIS_TREE_DEFAULT_PHYLOXML_WRITER_H_

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
#include "tree/formats/phyloxml/writer.hpp"
#include "utils/formats/xml/document.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Phyloxml Writer Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class DefaultTreePhyloxmlWriterMixin : public Base
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void node_to_element( TreeNode const& node, utils::XmlElement& element ) override
    {
        Base::node_to_element(node, element);
        set_name(element, node.data<DefaultNodeData>().name);
    }

    virtual void edge_to_element( TreeEdge const& edge, utils::XmlElement& element ) override
    {
        Base::edge_to_element(edge, element);
        set_branch_length(element, edge.data<DefaultEdgeData>().branch_length);
    }

    // -------------------------------------------------------------------------
    //     Mixin Functions
    // -------------------------------------------------------------------------

protected:

    void set_name( utils::XmlElement& element, const std::string& name )
    {
        // TODO do not create new element if there is already one!
        auto name_e = utils::make_unique< utils::XmlElement >( "name" );
        name_e->append_markup(name);
        element.content.push_back(std::move(name_e));
    }

    void set_branch_length( utils::XmlElement& element, double length )
    {
        // TODO do not create new element if there is already one!
        auto bl_e = utils::make_unique< utils::XmlElement >( "branch_length" );
        bl_e->append_markup(std::to_string(length));
        element.content.push_back(std::move(bl_e));
    }

    // TODO make a base class for processing, that takes care of issues like default names etc
    //
    // void from_tree(typename TreeType::ConstIteratorPreorder& it, XmlElement* clade)
    // {
        //~ it.node()->to_newick_broker_element(bn);
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        //~ if (!it.is_last_iteration()) {
            //~ it.edge()->to_newick_broker_element(bn);
        //~ }

        // filter out default names if needed
        //~ if (!use_default_names && bn->name != "" && (
            //~ bn->name == default_leaf_name ||
            //~ bn->name == default_internal_name ||
            //~ bn->name == default_root_name
        //~ )) {
            //~ bn->name = "";
        //~ }
    // }

};

// =================================================================================================
//     Default Tree Phyloxml Writer
// =================================================================================================

typedef DefaultTreePhyloxmlWriterMixin<PhyloxmlWriter> DefaultTreePhyloxmlWriter;

} // namespace tree
} // namespace genesis

#endif // include guard
