/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/tools/color.hpp"
#include "utils/tools/color/gradient.hpp"

#include <assert.h>
#include <algorithm>
#include <limits>

namespace genesis {

// =================================================================================================
//     Edge Color Functions
// =================================================================================================

/**
 * @brief
 */
template<class TreeType>
std::vector<color::Color> edge_color_branch_length_gradient( TreeType const& tree, bool zero_based )
{
    // Init the result vector with the min head color (green) for each edge.
    auto ret = std::vector<color::Color>( tree.edge_count(), color::heat_gradient(0.0) );
    if (tree.edge_count() == 0) {
        // If we are here, this is an empty vector.
        return ret;
    }

    // Perpare min and max.
    double min_bl = std::numeric_limits<double>::max();
    double max_bl = std::numeric_limits<double>::lowest();
    if (zero_based) {
        min_bl = 0.0;
    }

    // Find min and max branch lengths.
    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it) {
        auto const& edge = **it;
        min_bl = std::min(min_bl, edge.data.branch_length);
        max_bl = std::max(max_bl, edge.data.branch_length);
    }
    assert( min_bl <= max_bl );
    double dist = max_bl - min_bl;

    // If all branch lengths are the same, return the vector as it is (initialized to green).
    if (dist == 0.0) {;
        return ret;
    }

    // Calculate the heat gradient color based on the branch length for each edge.
    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it) {
        auto const& edge = **it;
        double val = (edge.data.branch_length - min_bl) / dist;
        ret[edge.index()] = color::heat_gradient(val);
    }
    return ret;
}

} // namespace genesis
