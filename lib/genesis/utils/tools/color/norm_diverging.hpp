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
#include "genesis/utils/tools/color/normalization.hpp"

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
    : public ColorNormalization
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor that sets `min == -1.0`, `mid = 0.0` and `max == 1.0`.
     */
    ColorNormalizationDiverging()
        : ColorNormalization( -1.0, 1.0 )
        , mid_( 0.0 )
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
        : ColorNormalization( min, max )
        , mid_( mid )
    {
        range_check_throw_();
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

    ColorNormalizationDiverging(ColorNormalizationDiverging const&)     = default;
    ColorNormalizationDiverging(ColorNormalizationDiverging&&) noexcept = default;

    ColorNormalizationDiverging& operator= (ColorNormalizationDiverging const&)     = default;
    ColorNormalizationDiverging& operator= (ColorNormalizationDiverging&&) noexcept = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Mid-point value, that is, where the middle value of a diverging_color() is.
     */
    double mid() const
    {
        return mid_;
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
        auto const dist = std::max( std::fabs( center - min() ), std::fabs( center - max() ));
        min( center - dist );
        max( center + dist );
        mid_ = center;

        return *this;
    }

    /**
     * @copydoc mid()
     */
    ColorNormalizationDiverging& mid( double value )
    {
        mid_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Virtual Functions
    // -------------------------------------------------------------------------

    virtual std::map<double, Color> gradient( ColorMap const& map ) const override
    {
        std::map<double, Color> result;

        // Get the fractions of the lower and upper half,
        // which are needed to scale the colors in a diverging palette correctly.
        // For example, a palette with 5, 15, 20 for min, mid and max gets
        // fractions 2/3 and 1/3 here.
        auto const frac_lower = ( mid() - min() ) / ( max() - min() );
        auto const frac_upper = ( max() - mid() ) / ( max() - min() );

        // Divide the palette in two, so that the mixed mid color counts as half a step
        // in palettes with even number of colors.
        auto const scale = 2.0 / static_cast<double>( map.size() - 1 );

        // Lower half.
        for( size_t i = 0; i < map.size() / 2; ++i ) {
            auto const offset = scale * frac_lower * static_cast<double>( i );
            result[ offset ] = map.color( i );
        }

        // For an even number of colors, we need to add a mixed middle color.
        if( map.size() % 2 == 0 ) {
            auto const mid_idx = map.size() / 2;
            auto const mid_color = interpolate(
                map.color( mid_idx - 1 ), map.color( mid_idx ), 0.5
            );
            result[ frac_lower ] = mid_color;
        }

        // Upper half, including mid if uneven number of colors.
        for( size_t i = map.size() / 2; i < map.size(); ++i ) {

            // Step away from end: We go backwards.
            auto const step = static_cast<double>( map.size() - i - 1 );

            // Offset, as before, just going backwards again, so that we end up in right order.
            auto const offset = 1.0 - ( scale * frac_upper * step );
            result[ offset ] = map.color( i );
        }

        return result;
    }

    virtual std::map<double, std::string> tickmarks( size_t num_ticks ) const override
    {
        std::map<double, std::string> result;
        auto tm = Tickmarks();

        // Get the fractions of the lower and upper half,
        // which are needed to scale the colors in a diverging palette correctly.
        // For example, a palette with 5, 15, 20 for min, mid and max gets
        // fractions 2/3 and 1/3 here.
        auto const frac_lower = ( mid() - min() ) / ( max() - min() );
        auto const frac_upper = ( max() - mid() ) / ( max() - min() );

        // Lower half.
        tm.include_max = false;
        auto const tm_labels_l = tm.linear_labels(
            min(), mid(), frac_lower * num_ticks
        );
        for( auto const& tm_label : tm_labels_l ) {
            auto const pos =  frac_lower * tm_label.relative_position;
            result[ pos ] = utils::to_string( tm_label.label );
        }

        // In cases where the mid value is a nice tickmark number (0 for example),
        // it will be included in the tickmarks, although it is the upper limit for
        // the lower half (that is, equal to the max for the half).
        // Thus, we already have a tickmark for the mid value, and now do not need it again
        // when making the upper half ticks. So, exclude the min for the upper half in this case.
        if( tm_labels_l.size() > 0 && tm_labels_l.back().relative_position == 1.0 ) {
            tm.include_min =  false;
        }

        // Upper half.
        tm.include_max = true;
        auto const tm_labels_u = tm.linear_labels(
            mid(), max(), frac_upper * num_ticks
        );
        for( auto const& tm_label : tm_labels_u ) {
            auto const pos =  frac_lower + frac_upper * tm_label.relative_position;
            result[ pos ] = utils::to_string( tm_label.label );
        }

        return result;
    }

    /**
     * @brief Return whether the ranges are correct.
     */
    virtual bool range_check() const override
    {
        return min() < mid_ && mid_ < max();
    }

protected:

    /**
     * @brief Throw if the ranges are incorrect.
     */
    virtual void range_check_throw_() const override
    {
        if( min() >= max() ) {
            throw std::runtime_error( "Invalid Color Normalization with min >= max." );
        }
        if( min() >= mid() ) {
            throw std::runtime_error( "Invalid Color Normalization with min >= mid." );
        }
        if( mid() >= max() ) {
            throw std::runtime_error( "Invalid Color Normalization with mid >= max." );
        }
    }

    virtual double normalize_( double value ) const override
    {
        // Already checked by base class.
        assert( min() <= value && value <= max() );
        assert( range_check() );

        // Bring value into the range [ 0.0, 1.0 ].
        double pos = 0.0;
        if( value < mid_ ) {
            pos = ( value - min() ) / ( mid_ - min() );
            pos /= 2.0;
        } else {
            pos = ( value - mid_ ) / ( max() - mid_ );
            pos /= 2.0;
            pos += 0.5;
        }
        return pos;
    }

    virtual void update_hook_( double min, double max ) override
    {
        mid_ = ( min + max ) / 2.0;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double mid_ = 0.0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
