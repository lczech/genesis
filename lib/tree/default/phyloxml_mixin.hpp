#ifndef GENESIS_TREE_DEFAULT_PHYLOXML_MIXIN_H_
#define GENESIS_TREE_DEFAULT_PHYLOXML_MIXIN_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/io/xml_document.hpp"

namespace genesis {

// =================================================================================================
//     Default Tree Phyloxml Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class DefaultTreePhyloxmlMixin : public Base
{
protected:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    typedef typename Base::TreeType TreeType;
    typedef typename Base::NodeType NodeType;
    typedef typename Base::EdgeType EdgeType;
    typedef typename Base::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

    virtual void tree_node_to_element( NodeType const& node, XmlElement& element )
    {
        Base::tree_node_to_element(node, element);
        set_name(element, node.data.name);
    }

    virtual void tree_edge_to_element( EdgeType const& edge, XmlElement& element )
    {
        Base::tree_edge_to_element(edge, element);
        set_branch_length(element, edge.data.branch_length);
    }

    // -------------------------------------------------------------------------
    //     Mixin Functions
    // -------------------------------------------------------------------------

    void set_name (XmlElement& element, const std::string& name)
    {
        auto name_e = make_unique<XmlElement>("name");
        name_e->append_markup(name);
        element.content.push_back(std::move(name_e));
    }

    void set_branch_length (XmlElement& element, double length)
    {
        auto bl_e = make_unique<XmlElement>("branch_length");
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

} // namespace genesis

#endif // include guard
