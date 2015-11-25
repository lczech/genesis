/**
 * @brief Implementation of Placement Tree Phyloxml Processor class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/io/phyloxml_processor.hpp"

#include "placement/functions.hpp"
#include "utils/tools/color.hpp"
#include "utils/tools/color/gradient.hpp"

namespace genesis {

// =================================================================================================
//     Placement Tree Phyloxml Processor
// =================================================================================================

void PlacementTreePhyloxmlProcessor::prepare_writing(
    TreeType const& tree,
    XmlDocument& xml
) {
    Base::prepare_writing(tree, xml);
    max_placements_per_edge_ = placement_count_max_edge(tree).second;
}

void PlacementTreePhyloxmlProcessor::tree_edge_to_element(
    EdgeType const& edge,
    XmlElement& element
) {
    Base::tree_edge_to_element(edge, element);

    Color edge_color (128,128,128);
    if (max_placements_per_edge_ > 0 && edge.data.placements.size() > 0) {
        edge_color = heat_gradient(
            log(edge.data.placements.size()) / log(max_placements_per_edge_)
        );
    }

    // LOG_DBG <<  edge.placements.size() << " --> "
    //         << 100 * edge.placements.size() / max_placements_per_edge_
    //         << " = " << edge_color.dump();

    set_color(element, edge_color);
}

} // namespace genesis
