#ifndef GENESIS_UTILS_MATH_TICKMARKS_H_
#define GENESIS_UTILS_MATH_TICKMARKS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup utils
 */

#include "utils/math/common.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Tickmarks
// =================================================================================================

/**
 * @brief Helper class to find "nice" tickmark intervals for creating scales and axis.
 */
struct Tickmarks
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Tickmarks() = default;
    ~Tickmarks() = default;

    Tickmarks(Tickmarks const&)     = default;
    Tickmarks(Tickmarks&&) noexcept = default;

    Tickmarks& operator= (Tickmarks const&)     = default;
    Tickmarks& operator= (Tickmarks&&) noexcept = default;

    // -------------------------------------------------------------------------
    //     Drudges
    // -------------------------------------------------------------------------

    /**
     * @brief Calculate a step size that fills the `interval_size` in approximately `target_steps`
     * many steps.
     *
     * The resulting step size is calculated to be a "nice" size, i.e. fits `[ 1 | 2 | 5 ] * 10^n`.
     * Thus, exemplary nice step sizes are 0.01, 20 and 500.
     */
    static double step_size( double interval_size, size_t target_steps )
    {
        // Adapted from
        // http://stackoverflow.com/questions/361681/algorithm-for-nice-grid-line-intervals-on-a-graph

        // Calculate an initial guess at step size.
        auto step_guess = interval_size / static_cast<double>( target_steps );

        // Get the magnitude of the step size.
        auto mag = std::floor( std::log10( step_guess ));
        auto mag_pow = std::pow( 10, mag );

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

    std::vector<double> linear_ticks( double min, double max, size_t target_steps )
    {
        auto res = std::vector<double>();

        // Get step size.
        auto interval_size = max - min;
        auto step_sz = step_size( interval_size, target_steps );

        // Calculate first tick position, so that it is the largest multiple of the step size
        // that is below the min.
        auto tick = step_sz * std::floor( min / step_sz );

        // Determine whether we want to start before or after the min.
        if( ! undershoot_at_min ) {
            tick += step_sz;
        }

        // Add ticks to the list.
        while( tick <= max ) {
            res.push_back( tick );
            tick += step_sz;
        }

        // Determine whether we want to stop before or after the max.
        if( overshoot_at_max ) {
            res.push_back( tick );
        }

        // Add min and max if needed.
        if( include_min ) {
            res.push_back( min );
        }
        if( include_max ) {
            res.push_back( max );
        }

        // Cleanup duplicate entries and those that are close by. We do not need ticks that are
        // too close to each other.
        // It is easier to do this than to check for duplicate entries in each addition step...
        std::sort( res.begin(), res.end() );
        auto uniq_end = std::unique( res.begin(), res.end(), [&]( double lhs, double rhs ){
            return almost_equal_relative( lhs, rhs, relative_epsilon );
        });
        res.resize( std::distance( res.begin(), uniq_end ));

        return res;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Should the provided min value be included in the resulting list of ticks or not.
     */
    bool include_min = true;

    /**
     * @brief Should the provided max value be included in the resulting list of ticks or not.
     */
    bool include_max = true;

    /**
     * @brief Should the lowest value in the resulting list of ticks be below the provided min
     * value (`true`) or not (`false`).
     */
    bool undershoot_at_min = false;

    /**
     * @brief Should the highest value in the resulting list of ticks be above the provided max
     * value (`true`) or not (`false`).
     */
    bool overshoot_at_max = false;

    /**
     * @brief Relative epsilon used to exclude two tickmarks that are too close to each other.
     */
    double relative_epsilon = std::numeric_limits<double>::epsilon();

};

} // namespace utils
} // namespace genesis

#endif // include guard
