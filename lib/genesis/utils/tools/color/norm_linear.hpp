#ifndef GENESIS_UTILS_TOOLS_COLOR_NORM_LINEAR_H_
#define GENESIS_UTILS_TOOLS_COLOR_NORM_LINEAR_H_

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
//     Color Normalization
// =================================================================================================

/**
 * @brief Default Color normalization, using a sequential linear scaling in the
 * range `[ min, max ]`.
 */
class ColorNormalizationLinear
    : public ColorNormalization
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor that sets `min == 0.0` and `max == 1.0`.
     */
    ColorNormalizationLinear() = default;

    /**
     * @brief Constructor that sets min() and max() to the provided values.
     */
    ColorNormalizationLinear( double min, double max )
        : min_value_( min )
        , max_value_( max )
    {
        is_valid_or_throw_();
    }

    /**
     * @brief Constructor that sets min() and max() to the min and max of the provided @p values.
     */
    ColorNormalizationLinear( std::vector<double> const& values )
    {
        autoscale( values.begin(), values.end() );
    }

    /**
     * @brief Constructor that sets min() and max() to the min and max of the provided range.
     */
    template <class ForwardIterator>
    ColorNormalizationLinear( ForwardIterator first, ForwardIterator last )
    {
        autoscale( first, last );
    }

    virtual ~ColorNormalizationLinear() = default;

    ColorNormalizationLinear(ColorNormalizationLinear const&) = default;
    ColorNormalizationLinear(ColorNormalizationLinear&&)      = default;

    ColorNormalizationLinear& operator= (ColorNormalizationLinear const&) = default;
    ColorNormalizationLinear& operator= (ColorNormalizationLinear&&)      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Minimum value, that is, where to begin the color scale.
     */
    double min_value() const
    {
        return min_value_;
    }

    /**
     * @brief Minimum value, that is, where to end the color scale.
     */
    double max_value() const
    {
        return max_value_;
    }

    // -------------------------------------------------------------------------
    //     Modificators
    // -------------------------------------------------------------------------

    /**
     * @brief
     */
    ColorNormalizationLinear& scale( double min, double max )
    {
        min_value_ = min;
        max_value_ = max;
        update_hook_( min, max );
        return *this;
    }

    /**
     * @brief
     */
    ColorNormalizationLinear& autoscale( std::vector<double> const& values )
    {
        return autoscale( values.begin(), values.end() );
    }

    /**
     * @brief
     */
    ColorNormalizationLinear& autoscale_min( std::vector<double> const& values )
    {
        return autoscale_min( values.begin(), values.end() );
    }

    /**
     * @brief
     */
    ColorNormalizationLinear& autoscale_max( std::vector<double> const& values )
    {
        return autoscale_max( values.begin(), values.end() );
    }

    /**
     * @brief Set the min and max of the Palette so that they reflect the min and max valid values
     * that are found in the range `[ first, last )`.
     *
     * The provided iterator range needs to contain values that are convertible and comparable
     * to `double`. Any non-finite values or values that are equal to the mask_value() are skipped.
     * If then no value is found at all, the min and max are not changed.
     *
     * In derived classes, other values might also be set from this. For example,
     * ColorNormalizationLinearDiverging also sets the mid value accordingly.
     */
    template <class ForwardIterator>
    ColorNormalizationLinear& autoscale( ForwardIterator first, ForwardIterator last )
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

        // Set the values simply to what we found.
        min_value_ = min;
        max_value_ = max;
        update_hook_( min, max );

        return *this;
    }

    /**
     * @brief Same as autoscale(), but only updates the min_value().
     */
    template <class ForwardIterator>
    ColorNormalizationLinear& autoscale_min( ForwardIterator first, ForwardIterator last )
    {
        auto const max = max_value_;
        autoscale( first, last );
        max_value_ = max;
        update_hook_( min_value_, max_value_ );
        return *this;
    }

    /**
     * @brief Same as autoscale(), but only updates the max_value().
     */
    template <class ForwardIterator>
    ColorNormalizationLinear& autoscale_max( ForwardIterator first, ForwardIterator last )
    {
        auto const min = min_value_;
        autoscale( first, last );
        min_value_ = min;
        update_hook_( min_value_, max_value_ );
        return *this;
    }

    /**
     * @copydoc min()
     */
    ColorNormalizationLinear& min_value( double value )
    {
        min_value_ = value;
        update_hook_( min_value_, max_value_ );
        return *this;
    }

    /**
     * @copydoc max()
     */
    ColorNormalizationLinear& max_value( double value )
    {
        max_value_ = value;
        update_hook_( min_value_, max_value_ );
        return *this;
    }

    // -------------------------------------------------------------------------
    //     (Pure) Virtual Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the ranges are correct.
     */
    virtual bool is_valid_() const override
    {
        return min_value_ < max_value_;
    }

protected:

    /**
     * @brief Throw if the ranges are incorrect.
     */
    virtual void is_valid_or_throw_() const
    {
        if( min_value_ >= max_value_ ) {
            throw std::runtime_error( "Invalid Color Normalization with min >= max." );
        }
    }

    /**
     * @brief Normalization function.
     *
     * Return a value in range `[ 0.0, 1.0 ]`. Derived classes can override this
     * to provide their specific normalization.
     */
    virtual double normalize_( double value ) const override
    {
        // Make sure that the norm is set up correctly.
        is_valid_or_throw_();

        // Extreme cases.
        if( value < min_value_ ) {
            return -1.0;
        }
        if( value > max_value_ ) {
            return 2.0;
        }
        assert( min_value_ <= value && value <= max_value_ );

        // Bring value into the range [ 0.0, 1.0 ].
        auto const pos = ( value - min_value_ ) / ( max_value_ - min_value_ );
        return pos;
    }

    /**
     * @brief Called whenever the min and max are set automatically.
     * Gives derived classes a chance to update their values.
     */
    virtual void update_hook_( double min, double max )
    {
        // Nothing to do for this class.
        (void) min;
        (void) max;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double min_value_  = 0.0;
    double max_value_  = 1.0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
