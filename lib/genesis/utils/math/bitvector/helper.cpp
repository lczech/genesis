/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector/helper.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Helper
// =================================================================================================

std::vector<bool> make_bool_vector_from_indices( std::vector<size_t> const& indices, size_t size )
{
    // Get the largest element of the vector. If it's empty, we return an all-false vector.
    auto max_it = std::max_element( indices.begin(), indices.end() );
    if( max_it == indices.end() ) {
        return std::vector<bool>( size, false );
    }
    size_t target_size = *max_it + 1;
    if( size > 0 ) {
        if( target_size > size ) {
            throw std::invalid_argument(
                "Cannot use make_bool_vector_from_indices() with size " + std::to_string( size ) +
                " that is smaller than required to include the larged index " +
                std::to_string( *max_it ) + " in the list of indices (zero-based)."
            );
        }
        target_size = size;
    }

    // Fill a bool vector, setting all positions to true
    // that are indicated by the indices, pun intended.
    auto result = std::vector<bool>( target_size, false );
    for( auto const& idx : indices ) {
        assert( idx < result.size() );
        result[idx] = true;
    }
    return result;
}

} // namespace utils
} // namespace genesis
