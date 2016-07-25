#ifndef GENESIS_PLACEMENT_SIMULATOR_EXTRA_PLACEMENT_DISTRIBUTION_H_
#define GENESIS_PLACEMENT_SIMULATOR_EXTRA_PLACEMENT_DISTRIBUTION_H_

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

#include <random>
#include <string>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator Extra Placement Distribution
// =================================================================================================

/**
 * @brief Generate a certain number of additional PqueryPlacement%s around a given
 * ::PlacementTreeEdge.
 *
 * This class models both the distribution of the number of generated placements and their
 * distribution around the given edge. It is easier to model it this way (both distributions in one
 * class), because they have a dependency: Once the number of placements is drawn, they have to be
 * distributed along the edges of the tree without repetition according to a distribution of
 * distances (path lengths) from the central edge. Doing those two steps in one class is easier.
 */
class SimulatorExtraPlacementDistribution
{
public:

    // -----------------------------------------------------
    //     Typedefs and Structs
    // -----------------------------------------------------

    struct EdgeProximity
    {
        using EdgeCandidates = std::vector<size_t>;
        std::vector<EdgeCandidates> candidates_per_level;
        size_t total_candidates = 0;
    };

    // -----------------------------------------------------
    //     Constructor and Rule of Five
    // -----------------------------------------------------

    SimulatorExtraPlacementDistribution()  = default;
    ~SimulatorExtraPlacementDistribution() = default;

    SimulatorExtraPlacementDistribution( SimulatorExtraPlacementDistribution const& ) = default;
    SimulatorExtraPlacementDistribution( SimulatorExtraPlacementDistribution&& )      = default;

    SimulatorExtraPlacementDistribution& operator= ( SimulatorExtraPlacementDistribution const& ) = default;
    SimulatorExtraPlacementDistribution& operator= ( SimulatorExtraPlacementDistribution&& )      = default;

    // -----------------------------------------------------
    //     Generate Random Positions
    // -----------------------------------------------------

    void prepare( Sample const& sample );
    std::vector<size_t> generate( PlacementTreeEdge const& edge );

    std::string         dump_edge_proximities() const;
    std::vector<size_t> edge_proximity_maxima() const;

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    std::vector<double> placement_number_weights;
    std::vector<double> placement_path_length_weights;

private:

    std::discrete_distribution<size_t> placement_number_distrib_;
    std::discrete_distribution<size_t> placement_path_length_distrib_;

    std::vector<EdgeProximity> edge_proximities_;
};

} // namespace placement
} // namespace genesis

#endif // include guard
