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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "genesis/population/genome_region.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Region List
// =================================================================================================

void GenomeRegionList::add( GenomeRegion const& region )
{
    // Sanity check
    if( region.start >= region.end ) {
        throw std::runtime_error( "Invalid GenomeRegion with start >= end" );
    }

    // Find the position in the already given regions at which we need to insert the new one.
    auto const pos = std::lower_bound( regions_.begin(), regions_.end(), region, [](
        GenomeRegion const& lhs, GenomeRegion const& rhs
    ){
        // The comp operator is called with `lhs` being the iterated elements of `regions_`,
        // and rhs being the new `region` to be inserted. The operator is 'less than', which we here
        // want to mean: if the chromosomes differ, we sort by chromosome. Next, on the same
        // chromosome, `lhs` is less than `rhs` iff it comes completely before `rhs`.
        // Any overlap between the two is forbidden, but we still stop in that case and then later
        // check for that. So, in all cases where the end of `lhs` is after the start of `rhs`,
        // we want to stop.
        // Alternative way of thinking about this: In the search, move to the next region in the list
        // as long as the condition is true, that is, as long as (on the same chromosome) the
        // region in the list is completely (start and end) before the given new region.
        return
            lhs.chromosome < rhs.chromosome ||
            ( lhs.chromosome == rhs.chromosome && lhs.end <= rhs.start )
        ;
    });

    // Check for overlap. We only need to check one condition here, as we already implicitly
    // checked the other above when finding the insertion position. But let's assert this.
    if( pos != regions_.end() && pos->start < region.end ) {
        assert( region.chromosome == pos->chromosome );
        assert( pos->end > region.start );
        throw std::runtime_error(
            "Cannot add GenomeRegion to GenomeRegionList: Invalid GenomeRegion [ " +
            std::to_string( region.start ) + ", " + std::to_string( region.end ) +
            " ) that overlaps with existing GenomeRegion [ " + std::to_string( pos->start ) +
            ", " + std::to_string( pos->end ) + " ) on chromosome " + region.chromosome
        );
    }

    // If the above check passed, let's assert that the region is in front of pos,
    // at least as long as we found a position and that position is on the same chromosome.
    assert( pos == regions_.end() || region.chromosome != pos->chromosome || region.end <= pos->start );

    // Finally, insert the new element.
    regions_.insert( pos, region );

    // Set the cache to a valid iterator.
    find_cache_ = regions_.end();

    // Assert that all regions are in order.
    assert(
        [&](){
            std::string chrom;
            size_t pos = 0;
            for( auto const& r : regions_ ) {
                if( r.chromosome != chrom ) {
                    chrom = r.chromosome;
                    pos = 0;
                }
                if( r.start < pos || r.end <= pos ) {
                    return false;
                }
                pos = r.end;
            }
            return true;
        }()
    );
}

void GenomeRegionList::add( std::string const& chromosome, size_t start, size_t end )
{
    add( GenomeRegion( chromosome, start, end ));
}

bool GenomeRegionList::is_covered( std::string const& chromosome, size_t position ) const
{
    return find( chromosome, position ) != regions_.end();
}

GenomeRegionList::const_iterator GenomeRegionList::find(
    std::string const& chromosome, size_t position
) const {

    // We use a cache for the region that was used last time.
    // In many cases, we query adjacent positions, and hence this should yield quite speedup.
    if(
        find_cache_ != regions_.end() && find_cache_->chromosome == chromosome &&
        find_cache_->start <= position && position < find_cache_->end
    ) {
        return find_cache_;
    }

    // Find the chromosome and position using binary search.
    // We are looking for the first interval that is not completely smaller (start and end)
    // than the given position. Think of the following lambda as stating "return true as long
    // as we want to move on to the next region in the list".
    auto const pos = std::lower_bound( regions_.begin(), regions_.end(), position, [&](
        GenomeRegion const& lhs, size_t position
    ){
        assert( lhs.start < lhs.end );
        return
            lhs.chromosome < chromosome ||
            ( lhs.chromosome == chromosome && lhs.end <= position )
        ;
    });

    // Nothing found that fits
    if( pos == regions_.end() ) {
        return regions_.end();
    }

    // Found the first interval not smaller than the given position.
    // Let's test whether position is within this interval.
    assert( pos->chromosome == chromosome );
    assert( position < pos->end );
    if( position >= pos->start ) {
        find_cache_ = pos;
        return pos;
    }
    return regions_.end();

    // Simple version without binary search:
    // for( auto it = regions_.cbegin(); it != regions_.end(); ++it ) {
    //     if( it->chromosome != chromosome ) {
    //         continue;
    //     }
    //     if( position >= it->start && position < it->end ) {
    //         return it;
    //     }
    // }
    // return regions_.end();
}

} // namespace population
} // namespace genesis
