#ifndef GENESIS_PLACEMENT_PLACEMENT_TREE_PHYLOXML_ADAPTER_H_
#define GENESIS_PLACEMENT_PLACEMENT_TREE_PHYLOXML_ADAPTER_H_

/**
 * @brief Header of PlacementTreePhyloxmlAdapter class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"
#include "placement/placement_tree.hpp"
#include "tree/default_tree_phyloxml_adapter.hpp"
#include "utils/color.hpp"

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

    PlacementTreePhyloxmlAdapter () {};

    PlacementTreePhyloxmlAdapter (const PlacementMap& map)
    {
        max_placements_per_edge = map.placement_count_max_edge().second;
    }

    template <class PreorderIteratorType>
    inline void populate_clade(XmlElement* clade, PreorderIteratorType& it)
    {
        set_name(clade, it.node()->name);
        set_branch_length(clade, it.edge()->branch_length);

        Color edge_color (128,128,128);
        if (it.edge()->placements.size() > 0) {
            edge_color = Color::heat_gradient(
                log(it.edge()->placements.size()) / log(max_placements_per_edge)
            );
        }

        // LOG_DBG <<  it.edge()->placements.size() << " --> " << 100 * it.edge()->placements.size() / max_placements_per_edge << " = " << edge_color.dump();

        set_color(clade, edge_color);
    }

    size_t max_placements_per_edge = 0;

};

} // namespace genesis

#endif // include guard
