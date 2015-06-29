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
class PlacementTreePhyloxmlAdapter : public DefaultTreePhyloxmlAdapter
{
public:

    template <class PreorderIteratorType>
    inline void populate_clade(XmlElement* clade, PreorderIteratorType& it)
    {
        DefaultTreePhyloxmlAdapter::populate_clade(clade, it);

        set_color(clade, 0, 100, 120);
    }

};

} // namespace genesis

#endif // include guard
