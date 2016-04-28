#ifndef GENESIS_PLACEMENT_SIMULATOR_LIKE_WEIGHT_RATIO_DISTRIBUTION_H_
#define GENESIS_PLACEMENT_SIMULATOR_LIKE_WEIGHT_RATIO_DISTRIBUTION_H_

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
#include <string>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator Like Weight Ratio Distribution
// =================================================================================================

class SimulatorLikeWeightRatioDistribution
{
public:

    // -------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------

    SimulatorLikeWeightRatioDistribution()  = default;
    ~SimulatorLikeWeightRatioDistribution() = default;

    SimulatorLikeWeightRatioDistribution( SimulatorLikeWeightRatioDistribution const& ) = default;
    SimulatorLikeWeightRatioDistribution( SimulatorLikeWeightRatioDistribution&& )      = default;

    SimulatorLikeWeightRatioDistribution& operator= ( SimulatorLikeWeightRatioDistribution const& ) = default;
    SimulatorLikeWeightRatioDistribution& operator= ( SimulatorLikeWeightRatioDistribution&& )      = default;

    // -----------------------------------------------------
    //     Generate Random Ratios
    // -----------------------------------------------------

    /**
     * @brief Prepare the distribution for usage. Needs to be called before generate().
     */
    void prepare( Sample const& sample );

    /**
     * @brief Return a randomly chosen like weight ratio.
     */
    double generate()
    {
        return distrib_( utils::Options::get().random_engine() );
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    std::vector<double> intervals;
    std::vector<double> weights;

private:

    std::piecewise_linear_distribution<double> distrib_;
};

} // namespace placement
} // namespace genesis

#endif // include guard
