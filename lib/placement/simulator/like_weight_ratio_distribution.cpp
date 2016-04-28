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

#include "placement/simulator/like_weight_ratio_distribution.hpp"

#include <algorithm>
#include <stdexcept>

namespace genesis {
namespace placement {

// =================================================================================================
//     Like Weight Ratio Distribution
// =================================================================================================

void SimulatorLikeWeightRatioDistribution::prepare( Sample const& sample )
{
    (void) sample;

    // Check condition.
    if( intervals.size() != weights.size() ) {
        throw std::logic_error(
            "The number of intervals and weights has to identical for "
            "SimulatorLikeWeightRatioDistribution."
        );
    }
    if( ! std::is_sorted( intervals.begin(), intervals.end() )) {
        throw std::logic_error(
            "Intervals need to be sorted in SimulatorLikeWeightRatioDistribution."
        );
    }
    if( std::any_of( weights.begin(), weights.end(), [] ( double v ) { return v < 0.0; } )) {
        throw std::logic_error(
            "Weights need to be non-negative in SimulatorLikeWeightRatioDistribution."
        );
    }

    // Set distribution.
    distrib_ = std::piecewise_linear_distribution<double>(
        intervals.begin(),
        intervals.end(),
        weights.begin()
    );
}

} // namespace placement
} // namespace genesis
