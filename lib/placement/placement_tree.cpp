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
 * @brief Implementation of PlacementTree class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"

#include <map>

#include "placement/sample.hpp"
#include "utils/core/logging.hpp"

namespace genesis {
namespace placement {

// =============================================================================
//     PlacementTreeEdgeData
// =============================================================================

/**
 * @brief Return the `edge_num` of this edge. This value is defined by the `jplace` standard.
 */
int PlacementTreeEdgeData::edge_num() const
{
    return edge_num_;
}

/**
 * @brief Force to set the edge_num to a certain value.
 *
 * The edge_num is usually considered to be constant for the tree. It rarely needs to be set,
 * except for when constructing the object. Use with care.
 */
void PlacementTreeEdgeData::reset_edge_num( int val )
{
    edge_num_ = val;
}

// /**
//  * @brief Destructor.
//  */
// PlacementTreeEdgeData::~PlacementTreeEdgeData()
// {
//     for( auto& place : placements ) {
//         place->edge = nullptr;
//     }
// }
//
// /**
//  * @brief Returns the number of placements on this edge.
//  */
// size_t PlacementTreeEdgeData::placement_count() const
// {
//     return placements.size();
// }
//
// /**
//  * @brief Returns the mass of the placements on this edge, as given by their `like_weight_ratio`.
//  */
// double PlacementTreeEdgeData::placement_mass() const
// {
//     double mass = 0.0;
//     for (PqueryPlacement* pl : placements) {
//         mass += pl->like_weight_ratio;
//     }
//     return mass;
// }
//
// /**
//  * @brief Sorts the placements on this edge by their distance from the root, ascending.
//  */
// void PlacementTreeEdgeData::sort_placements()
// {
//     std::multimap<double, PqueryPlacement*> sorted;
//     std::vector<PqueryPlacement*> new_placements;
//
//     for (PqueryPlacement* place : placements) {
//         sorted.emplace(place->proximal_length, place);
//     }
//     for (auto it = sorted.begin(); it != sorted.end(); ++it) {
//         new_placements.push_back(it->second);
//     }
//
//     placements.swap(new_placements);
// }

} // namespace placement
} // namespace genesis
