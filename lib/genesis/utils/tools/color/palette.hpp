#ifndef GENESIS_UTILS_TOOLS_COLOR_PALETTE_H_
#define GENESIS_UTILS_TOOLS_COLOR_PALETTE_H_

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

#include "genesis/utils/tools/color.hpp"

#include <cmath>
#include <limits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Palette
// =================================================================================================

/**
 * @brief
 */
class ColorPalette
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    ColorPalette() = default;

    ColorPalette( std::vector<Color> const& colors )
    {
        palette( colors );
    }

    ~ColorPalette() = default;

    ColorPalette(ColorPalette const&)     = default;
    ColorPalette(ColorPalette&&) noexcept = default;

    ColorPalette& operator= (ColorPalette const&)     = default;
    ColorPalette& operator= (ColorPalette&&) noexcept = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    double min() const
    {
        return min_;
    }

    double mid() const
    {
        return mid_;
    }

    double max() const
    {
        return max_;
    }

    /**
     * @brief Mask value that identifies invalid values.
     *
     * When querying a value that compares equal to the mask value, the color functions
     * return mask_color() instead of a Color from the palette. This is meant as a simple means
     * of filtering and visualizing invalid values.
     *
     * The mask value is initialized to quiet NaN, so that it never compares equal to anything.
     * Thus, it is not used by default has to be explicitly set to a different value.
     */
    double mask_value() const
    {
        return mask_value_;
    }

    /**
     * @brief Color that indicates values equal to mask_value().
     */
    Color mask_color() const
    {
        return mask_color_;
    }

    /**
     * @brief Color that indicates values greater than max().
     *
     * Only used if `clip() == false`.
     */
    Color over_color() const
    {
        return over_color_;
    }

    /**
     * @brief Color that indicates values less than min().
     *
     * Only used if `clip() == false`.
     */
    Color under_color() const
    {
        return under_color_;
    }

    /**
     * @brief Clip values to be inside `[ min max ]`.
     *
     * If set to `true`, over_color() and under_color() are not used to indicate values out of
     * range.
     */
    bool clip() const
    {
        return clip_;
    }

    /**
     * @brief Use the palette colors in reverse, back to front.
     */
    bool reverse() const
    {
        return reverse_;
    }

    std::vector<Color> const& palette() const
    {
        return palette_;
    }

    // -------------------------------------------------------------------------
    //     Modificators
    // -------------------------------------------------------------------------

    ColorPalette& range( double min, double max );

    ColorPalette& range( double min, double mid, double max );

    /**
     * @brief Set the min and max of the Palette to the min and max valid values
     * that are found in the range `[ first, last )`.
     *
     * Furthermore, mid is set to the average of the values.
     *
     * The provided iterator range needs to contain values that are convertible and comparable
     * to `double`. Any non-finite values or values that are equal to the mask_value() are skipped.
     */
    template <class ForwardIterator>
    void range( ForwardIterator first, ForwardIterator last )
    {
        // New values, so that we first do not override the current ones.
        auto min = std::numeric_limits<double>::max();
        auto max = std::numeric_limits<double>::lowest();

        double sum = 0.0;
        size_t cnt = 0;

        while( first != last ) {
            if( ! std::isfinite( *first ) || *first == mask_value_ ) {
                continue;
            }
            if( *first < min ) {
                min = *first;
            }
            if( *first > max ) {
                max = *first;
            }

            sum += *first;
            ++cnt;

            ++first;
        }

        // Only update if we found values.
        if( cnt > 0 ) {
            min_ = min;
            max_ = max;

            // mid_ = ( min + max ) / 2.0;
            mid_ = sum / static_cast<double>( cnt );
        }
    }

    ColorPalette& min( double value )
    {
        min_ = value;
        return *this;
    }

    ColorPalette& mid( double value )
    {
        mid_ = value;
        return *this;
    }

    ColorPalette& max( double value )
    {
        max_ = value;
        return *this;
    }

    ColorPalette& mask_value( double value )
    {
        mask_value_ = value;
        return *this;
    }

    ColorPalette& mask_color( Color value )
    {
        mask_color_ = value;
        return *this;
    }

    ColorPalette& over_color( Color value )
    {
        over_color_ = value;
        return *this;
    }

    ColorPalette& under_color( Color value )
    {
        under_color_ = value;
        return *this;
    }

    ColorPalette& clip( bool value )
    {
        clip_ = value;
        return *this;
    }

    ColorPalette& reverse( bool value )
    {
        reverse_ = value;
        return *this;
    }

    ColorPalette& palette( std::vector<Color> const& value )
    {
        palette_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Palette
    // -------------------------------------------------------------------------

    Color qualitative_color( size_t index ) const;

    Color sequential_color( double value ) const;

    /**
     * @brief Get a color by using the palette for a diverging scale.
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
     * treated according to the settings clip(), mask_value(), mask_color(), under_color(),
     * over_color().
     */
    Color diverging_color( double value ) const;

    std::vector<Color> qualitative_colors( std::vector<size_t> const& values ) const;

    std::vector<Color> sequential_colors( std::vector<double> const& values ) const;

    std::vector<Color> diverging_colors( std::vector<double> const& values ) const;

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Return an entry at @p index from the palette, taking reverse() into account.
     */
    Color get_entry_( size_t index ) const
    {
        if( reverse_ ) {
            index = palette_.size() - index - 1;
        }
        return palette_[ index ];
    }

    /**
     * @brief Internal helper function that does the range checks.
     *
     * If clip() is `true`, the function can modify @p value.
     * It throws if invalid conditions are found.
     * It returns a pair, where the bool indicates whether a check failed,
     * using the Color as an indicator of what failed.
     */
    std::pair<Color, bool> boundary_checks_( double& value ) const;

    /**
     * @brief Returns an interpolated color for a value in the range `[ 0.0, 1.0 ]`,
     * representing a position in the palette.
     */
    Color get_interpolated_color_( double value ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double min_   = 0.0;
    double mid_   = 0.5;
    double max_   = 1.0;

    double mask_value_  = std::numeric_limits<double>::quiet_NaN();

    Color mask_color_   = { 1.0, 1.0, 0.0 };
    Color over_color_   = { 0.0, 1.0, 1.0 };
    Color under_color_  = { 1.0, 0.0, 1.0 };

    bool clip_    = false;
    bool reverse_ = false;

    std::vector<Color> palette_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
