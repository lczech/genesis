/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/tools/tickmarks.hpp"

#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Tickmarks
// ================================================================================================

double Tickmarks::step_size( double interval_size, size_t target_steps )
{
    // Adapted from
    // http://stackoverflow.com/questions/361681/algorithm-for-nice-grid-line-intervals-on-a-graph

    if( target_steps == 0 ) {
        throw std::invalid_argument( "Cannot calculate tickmark step size for 0 steps." );
    }
    if( interval_size <= 0.0 ) {
        throw std::invalid_argument( "Cannot calculate tickmark step size for negative intervals." );
    }

    // Calculate an initial guess at step size.
    auto const step_guess = interval_size / static_cast<double>( target_steps );

    // Get the magnitude of the step size.
    auto const mag = std::floor( std::log10( step_guess ));
    auto const mag_pow = std::pow( 10, mag );

    // Calculate most significant digit (MSD) of the new step size.
    auto mag_msd = static_cast<int>( step_guess / mag_pow + 0.5 );

    // Promote the MSD to either 1, 2, 5 or 10.
    if( mag_msd > 5 ) {
        mag_msd = 10;
    } else if( mag_msd > 2 ) {
        mag_msd = 5;
    } else if( mag_msd > 1 ) {
        mag_msd = 2;
    } else {
        assert( mag_msd == 1 );
    }

    return static_cast<double>( mag_msd ) * mag_pow;
}

std::vector<double> Tickmarks::linear_ticks( double min, double max, size_t target_steps )
{
    // Prepare result.
    auto res = std::vector<double>();

    if( max < min ) {
        throw std::runtime_error( "Cannot calculate scale with max < min." );
    }
    auto const interval_size = max - min;

    // We want the ticks to be somewhat rounded, within some epsilon of the nice values that
    // we are trying to create here, in order to avoid artifacts such as 1e-18 instead of 0.
    // As the epsilon of double values is in the order of 1e-18 itself, we do e-15 here,
    // which is large enough to avoid these trailing rounding errors, but small enough to not
    // affect the numerical results too much, which is important for linear_labels() to work.
    auto const rounding_accuracy = 15;

    // The case of 0 target steps can happen for example in SvgPalette.
    // In that case, we only output min and max if needed, but not any inner tickmarks.
    if( target_steps > 0 ) {
        // Get step size.
        auto const step_sz = step_size( interval_size, target_steps );

        // Calculate first tick position, so that it is the largest multiple of the step size
        // that is below the min.
        auto tick = step_sz * std::floor( min / step_sz );

        // Determine whether we want to start before or after the min.
        if( ! undershoot_at_min ) {
            tick += step_sz;
        }

        // Add ticks to the list.
        while( tick <= max ) {
            res.push_back( round_to( tick, rounding_accuracy ));
            tick += step_sz;
        }

        // Determine whether we want to stop before or after the max.
        if( overshoot_at_max ) {
            res.push_back( round_to( tick, rounding_accuracy ));
        }
    }

    // Add min and max if needed.
    if( include_min ) {
        res.push_back( round_to( min, rounding_accuracy ));
    }
    if( include_max ) {
        res.push_back( round_to( max, rounding_accuracy ));
    }

    // Cleanup duplicate entries and those that are close by. We do not need ticks that are
    // too close to each other.
    // It is easier to do this than to check for duplicate entries in each addition step...
    std::sort( res.begin(), res.end() );
    auto uniq_end = std::unique( res.begin(), res.end(), [&]( double lhs, double rhs ){
        return almost_equal_relative( lhs, rhs, relative_epsilon );
    });
    res.erase( uniq_end, res.end() );

    return res;
}

std::vector<Tickmarks::LabeledTick> Tickmarks::linear_labels(
    double min, double max, size_t target_steps
) {
    auto res = std::vector<LabeledTick>();

    auto ticks = linear_ticks( min, max, target_steps );
    auto range = max - min;

    for( auto const& tick : ticks ) {
        auto rel_pos = ( tick - min ) / range;
        res.emplace_back( rel_pos, tick );
    }
    return res;
}

std::vector<Tickmarks::LabeledTick> Tickmarks::logarithmic_labels( double min, double max, double base )
{
    auto res = std::vector<LabeledTick>();

    if( min <= 0.0 ) {
        throw std::runtime_error( "Cannot calculate logarithmic scale for negative values." );
    }
    if( min >= max ) {
        throw std::runtime_error( "Cannot calculate scale with min >= max." );
    }
    if( base < 0.0 ) {
        throw std::runtime_error( "Cannot calculate logarithmic scale with negative base." );
    }

    // Start at a power below min.
    double exp_i = std::floor( std::log( min ) / std::log( base ) );

    // Determine whether we want to start before or after the min.
    if( ! undershoot_at_min ) {
        exp_i += 1.0;
    }

    // Constants.
    auto const lg_min = std::log( min ) / std::log( base );
    auto const lg_max = std::log( max ) / std::log( base );

    // Either stop at max or do one more loop if we want to overshoot.
    auto const lim = lg_max + ( overshoot_at_max ? 1.0 : 0.0 );

    while( exp_i <= lim ) {
        auto const rel_pos = ( exp_i - lg_min ) / ( lg_max - lg_min );
        res.emplace_back( rel_pos, std::pow( base, exp_i ) );

        // Next order of magnitude.
        exp_i += 1.0;
    }

    // Add min and max if needed.
    if( include_min ) {
        res.emplace_back( 0.0, min );
    }
    if( include_max ) {
        res.emplace_back( 1.0, max );
    }

    // Cleanup duplicate entries and those that are close by. We do not need ticks that are
    // too close to each other.
    // It is easier to do this than to check for duplicate entries in each addition step...
    std::sort( res.begin(), res.end(), []( LabeledTick const& lhs, LabeledTick const& rhs ){
        return lhs.relative_position < rhs.relative_position;
    });
    auto uniq_end = std::unique( res.begin(), res.end(),
        [&]( LabeledTick const& lhs, LabeledTick const& rhs ){
            return almost_equal_relative(
                lhs.relative_position,
                rhs.relative_position,
                relative_epsilon
            );
        }
    );
    res.erase( uniq_end, res.end() );

    return res;
}

} // namespace utils
} // namespace genesis
