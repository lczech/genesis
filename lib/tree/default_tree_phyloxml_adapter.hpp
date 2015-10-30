#ifndef GENESIS_TREE_DEFAULT_TREE_PHYLOXML_ADAPTER_H_
#define GENESIS_TREE_DEFAULT_TREE_PHYLOXML_ADAPTER_H_

/**
 * @brief Header of DefaultTreePhyloxmlAdapter class.
 *
 * @file
 * @ingroup tree
 */

#include "tree/default_tree.hpp"
#include "tree/io/phyloxml_adapter.hpp"
#include "utils/io/xml_document.hpp"

namespace genesis {

// =================================================================================================
//     Default Tree Phyloxml Adapter
// =================================================================================================

/**
 * @brief
 */
template <typename TreeType>
class DefaultTreePhyloxmlAdapter : public PhyloxmlAdapter<TreeType>
{
public:

    inline void from_tree(typename TreeType::ConstIteratorPreorder& it, XmlElement* clade)
    {
        PhyloxmlAdapter<TreeType>::set_name(it.node()->data.name, clade);
        PhyloxmlAdapter<TreeType>::set_branch_length(it.edge()->data.branch_length, clade);

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

// =================================================================================================
//     Typedef
// =================================================================================================

typedef PhyloxmlProcessor<DefaultTreePhyloxmlAdapter<DefaultTree>> DefaultPhyloxmlProcessor;

} // namespace genesis

#endif // include guard
