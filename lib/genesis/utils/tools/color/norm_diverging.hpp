#ifndef GENESIS_UTILS_TOOLS_COLOR_NORM_DIVERGING_H_
#define GENESIS_UTILS_TOOLS_COLOR_NORM_DIVERGING_H_

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

#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/tools/color/norm_linear.hpp"

#include <cmath>
#include <limits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Normalization Diverging
// =================================================================================================

/**
 * @brief Color normalization for a diverging scale.
 *
 * The min() and max() values determine the boundaries of the range of allowed values;
 * they map to the first and last entry of the palette, respectively.
 * The mid() value is used as the "neutral" value in the range of allowed values, that is, it
 * maps to the central color of the palette.
 *
 * In other words, values in `[ min, mid ]` are mapped to the first half of the palette,
 * and values in `[ mid, max ]` to the second half:
 *
 *          palette
 *        |----|----|
 *       /      \    \
 *      /        \    \
 *      |---------|---|
 *     min      mid  max
 *
 * Values outside of the allowed range `[ min, max ]`, as well as non-finite values, are
 * treated according to the settings clip_under(), clip_over(), mask_value(), mask_color(),
 * under_color(), over_color().
 */
class ColorNormalizationDiverging
    : public ColorNormalizationLinear
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor that sets `min == -1.0`, `mid = 0.0` and `max == 1.0`.
     */
    ColorNormalizationDiverging()
        : ColorNormalizationLinear( -1.0, 1.0 )
        , mid_value_( 0.0 )
    {}

    /**
     * @brief Constructor that sets min() and max() to the provided values, and mid() to their
     * midpoint.
     */
    ColorNormalizationDiverging( double min, double max )
        : ColorNormalizationDiverging( min, ( min + max ) / 2.0, max )
    {}

    /**
     * @brief Constructor that sets min(), mid() and max() to the provided values, in that order.
     */
    ColorNormalizationDiverging( double min, double mid, double max )
        : ColorNormalizationLinear( min, max )
        , mid_value_( mid )
    {
        is_valid_or_throw_();
    }

    /**
     * @brief Constructor that sets min() and max() to the min and max of the provided @p values,
     * and mid() to their midpoint.
     */
    ColorNormalizationDiverging( std::vector<double> const& values )
    {
        autoscale( values.begin(), values.end() );
    }

    /**
     * @brief Constructor that sets min() and max() to the min and max of the provided range,
     * and mid() to their midpoint.
     */
    template <class ForwardIterator>
    ColorNormalizationDiverging( ForwardIterator first, ForwardIterator last )
    {
        autoscale( first, last );
    }

    virtual ~ColorNormalizationDiverging() = default;

    ColorNormalizationDiverging(ColorNormalizationDiverging const&) = default;
    ColorNormalizationDiverging(ColorNormalizationDiverging&&)      = default;

    ColorNormalizationDiverging& operator= (ColorNormalizationDiverging const&) = default;
    ColorNormalizationDiverging& operator= (ColorNormalizationDiverging&&)      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Mid-point value, that is, where the middle value of a diverging_color() is.
     */
    double mid_value() const
    {
        return mid_value_;
    }

    // -------------------------------------------------------------------------
    //     Modificators
    // -------------------------------------------------------------------------

    /**
     * @brief Make the range symmetric around a center value.
     *
     * For example, if autoscale() yielded `min == -0.89` and `max == 0.95`, then calling this
     * function with `center == 0.0` sets `min == -0.95` and `mid == 0.0`;
     */
    ColorNormalizationDiverging& make_centric( double center = 0.0 )
    {
        // Set the min and max so that they are symmetric around center.
        auto const dist = std::max(
            std::fabs( center - min_value() ),
            std::fabs( center - max_value() )
        );
        min_value( center - dist );
        max_value( center + dist );
        mid_value_ = center;

        return *this;
    }

    /**
     * @copydoc mid()
     */
    ColorNormalizationDiverging& mid_value( double value )
    {
        mid_value_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Virtual Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the ranges are correct.
     */
    virtual bool is_valid_() const override
    {
        return min_value() < mid_value_ && mid_value_ < max_value();
    }

protected:

    /**
     * @brief Throw if the ranges are incorrect.
     */
    virtual void is_valid_or_throw_() const override
    {
        if( min_value() >= max_value() ) {
            throw std::runtime_error( "Invalid Color Normalization with min >= max." );
        }
        if( min_value() >= mid_value() ) {
            throw std::runtime_error( "Invalid Color Normalization with min >= mid." );
        }
        if( mid_value() >= max_value() ) {
            throw std::runtime_error( "Invalid Color Normalization with mid >= max." );
        }
    }

    virtual double normalize_( double value ) const override
    {
        // Already checked by base class.
        assert( min_value() <= value && value <= max_value() );
        assert( is_valid_() );

        // Bring value into the range [ 0.0, 1.0 ].
        double pos = 0.0;
        if( value < mid_value_ ) {
            pos = ( value - min_value() ) / ( mid_value_ - min_value() );
            pos /= 2.0;
        } else {
            pos = ( value - mid_value_ ) / ( max_value() - mid_value_ );
            pos /= 2.0;
            pos += 0.5;
        }
        return pos;
    }

    virtual void update_hook_( double min, double max ) override
    {
        mid_value_ = ( min + max ) / 2.0;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double mid_value_ = 0.0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
