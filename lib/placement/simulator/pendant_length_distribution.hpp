#ifndef GENESIS_PLACEMENT_SIMULATOR_PENDANT_LENGTH_DISTRIBUTION_H_
#define GENESIS_PLACEMENT_SIMULATOR_PENDANT_LENGTH_DISTRIBUTION_H_

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

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator Pendant Length Distribution
// =================================================================================================

class SimulatorPendantLengthDistribution
{
public:

    // -------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------

    SimulatorPendantLengthDistribution()  = default;
    ~SimulatorPendantLengthDistribution() = default;

    SimulatorPendantLengthDistribution( SimulatorPendantLengthDistribution const& ) = default;
    SimulatorPendantLengthDistribution( SimulatorPendantLengthDistribution&& )      = default;

    SimulatorPendantLengthDistribution& operator= ( SimulatorPendantLengthDistribution const& ) = default;
    SimulatorPendantLengthDistribution& operator= ( SimulatorPendantLengthDistribution&& )      = default;

    // -----------------------------------------------------
    //     Generate Random Length
    // -----------------------------------------------------

    /**
     * @brief Prepare the distribution for usage. Needs to be called before generate().
     */
    void prepare( Sample const& sample )
    {
        (void) sample;
        distrib_ = std::uniform_real_distribution<double>(min, max);
    }

    /**
     * @brief Return a randomly chosen position on an edge.
     */
    double generate( PlacementTreeEdge const& edge )
    {
        // We don't use the edge in the default distribution.
        (void) edge;

        return distrib_( utils::Options::get().random_engine() );
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    double min = 0.0;
    double max = 1.0;

private:

    std::uniform_real_distribution<double> distrib_;
};

} // namespace placement
} // namespace genesis

#endif // include guard
