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

#include "genesis/utils/tools/color/palette.hpp"

#include "genesis/utils/tools/color/functions.hpp"

#include <algorithm>
#include <cassert>
#include <cassert>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Modificators
// =================================================================================================

ColorPalette& ColorPalette::range( double min, double max )
{
    min_ = min;
    mid_ = ( min + max ) / 2.0;
    max_ = max;
    return *this;
}

ColorPalette& ColorPalette::range( double min, double mid, double max )
{
    min_ = min;
    mid_ = mid;
    max_ = max;
    return *this;
}

// =================================================================================================
//     Palette
// =================================================================================================

Color ColorPalette::qualitative_color( size_t index ) const
{
    if( palette_.size() == 0 ) {
        return Color();
    }
    return get_entry_( index % palette_.size() );
}

Color ColorPalette::sequential_color( double value ) const
{
    // Perform checks. Might manipulate value.
    auto const checks = boundary_checks_( value );
    if( checks.second ) {
        return checks.first;
    }

    // Bring value into the range [ 0.0, 1.0 ].
    auto const pos = ( value - min_ ) / ( max_ - min_ );

    // Get the color at that position in the palette.
    return get_interpolated_color_( pos );
}

Color ColorPalette::diverging_color( double value ) const
{
    // Perform checks. Might manipulate value.
    auto const checks = boundary_checks_( value );
    if( checks.second ) {
        return checks.first;
    }

    // Bring value into the range [ 0.0, 1.0 ].
    double pos = 0.0;
    if( value < mid_ ) {
        pos = ( value - min_ ) / ( mid_ - min_ );
        pos /= 2.0;
    } else {
        pos = ( value - mid_ ) / ( max_ - mid_ );
        pos /= 2.0;
        pos += 0.5;
    }

    // Get the color at that position in the palette.
    return get_interpolated_color_( pos );
}

std::vector<Color> ColorPalette::qualitative_colors( std::vector<size_t> const& values ) const
{
    auto result = std::vector<Color>( values.size() );
    for( size_t i = 0; i < values.size(); ++i ) {
        result[i] = qualitative_color( values[i] );
    }
    return result;
}

std::vector<Color> ColorPalette::sequential_colors( std::vector<double> const& values ) const
{
    auto result = std::vector<Color>( values.size() );
    for( size_t i = 0; i < values.size(); ++i ) {
        result[i] = sequential_color( values[i] );
    }
    return result;
}

std::vector<Color> ColorPalette::diverging_colors( std::vector<double> const& values ) const
{
    auto result = std::vector<Color>( values.size() );
    for( size_t i = 0; i < values.size(); ++i ) {
        result[i] = diverging_color( values[i] );
    }
    return result;
}

// =================================================================================================
//     Internal Functions
// =================================================================================================

std::pair<Color, bool> ColorPalette::boundary_checks_( double& value ) const
{
    // Extreme cases check.
    if( palette_.size() == 0 ) {
        return { Color( 0.0, 0.0, 0.0 ), true};
    }
    if( palette_.size() == 1 ) {
        return { palette_[0], true };
    }
    if( min_ >= max_ ) {
        throw std::invalid_argument( "Invalid ColorPalette with min >= max." );
    }
    if( min_ >= mid_ ) {
        throw std::invalid_argument( "Invalid ColorPalette with min >= mid." );
    }
    if( mid_ >= max_ ) {
        throw std::invalid_argument( "Invalid ColorPalette with mid >= max." );
    }

    // Boundary checks.
    if( ! std::isfinite( value ) || value == mask_value_ ) {
        return { mask_color_, true };
    }
    if( clip_ ) {
        value = std::min( std::max( min_, value ), max_ );
    }
    if( value < min_ ) {
        return { under_color_, true };
    }
    if( value > max_ ) {
        return { over_color_, true };
    }

    // Nothing bad happend.
    return{ Color(), false };
}

Color ColorPalette::get_interpolated_color_( double value ) const
{
    assert( value >= 0.0 && value <= 1.0 );
    assert( palette_.size() > 0 );

    // Bring value into the correct range within the palette size.
    auto const val_entry = static_cast<double>( palette_.size() - 1 ) * value;
    assert( val_entry >= 0 );
    assert( val_entry < static_cast<double>( palette_.size() ));

    // Get the values at the two colors next to the entry.
    // auto const low_value  = min_ + (( std::floor( val_entry ) / max_entry ) * ( max_ - min_ ));
    // auto const high_value = min_ + (( std::ceil(  val_entry ) / max_entry ) * ( max_ - min_ ));
    // auto const interval_len = ( max_ - min_ ) / max_entry;

    // Get the color indices next to our chosen value.
    auto const low_idx   = static_cast<size_t>( std::floor( val_entry ) );
    auto const high_idx  = static_cast<size_t>( std::ceil(  val_entry ) );
    if( low_idx == high_idx ) {
        return get_entry_( low_idx );
    }
    assert( low_idx < high_idx );

    // Get the fraction between the two entries that our value sits on.
    auto const fraction = val_entry - std::floor( val_entry );
    assert( fraction >= 0.0 && fraction <= 1.0 );

    // Return the interpolated result.
    return interpolate( get_entry_( low_idx ), get_entry_( high_idx ), fraction );
}


} // namespace utils
} // namespace genesis
