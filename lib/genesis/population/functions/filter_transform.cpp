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
 * @ingroup population
 */

#include "genesis/population/functions/filter_transform.hpp"

#include "genesis/population/functions/base_counts.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     General Filtering
// =================================================================================================

// =================================================================================================
//     Filtering by count
// =================================================================================================

// =================================================================================================
//     Transforming by count
// =================================================================================================

void transform_by_min_count( BaseCounts& sample, size_t min_count )
{
    // Reset counts if needed, according to min count setting.
    if( sample.a_count < min_count ) {
        sample.a_count = 0;
    }
    if( sample.c_count < min_count ) {
        sample.c_count = 0;
    }
    if( sample.g_count < min_count ) {
        sample.g_count = 0;
    }
    if( sample.t_count < min_count ) {
        sample.t_count = 0;
    }
}

void transform_by_min_count( Variant& variant, size_t min_count )
{
    for( auto& sample : variant.samples ) {
        transform_by_min_count( sample, min_count );
    }
}

void transform_by_max_count( BaseCounts& sample, size_t max_count )
{
    // Reset counts if needed, according to max count setting.
    if( sample.a_count > max_count ) {
        sample.a_count = 0;
    }
    if( sample.c_count > max_count ) {
        sample.c_count = 0;
    }
    if( sample.g_count > max_count ) {
        sample.g_count = 0;
    }
    if( sample.t_count > max_count ) {
        sample.t_count = 0;
    }
}

void transform_by_max_count( Variant& variant, size_t max_count )
{
    for( auto& sample : variant.samples ) {
        transform_by_max_count( sample, max_count );
    }
}

void transform_by_min_max_count( BaseCounts& sample, size_t min_count, size_t max_count )
{
    // We could just call transform_by_min_count() and transform_by_max_count() here,
    // but doing that would require more branching, so let's do a little code duplication.
    if( sample.a_count < min_count || sample.a_count > max_count ) {
        sample.a_count = 0;
    }
    if( sample.c_count < min_count || sample.c_count > max_count ) {
        sample.c_count = 0;
    }
    if( sample.g_count < min_count || sample.g_count > max_count ) {
        sample.g_count = 0;
    }
    if( sample.t_count < min_count || sample.t_count > max_count ) {
        sample.t_count = 0;
    }
}

void transform_by_min_max_count( Variant& variant, size_t min_count, size_t max_count )
{
    for( auto& sample : variant.samples ) {
        transform_by_min_max_count( sample, min_count, max_count );
    }
}

} // namespace population
} // namespace genesis
