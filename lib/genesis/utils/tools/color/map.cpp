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

#include "genesis/utils/tools/color/map.hpp"

#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/tools/color/normalization.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Palette
// =================================================================================================

std::vector<Color> ColorMap::color_list( size_t n ) const
{
    // Result.
    std::vector<Color> result;
    result.reserve( n );

    // Boundary checks.
    if( n == 0 ) {
        return palette_;
    }
    if( n == 1 ) {
        return { operator()( 0.5 ) };
    }

    for( size_t i = 0; i < n; ++i ) {
        auto const pos = static_cast<double>( i ) / static_cast<double>( n - 1 );
        result.push_back( operator()( pos ));
    }
    return result;
}

Color ColorMap::operator() ( double value ) const
{
    // Extreme cases check.
    if( palette_.size() == 0 ) {
        return Color( 0.0, 0.0, 0.0 );
    }
    if( palette_.size() == 1 ) {
        return palette_[0];
    }

    // Boundary checks.
    if( ! std::isfinite( value ) ) {
        return mask_color_;
    }
    if( clip_under_ ) {
        value = std::max( 0.0, value );
    }
    if( clip_over_ ) {
        value = std::min( value, 1.0 );
    }
    if( value < 0.0 ) {
        return under_color_;
    }
    if( value > 1.0 ) {
        return over_color_;
    }

    // Check again.
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

std::vector<Color> ColorMap::operator() ( std::vector<double> const& values ) const
{
    using it_type = std::vector<double>::const_iterator;
    return operator()<it_type>( values.begin(), values.end() );
}

Color ColorMap::operator() ( ColorNormalization const& norm, double value ) const
{
    return operator()( norm( value ) );
}

std::vector<Color> ColorMap::operator() (
    ColorNormalization const& norm, std::vector<double> const& values
) const {
    using it_type = std::vector<double>::const_iterator;
    return operator()<it_type>( norm, values.begin(), values.end() );
}


} // namespace utils
} // namespace genesis
