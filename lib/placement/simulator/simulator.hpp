#ifndef GENESIS_PLACEMENT_SIMULATOR_SIMULATOR_H_
#define GENESIS_PLACEMENT_SIMULATOR_SIMULATOR_H_

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

#include "placement/simulator/edge_distribution.hpp"
#include "placement/simulator/extra_placement_distribution.hpp"
#include "placement/simulator/like_weight_ratio_distribution.hpp"
#include "placement/simulator/pendant_length_distribution.hpp"
#include "placement/simulator/proximal_length_distribution.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator
// =================================================================================================

/**
 * @brief Simulate @link Pquery Pqueries @endlink on the Tree of a Sample.
 */
class Simulator
{
public:

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    Simulator()  = default;
    ~Simulator() = default;

    Simulator( Simulator const& ) = default;
    Simulator( Simulator&& )      = default;

    Simulator& operator= ( Simulator const& ) = default;
    Simulator& operator= ( Simulator&& )      = default;

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    void generate( Sample& sample, size_t n );

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    SimulatorEdgeDistribution& edge_distribution()
    {
        return edge_distribution_;
    }

    SimulatorExtraPlacementDistribution& extra_placement_distribution()
    {
        return extra_placement_distribution_;
    }

    SimulatorLikeWeightRatioDistribution& like_weight_ratio_distribution()
    {
        return like_weight_ratio_distribution_;
    }

    SimulatorProximalLengthDistribution& proximal_length_distribution()
    {
        return proximal_length_distribution_;
    }

    SimulatorPendantLengthDistribution& pendant_length_distribution()
    {
        return pendant_length_distribution_;
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    SimulatorEdgeDistribution            edge_distribution_;
    SimulatorExtraPlacementDistribution  extra_placement_distribution_;
    SimulatorLikeWeightRatioDistribution like_weight_ratio_distribution_;
    SimulatorProximalLengthDistribution  proximal_length_distribution_;
    SimulatorPendantLengthDistribution   pendant_length_distribution_;

};

} // namespace placement
} // namespace genesis

#endif // include guard
