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
#include <iosfwd>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sample;

class SimulatorEdgeDistribution;
class SimulatorExtraPlacementDistribution;
class SimulatorLikeWeightRatioDistribution;
class SimulatorPendantLengthDistribution;
class SimulatorProximalLengthDistribution;

// =================================================================================================
//     Stream Output Operators
// =================================================================================================

std::ostream& operator <<( std::ostream& out, SimulatorEdgeDistribution const&            distrib );
std::ostream& operator <<( std::ostream& out, SimulatorExtraPlacementDistribution const&  distrib );
std::ostream& operator <<( std::ostream& out, SimulatorLikeWeightRatioDistribution const& distrib );

// =================================================================================================
//     Edge Distribution
// =================================================================================================

void set_uniform_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib );
void set_uniform_weights( size_t edge_count,    SimulatorEdgeDistribution& edge_distrib );

void set_random_weights(  Sample const& sample, SimulatorEdgeDistribution& edge_distrib );
void set_random_weights(  size_t edge_count,    SimulatorEdgeDistribution& edge_distrib );

void set_random_edges(    Sample const& sample, SimulatorEdgeDistribution& edge_distrib );
void set_random_edges(    size_t edge_count,    SimulatorEdgeDistribution& edge_distrib );

void set_depths_distributed_weights(
    Sample const& sample,
    SimulatorEdgeDistribution& edge_distrib
);

void set_depths_distributed_weights(
    Sample const&              sample,
    std::vector<double> const& depth_weights,
    SimulatorEdgeDistribution& edge_distrib
);

size_t set_random_subtree_weights(
    Sample const& sample,
    SimulatorEdgeDistribution& edge_distrib
);

void set_subtree_weights(
    Sample const& sample,
    size_t link_index,
    SimulatorEdgeDistribution& edge_distrib
);

void learn_per_edge_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib );

// =================================================================================================
//     Extra Placement Number Distribution
// =================================================================================================

void learn_placement_number_weights(
    Sample const& sample,
    SimulatorExtraPlacementDistribution& p_distib
);

void learn_placement_path_length_weights(
    Sample const& sample,
    SimulatorExtraPlacementDistribution& p_distib
);

// =================================================================================================
//     Like Weight Ratio Distribution
// =================================================================================================

void learn_like_weight_ratio_distribution(
    Sample const& sample,
    SimulatorLikeWeightRatioDistribution& lwr_distib,
    size_t number_of_intervals = 10
);

} // namespace placement
} // namespace genesis

#endif // include guard
