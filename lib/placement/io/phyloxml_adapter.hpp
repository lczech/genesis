#ifndef GENESIS_PLACEMENT_IO_PHYLOXML_ADAPTER_H_
#define GENESIS_PLACEMENT_IO_PHYLOXML_ADAPTER_H_

/**
 * @brief Header of PlacementTreePhyloxmlAdapter class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"
#include "placement/placement_tree.hpp"
#include "tree/default_tree_phyloxml_adapter.hpp"
#include "utils/tools/color.hpp"
#include "utils/tools/color/gradient.hpp"

namespace genesis {

// =================================================================================================
//     Placement Tree Phyloxml Adapter
// =================================================================================================

/**
 * @brief
 */
class PlacementTreePhyloxmlAdapter : public DefaultTreePhyloxmlAdapter<PlacementTree>
{
public:

    PlacementTreePhyloxmlAdapter () {};

    PlacementTreePhyloxmlAdapter (const PlacementMap& map)
    {
        max_placements_per_edge = map.placement_count_max_edge().second;
    }

    inline void from_tree(PlacementTree::ConstIteratorPreorder& it, XmlElement* clade)
    {
        DefaultTreePhyloxmlAdapter<PlacementTree>::from_tree(it, clade);
        // set_name(clade, it.node()->name);
        // set_branch_length(clade, it.edge()->branch_length);

        Color edge_color (128,128,128);
        if (max_placements_per_edge > 0 && it.edge()->data.placements.size() > 0) {
            edge_color = heat_gradient(
                log(it.edge()->data.placements.size()) / log(max_placements_per_edge)
            );
        }

        // LOG_DBG <<  it.edge()->placements.size() << " --> " << 100 * it.edge()->placements.size() / max_placements_per_edge << " = " << edge_color.dump();

        set_color(edge_color, clade);
    }

    size_t max_placements_per_edge = 0;

};

typedef PhyloxmlProcessor<PlacementTreePhyloxmlAdapter> PlacementTreePhyloxmlProcessor;

} // namespace genesis

#endif // include guard
