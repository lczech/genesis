#ifndef GENESIS_PLACEMENT_PLACEMENT_TREE_PHYLOXML_ADAPTER_H_
#define GENESIS_PLACEMENT_PLACEMENT_TREE_PHYLOXML_ADAPTER_H_

/**
 * @brief Header of PlacementTreePhyloxmlAdapter class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"
#include "tree/default_tree_phyloxml_adapter.hpp"

namespace genesis {

// =================================================================================================
//     PlacementTreePhyloxmlAdapter
// =================================================================================================

/**
 * @brief
 */
class PlacementTreePhyloxmlAdapter : public PhyloxmlAdapter //DefaultTreePhyloxmlAdapter
{
public:

    template <class PreorderIteratorType>
    inline void populate_clade(XmlElement* clade, PreorderIteratorType& it)
    {
        set_name(clade, it.node()->name);
        set_branch_length(clade, it.edge()->branch_length);

        set_color(clade, 0, 100, 120);
        LOG_DBG << it.edge()->placements.size();
    }

};

} // namespace genesis

#endif // include guard
