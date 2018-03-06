#ifndef GENESIS_UTILS_TOOLS_COLOR_NORM_BOUNDARY_H_
#define GENESIS_UTILS_TOOLS_COLOR_NORM_BOUNDARY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/map.hpp"
#include "genesis/utils/tools/color/normalization.hpp"
#include "genesis/utils/tools/tickmarks.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Normalization Boundary
// =================================================================================================

/**
 * @brief Color normalization that maps to discrete intervals.
 *
 * The normalization takes a set of boundaries, which define the edges of intervals.
 * Values falling within an interval are mapped to a value within `[ 0.0, 1.0 ]`
 * that corresponds to the index of the interval. For example, with boundaries `3.0, 4.0, 5.0, 6.0`,
 * we get three intervals. Valus outside of these boundaries are mapped to `-1.0` or `2.0`.
 * Hence, values are mapped as follows:
 *
 *     < 3.0        : -1.0
 *     [ 3.0, 4.0 ) :  0.0
 *     [ 4.0, 5.0 ) :  0.5
 *     [ 5.0, 6.0 ] :  1.0
 *     > 6.0        :  2.0
 *
 * Thus, this normalization can immediately be used with a ColorMap.
 * Furthermore, the function interval() offers to retrieve the interval index for a given value.
 * For the example above, these would be values between 0 and 2 for valid values.
 */
class ColorNormalizationBoundary
    : public ColorNormalization
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief
     */
    ColorNormalizationBoundary() = default;

    /**
     * @brief
     */
    ColorNormalizationBoundary( std::vector<double> const& boundaries )
        : boundaries_( boundaries )
    {
        is_valid_or_throw_();
    }

    ColorNormalizationBoundary( double min, double max, size_t intervals )
    {
        scale( min, max, intervals );
    }

    virtual ~ColorNormalizationBoundary() = default;

    ColorNormalizationBoundary(ColorNormalizationBoundary const&) = default;
    ColorNormalizationBoundary(ColorNormalizationBoundary&&)      = default;

    ColorNormalizationBoundary& operator= (ColorNormalizationBoundary const&) = default;
    ColorNormalizationBoundary& operator= (ColorNormalizationBoundary&&)      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return the boundaries currently set.
     */
    std::vector<double> const& boundaries() const
    {
        return boundaries_;
    }

    /**
     * @brief Return the interval index of a value.
     */
    int interval( double value ) const
    {
        // Make sure that the norm is set up correctly.
        // This is now done while setting, so no need to do this here.
        // is_valid_or_throw_();

        // Extreme cases: value is not within the boundaries.
        // For the upper limit, we return size-1, as this is the first interval index
        // that is not reachable with valid (within-boundary) values.
        assert( boundaries_.size() >= 3 );
        if( value < boundaries_.front() ) {
            return -1;
        }
        if( value > boundaries_.back() ) {
            return boundaries_.size() - 1;
        }

        // Get an iterator to the boundary, and turn it into an interval index upon return.
        // The index calculation has a special case if the value hits exaclty the highest
        // boundary value. In that case, the iterator points to the end of the boundary vector.
        // In all other cases, the iterator points to the end of interval boundary,
        // so we subtract one to get the interval index.
        // The first assertion checks this special case.
        // The second assertion holds because we already checked that the value is not less than
        // the first boundary.
        auto const it = std::upper_bound( boundaries_.begin(), boundaries_.end(), value );
        assert(( it != boundaries_.end() ) xor ( value == boundaries_.back() ));
        assert( it != boundaries_.begin() );

        if( it == boundaries_.end() ) {
            return boundaries_.size() - 2;
        } else {
            return std::distance( boundaries_.begin(), it ) - 1;
        }
    }

    // -------------------------------------------------------------------------
    //     Modificators
    // -------------------------------------------------------------------------

    /**
     * @brief Explicitly set the boundaries to the given @p values. Have to be sorted.
     */
    ColorNormalizationBoundary& boundaries( std::vector<double> const& values )
    {
        boundaries_ = values;
        is_valid_or_throw_();
        return *this;
    }

    /**
     * @brief Set the boundaries to equal-sizes intervals between @p min and @p max,
     * using @p interval many steps.
     */
    ColorNormalizationBoundary& scale( double min, double max, size_t intervals )
    {
        // Make evenly distributed boundary intervals between min and max.
        // We add max separately to increase precision.
        boundaries_.clear();
        double const interval = ( max - min ) / static_cast<double>( intervals );
        for( size_t i = 0; i < intervals; ++i ) {
            boundaries_.push_back( min + static_cast<double>(i) * interval );
        }
        boundaries_.push_back( max );
        is_valid_or_throw_();

        return *this;
    }

    /**
     * @brief Set the boundaries similar to scale(), but use the given vector of values to determine
     * `min` and `max` first.
     */
    ColorNormalizationBoundary& autoscale( std::vector<double> const& values, size_t intervals )
    {
        return autoscale( values.begin(), values.end(), intervals );
    }

    /**
     * @brief Set the boundaries similar to scale(), but use the given range to determine
     * `min` and `max` first.
     */
    template <class ForwardIterator>
    ColorNormalizationBoundary& autoscale( ForwardIterator first, ForwardIterator last, size_t intervals )
    {
        // New values, so that we first do not override the current ones.
        auto min = std::numeric_limits<double>::max();
        auto max = std::numeric_limits<double>::lowest();

        size_t cnt = 0;
        while( first != last ) {
            if( ! std::isfinite( *first ) || *first == mask_value() ) {
                ++first;
                continue;
            }
            if( *first < min ) {
                min = *first;
            }
            if( *first > max ) {
                max = *first;
            }

            ++cnt;
            ++first;
        }

        // Only update if we found values.
        if( cnt == 0 ) {
            return *this;
        }

        scale( min, max, intervals );
        return *this;
    }

    // -------------------------------------------------------------------------
    //     (Pure) Virtual Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the boundaries are correct.
     */
    virtual bool is_valid_() const override
    {
        return boundaries_.size() >= 3 && std::is_sorted( boundaries_.begin(), boundaries_.end() );
    }

protected:

    /**
     * @brief Throw if the boundaries are incorrect.
     */
    virtual void is_valid_or_throw_() const
    {
        if( boundaries_.size() < 3 ) {
            throw std::runtime_error( "Invalid Color Normalization with less than three boundaries." );
        }
        if( ! std::is_sorted( boundaries_.begin(), boundaries_.end() ) ) {
            throw std::runtime_error( "Invalid Color Normalization with unsorted boundaries." );
        }
    }

    /**
     * @brief Normalization function.
     */
    virtual double normalize_( double value ) const override
    {
        auto const idx = interval( value );

        // Special cases: the value is outside of the boundaries.
        if( idx < 0 ) {
            return -1.0;
        }
        if( idx >= static_cast<int>( boundaries_.size() - 1 )) {
            return 2.0;
        }

        // If we are inside the boundaries, calculate a fixed position depending on the interval.
        // As the index returned for too large values is size-1, we need to use size-2 here
        // to normalize properly so that the max value corresponds to a normalized value of 1.0.
        assert( boundaries_.size() >= 3 );
        return static_cast<double>( idx ) / static_cast<double>( boundaries_.size() - 2 );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<double> boundaries_;

};

} // namespace utils
} // namespace genesis

#endif // include guardmask_value_
