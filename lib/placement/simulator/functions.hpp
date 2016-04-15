#ifndef GENESIS_PLACEMENT_SIMULATOR_FUNCTIONS_H_
#define GENESIS_PLACEMENT_SIMULATOR_FUNCTIONS_H_

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

#include <cstddef>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class SimulatorEdgeDistribution;
class SimulatorPlacementDistribution;
class Sample;

// =================================================================================================
//     Edge Distribution
// =================================================================================================

void set_uniform_weights(            Sample const& sample, SimulatorEdgeDistribution& edge_distrib );
void set_uniform_weights(            size_t edge_count,    SimulatorEdgeDistribution& edge_distrib );

void set_random_weights(             Sample const& sample, SimulatorEdgeDistribution& edge_distrib );
void set_random_weights(             size_t edge_count,    SimulatorEdgeDistribution& edge_distrib );

void set_random_edges(               Sample const& sample, SimulatorEdgeDistribution& edge_distrib );
void set_random_edges(               size_t edge_count,    SimulatorEdgeDistribution& edge_distrib );

void set_depths_distributed_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib );
void set_depths_distributed_weights(
    Sample const&              sample,
    std::vector<double> const& depth_weights,
    SimulatorEdgeDistribution& edge_distrib
);

void set_random_subtree_weights(     Sample const& sample, SimulatorEdgeDistribution& edge_distrib );
void learn_per_edge_weights(           Sample const& sample, SimulatorEdgeDistribution& edge_distrib );

// =================================================================================================
//     Placement Number Distribution
// =================================================================================================

void learn_placement_number_weights(      Sample const& sample, SimulatorPlacementDistribution& p_distib );
void learn_placement_path_length_weights( Sample const& sample, SimulatorPlacementDistribution& p_distib );

} // namespace placement
} // namespace genesis

#endif // include guard
