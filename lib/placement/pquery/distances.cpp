/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

// #include <algorithm>
// #include <assert.h>
// #include <string>
// #include <vector>
// #include <unordered_map>
// #include <unordered_set>

#include "placement/pquery/distances.hpp"
// #include "utils/utils.hpp"

namespace genesis {
namespace placement {

/*
// =================================================================================================
//     Distance Measuring
// =================================================================================================

double distance (
    const PqueryPlacement& placement,
    const PlacementTreeNode& node,
    const Matrix<double>& node_distance_matrix
) {
    double pp, pd, dp;

    // proximal-proximal case
    pp = placement.proximal_length
       + node_distance_matrix(placement.edge->primary_node()->index(), node.index());

    // proximal-distal case
    pd = placement.proximal_length
       + node_distance_matrix(placement.edge->primary_node()->index(), node.index());

    // distal-proximal case
    dp = placement.edge->data.branch_length - placement.proximal_length
       + node_distance_matrix(placement.edge->secondary_node()->index(), node.index());

    // find min of the three cases
    double dist = std::min(pp, std::min(pd, dp));

    //  If needed, use pendant length.
    if (with_pendant_length) {
        dist += placement.pendant_length;
    }

    return dist;
}

double distance (
    const Pquery& pquery,
    const PlacementTreeNode& node,
    const Matrix<double>& node_distance_matrix
) {
    double sum = 0.0;

    // TODO good candidate for a for_all algorithm or sum, or reduce, or whatever the name is.
    for (const auto& place : pquery) {
        sum += distance(place, node, node_distance_matrix) * place.like_weight_ratio;
    }

    return sum;
}
*/

} // namespace placement
} // namespace genesis
