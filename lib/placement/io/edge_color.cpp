/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/io/edge_color.hpp"

#include "placement/functions.hpp"
#include "placement/placement_tree.hpp"
#include "utils/tools/color.hpp"
#include "utils/tools/color/gradient.hpp"

namespace genesis {

// =================================================================================================
//     Placement Edge Color Functions
// =================================================================================================

/**
 * @brief Returns a vector with a Color for each edge that visualizes the number of placements on
 * that edge.
 *
 * The vector is indexed using the edge.index(). Each edge gets assigned a Color value with these
 * properties:
 *
 *   * Edges with no placements on them are grey (RGB 128, 128, 128).
 *   * Edges with placements get a color according to the relative number of placements compared to
 *     the other edges. The edge with most placements is pure red (RGB 255, 0, 0), while lower
 *     numbers of placements smoothly transition towards yellow and green edges.
 *
 * The gradient can be controlled via the `linear` parameter. If set to `true`, the scaling of the
 * color gradient is linar in the number of placements. If set to `false` (default), it is
 * logarithmic. This way, the color resolution is higher for low placement numbers, and compressed
 * for higher numbers. A typical distribution of placements yields only some edges with a very high
 * number of placements, while most of the other edges have little to no placements. Thus, it is
 * reasonable to emphasize the differences between those edges with a lower placement count - which
 * is what the default does.
 *
 * See color heat_gradient() for more information.
 */
std::vector<utils::Color> placement_color_count_gradient( PlacementTree const& tree, bool linear )
{
    // Init the result vector with grey color for each edge.
    auto ret = std::vector<utils::Color>( tree.edge_count(), utils::Color(128,128,128) );

    // Get the highest number of placements on any edge.
    // If this is zero, there are no placements, so we can immediately return.
    auto const max_placements_per_edge = placement_count_max_edge(tree).second;
    if (max_placements_per_edge == 0) {
        return ret;
    }

    // Calculate the heat gradient color based on the number of placements for each edge.
    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it) {
        auto const& edge = **it;
        if (edge.data.placements.size() > 0) {
            double val;
            if (linear) {
                val = edge.data.placements.size() / max_placements_per_edge;
            } else {
                val = log(edge.data.placements.size()) / log(max_placements_per_edge);
            }
            ret[edge.index()] = utils::heat_gradient(val);
        }

        // LOG_DBG <<  edge.placements.size() << " --> "
        //         << 100 * edge.placements.size() / max_placements_per_edge_
        //         << " = " << edge_color.dump();
    }

    return ret;
}

} // namespace genesis
