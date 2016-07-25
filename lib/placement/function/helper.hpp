#ifndef GENESIS_PLACEMENT_FUNCTION_HELPER_H_
#define GENESIS_PLACEMENT_FUNCTION_HELPER_H_

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

#include "placement/sample.hpp"

#include <unordered_map>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

struct PqueryPlain;

// =================================================================================================
//     Helper Functions
// =================================================================================================

std::unordered_map<int, PlacementTreeEdge*> edge_num_to_edge_map( PlacementTree const& tree );
std::unordered_map<int, PlacementTreeEdge*> edge_num_to_edge_map( Sample const & smp );

std::unordered_map< size_t, std::vector< PqueryPlacement const* >> placements_per_edge(
    Sample const& smp
);

std::vector<PqueryPlacement const*> placements_per_edge(
    Sample            const& smp,
    PlacementTreeEdge const& edge
);

std::vector<size_t> placement_count_per_edge( Sample const& sample );
std::vector<double> placement_weight_per_edge( Sample const& sample );

std::vector<PqueryPlain> plain_queries( Sample const & smp );

// =================================================================================================
//     Verification
// =================================================================================================

void reset_edge_nums( PlacementTree& tree );
bool has_correct_edge_nums( PlacementTree const& tree );

bool validate( Sample const& smp, bool check_values = false, bool break_on_values = false );

} // namespace placement
} // namespace genesis

#endif // include guard
