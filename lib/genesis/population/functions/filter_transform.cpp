/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include <cassert>
#include <iostream>

namespace genesis {
namespace population {

// =================================================================================================
//     Filter by Status
// =================================================================================================

bool filter_by_status(
    std::function<bool(BaseCountsStatus const&)> predicate,
    Variant const& variant,
    SampleFilterType type,
    size_t min_coverage,
    size_t max_coverage,
    size_t min_count,
    bool tolerate_deletions
) {
    // kMerge: Merge first, then stat.
    if( type == SampleFilterType::kMerge ) {
        auto const smp = merge( variant.samples );
        auto const stat = status( smp, min_coverage, max_coverage, min_count, tolerate_deletions );
        return predicate( stat );
    }

    // kConjunction or kDisjunction: Stat individually, then logical combination.
    size_t true_cnt = 0;
    for( auto const& smp : variant.samples ) {
        auto const stat = status( smp, min_coverage, max_coverage, min_count, tolerate_deletions );
        true_cnt += static_cast<size_t>( predicate( stat ));
    }
    if( type == SampleFilterType::kConjunction ) {
        return true_cnt == variant.samples.size();
    } else if( type == SampleFilterType::kDisjunction ) {
        return true_cnt > 0;
    }
    throw std::invalid_argument( "Invalid SampleFilterType." );
}

// =================================================================================================
//     Transform by Count
// =================================================================================================

void transform_zero_out_by_min_count( BaseCounts& sample, size_t min_count )
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

void transform_zero_out_by_min_count( Variant& variant, size_t min_count )
{
    for( auto& sample : variant.samples ) {
        transform_zero_out_by_min_count( sample, min_count );
    }
}

void transform_zero_out_by_max_count( BaseCounts& sample, size_t max_count )
{
    if( max_count == 0 ) {
        return;
    }

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

void transform_zero_out_by_max_count( Variant& variant, size_t max_count )
{
    for( auto& sample : variant.samples ) {
        transform_zero_out_by_max_count( sample, max_count );
    }
}

void transform_zero_out_by_min_max_count( BaseCounts& sample, size_t min_count, size_t max_count )
{
    // We could just call transform_zero_out_by_min_count() and transform_zero_out_by_max_count()
    // here, but doing that would require more branching, so let's do a little code duplication.
    if( sample.a_count < min_count || ( max_count > 0 && sample.a_count > max_count )) {
        sample.a_count = 0;
    }
    if( sample.c_count < min_count || ( max_count > 0 && sample.c_count > max_count )) {
        sample.c_count = 0;
    }
    if( sample.g_count < min_count || ( max_count > 0 && sample.g_count > max_count )) {
        sample.g_count = 0;
    }
    if( sample.t_count < min_count || ( max_count > 0 && sample.t_count > max_count )) {
        sample.t_count = 0;
    }
}

void transform_zero_out_by_min_max_count( Variant& variant, size_t min_count, size_t max_count )
{
    for( auto& sample : variant.samples ) {
        transform_zero_out_by_min_max_count( sample, min_count, max_count );
    }
}

} // namespace population
} // namespace genesis
