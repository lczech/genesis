#ifndef GENESIS_UTILS_TOOLS_COLOR_MAP_H_
#define GENESIS_UTILS_TOOLS_COLOR_MAP_H_

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

#include <cmath>
#include <limits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class ColorNormalization;

// =================================================================================================
//     Color Map
// =================================================================================================

/**
 * @brief Store a list of colors and offer them as a map for values in range `[ 0.0, 1.0 ]`.
 *
 * The class is an abstraction of color lists, making them easier to use for ranges, gradients, etc.
 * When invoked, it interpolates between entries of the list according to the provided value.
 * It is best used in combination with a ColorNormalization, so that arbitrary ranges can be
 * mapped into the allowed interval `[ 0.0, 1.0 ]`.
 */
class ColorMap
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    ColorMap() = default;

    ColorMap( std::vector<Color> const& colors )
    {
        palette( colors );
    }

    ~ColorMap() = default;

    ColorMap(ColorMap const&) = default;
    ColorMap(ColorMap&&)      = default;

    ColorMap& operator= (ColorMap const&)     = default;
    ColorMap& operator= (ColorMap&&)      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Color that indicates values equal to mask_value() or non-finite values.
     */
    Color mask_color() const
    {
        return mask_color_;
    }

    /**
     * @brief Color that indicates values greater than max().
     *
     * Only used if `clip_over() == false`.
     */
    Color over_color() const
    {
        return over_color_;
    }

    /**
     * @brief Color that indicates values less than min().
     *
     * Only used if `clip_under() == false`.
     */
    Color under_color() const
    {
        return under_color_;
    }

    /**
     * @brief Clip (clamp) values less than min() to be inside `[ min, max ]`.
     *
     * If set to `true`, under_color() is not used to indicate values out of range.
     */
    bool clip_under() const
    {
        return clip_under_;
    }

    /**
     * @brief Clip (clamp) values greater than max() to be inside `[ min, max ]`.
     *
     * If set to `true`, over_color() is not used to indicate values out of range.
     */
    bool clip_over() const
    {
        return clip_over_;
    }

    /**
     * @brief Use the palette colors in reverse, back to front.
     */
    bool reverse() const
    {
        return reverse_;
    }

    // -------------------------------------------------------------------------
    //     Modificators
    // -------------------------------------------------------------------------

    /**
     * @copydoc mask_color()
     */
    ColorMap& mask_color( Color value )
    {
        mask_color_ = value;
        return *this;
    }

    /**
     * @copydoc over_color()
     */
    ColorMap& over_color( Color value )
    {
        over_color_ = value;
        return *this;
    }

    /**
     * @copydoc under_color()
     */
    ColorMap& under_color( Color value )
    {
        under_color_ = value;
        return *this;
    }

    /**
     * Set both clip_under( bool ) and clip_over( bool ).
     */
    ColorMap& clip( bool value )
    {
        clip_under_ = value;
        clip_over_  = value;
        return *this;
    }

    /**
     * @copydoc clip_under()
     */
    ColorMap& clip_under( bool value )
    {
        clip_under_ = value;
        return *this;
    }

    /**
     * @copydoc clip_over()
     */
    ColorMap& clip_over( bool value )
    {
        clip_over_ = value;
        return *this;
    }

    ColorMap& reverse( bool value )
    {
        reverse_ = value;
        return *this;
    }

    ColorMap& palette( std::vector<Color> const& value )
    {
        palette_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Palette
    // -------------------------------------------------------------------------

    /**
     * @brief Get the color list currently in use.
     */
    std::vector<Color> const& palette() const
    {
        return palette_;
    }

    /**
     * @brief Return whether the Palette is empty, that is, no colors were set.
     */
    bool empty() const
    {
        return palette_.empty();
    }

    /**
     * @brief Return the size of the map, that is, the number of colors in the list.
     */
    size_t size() const
    {
        return palette_.size();
    }

    /**
     * @brief Return a particular color from the palette, module the palette size.
     *
     * This is useful for qualitative measures. The function respects the reverse() setting,
     * and uses modulo for indices out of range, that is, it "wraps around".
     */
    Color color( size_t index ) const
    {
        if( palette_.size() == 0 ) {
            return Color();
        }
        return get_entry_( index % palette_.size() );
    }

    /**
     * @brief Get a color list based on the palette, containing @p n colors sampled at equal
     * distance across the palette.
     *
     * This is for example useful for creating a palette to write a bitmap file, see
     * @link BmpWriter::to_file( Matrix<unsigned char> const&, std::vector<Color> const&, std::string const& ) const BmpWriter::to_file()@endlink.
     *
     * If `n == 0`, the original palette is returned, making it equal to palette().
     * If `n == 1`, the mid point color is returned.
     */
    std::vector<Color> color_list( size_t n = 256 ) const;

    // -------------------------------------------------------------------------
    //     Mapping
    // -------------------------------------------------------------------------

    /**
     * @brief Return an interpolated color for a @p value in the range `[ 0.0, 1.0 ]`,
     * representing a position in the palette.
     *
     * Values less than `0.0` are mapped to under_color() or to the first color of the map,
     * depending on whether clip_under() is set. The same applies for values greater than `1.0`:
     * Either they map to over_color() or the last color in the map, depending on whether
     * clip_over() is set. Lastly, any non-finite values (e.g., NaN) are mapped to mask_color().
     */
    Color operator() ( double value ) const;

    /**
     * @brief Return the mapped colors for a vector of @p values.
     *
     * Simply applies operator() ( double value ) to all values.
     */
    std::vector<Color> operator() ( std::vector<double> const& values ) const;

    /**
     * @brief Return the mapped colors for a range of values.
     *
     * Simply applies operator() ( double value ) to all values.
     */
    template <class ForwardIterator>
    std::vector<Color> operator()( ForwardIterator first, ForwardIterator last ) const
    {
        std::vector<Color> result;
        while( first != last ) {
            result.push_back( operator()( *first ) );
            ++first;
        }
        return result;
    }

    /**
     * @brief Return a Color for the given @p value, normalized by @p norm.
     */
    Color operator() ( ColorNormalization const& norm, double value ) const;

    /**
     * @brief Return the mapped colors for a vector of @p values, normalized by @p norm.
     */
    std::vector<Color> operator() ( ColorNormalization const& norm, std::vector<double> const& values ) const;

    /**
     * @brief Return the mapped colors for a range of values, normalized by @p norm.
     */
    template <class ForwardIterator>
    std::vector<Color> operator()(
        ColorNormalization const& norm,
        ForwardIterator first,
        ForwardIterator last
    ) const {
        std::vector<Color> result;
        while( first != last ) {
            result.push_back( operator()( norm, *first ) );
            ++first;
        }
        return result;
    }

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

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    Color mask_color_   = { 1.0, 1.0, 0.0 };
    Color over_color_   = { 0.0, 1.0, 1.0 };
    Color under_color_  = { 1.0, 0.0, 1.0 };

    bool clip_under_ = false;
    bool clip_over_  = false;
    bool reverse_    = false;

    std::vector<Color> palette_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
