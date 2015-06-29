#ifndef GENESIS_TREE_DEFAULT_TREE_PHYLOXML_ADAPTER_H_
#define GENESIS_TREE_DEFAULT_TREE_PHYLOXML_ADAPTER_H_

/**
 * @brief Header of DefaultTreePhyloxmlAdapter class.
 *
 * @file
 * @ingroup tree
 */

#include "tree/default_tree.hpp"
#include "tree/phyloxml_adapter.hpp"
#include "utils/utils.hpp"
#include "utils/xml_document.hpp"

namespace genesis {

// =================================================================================================
//     DefaultTreePhyloxmlAdapter
// =================================================================================================

/**
 * @brief
 */
class DefaultTreePhyloxmlAdapter : public PhyloxmlAdapter
{
public:

    template <class PreorderIteratorType>
    inline void populate_clade(XmlElement* clade, PreorderIteratorType& it)
    {
        // Create name entry.
        auto name_e = make_unique<XmlElement>("name");
        name_e->append_markup(it.node()->name);
        clade->content.push_back(std::move(name_e));

        // Create branch length entry.
        auto bl_e = make_unique<XmlElement>("branch_length");
        bl_e->append_markup(std::to_string(it.edge()->branch_length));
        clade->content.push_back(std::move(bl_e));

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
    }

};

} // namespace genesis

#endif // include guard
