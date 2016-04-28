#ifndef GENESIS_PLACEMENT_SIMULATOR_EDGE_DISTRIBUTION_H_
#define GENESIS_PLACEMENT_SIMULATOR_EDGE_DISTRIBUTION_H_

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
#include "utils/core/options.hpp"

#include <random>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator Edge Distribution
// =================================================================================================

class SimulatorEdgeDistribution
{
public:

    // -------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------

    SimulatorEdgeDistribution()  = default;
    ~SimulatorEdgeDistribution() = default;

    SimulatorEdgeDistribution( SimulatorEdgeDistribution const& ) = default;
    SimulatorEdgeDistribution( SimulatorEdgeDistribution&& )      = default;

    SimulatorEdgeDistribution& operator= ( SimulatorEdgeDistribution const& ) = default;
    SimulatorEdgeDistribution& operator= ( SimulatorEdgeDistribution&& )      = default;

    // -------------------------------------------------
    //     Generate Random Edges
    // -------------------------------------------------

    /**
     * @brief Prepare the distribution for usage. Needs to be called before generate().
     */
    void prepare( Sample const& sample )
    {
        // If nothing was set, initialize to uniform distrib.
        if( edge_weights.size() == 0 ) {
            edge_weights = std::vector<double>( sample.tree().edge_count(), 1.0 );
        }

        // Check size.
        if( edge_weights.size() != sample.tree().edge_count() ) {
            throw std::runtime_error(
                "Incorrect number of edge weights for SimulatorEdgeDistribution."
            );
        }

        // Prepare the actual distrib.
        distrib_ = std::discrete_distribution<size_t>( edge_weights.begin(), edge_weights.end() );
    }

    /**
     * @brief Return a randomly chosen edge index, according to the distribution.
     */
    size_t generate()
    {
        return distrib_( utils::Options::get().random_engine() );
    }

    // -------------------------------------------------
    //     Data Members
    // -------------------------------------------------

public:

    std::vector<double> edge_weights;

private:

    std::discrete_distribution<size_t> distrib_;
};

} // namespace placement
} // namespace genesis

#endif // include guard
