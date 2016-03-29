/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

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
