/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/population/genome_locus_set.hpp"

#include "genesis/utils/math/bitvector/operators.hpp"

#include <unordered_set>

namespace genesis {
namespace population {

// =================================================================================================
//     Add
// =================================================================================================

void GenomeLocusSet::add(
    std::string const& chromosome,
    size_t start,
    size_t end
) {
    // Check chromosome.
    if( chromosome.empty() ) {
        throw std::invalid_argument(
            "Cannot add region to GenomeLocusSet with empty chromosome name, "
            "as this denotes an invalid chromosome."
        );
    }

    // Check positions.
    // The start and end are also checked in the interval tree, but let's do it here
    // so that the error message is nicer in case they are wrong.
    if( start > end ) {
        throw std::invalid_argument(
            "Cannot add region to GenomeLocusSet with start == " +
            std::to_string( start ) + " > end == " + std::to_string( end )
        );
    }
    if(( start == 0 ) ^ ( end == 0 )) {
        throw std::invalid_argument(
            "Cannot add region to GenomeLocusSet with either start == 0 or end == 0, "
            "but not both, as we use 1-base indexing, with both being 0 being interpreted "
            "as the special case of denoting the whole chromosome. "
            "Hence either both start and end have to be 0, or neither."
        );
    }
    assert( start <= end );

    // Create and extend the bitvector as needed. For now, we double its size,
    // for amortization in the long run. Might find a better strategy later.
    auto& bv = locus_map_[ chromosome ];
    if( bv.empty() ) {
        bv = utils::Bitvector( end + 1 );
    } else if( bv.size() < end + 1 ) {
        if( bv.size() * 2 < end + 1 ) {
            bv = utils::Bitvector( end + 1, bv );
        } else {
            bv = utils::Bitvector( bv.size() * 2, bv );
        }
    }
    assert( bv.size() >= end + 1 );

    // Now set all bits in between the two positions, inclusive.
    bv.set( start, end + 1 );
    // for( size_t i = start; i <= end; ++i ) {
    //     bv.set( i );
    // }
}

void GenomeLocusSet::add( std::string const& chromosome, utils::Bitvector const& values )
{
    add( chromosome, utils::Bitvector( values ));
}

void GenomeLocusSet::add( std::string const& chromosome, utils::Bitvector&& values )
{
    // Checks.
    if( chromosome.empty() ) {
        throw std::invalid_argument(
            "Cannot add region to GenomeLocusSet with empty chromosome name, "
            "as this denotes an invalid chromosome."
        );
    }
    if( locus_map_.count( chromosome ) > 0 ) {
        throw std::invalid_argument(
            "Cannot add region via chromosome and Bitvector, as chromosome \"" + chromosome +
            "\" is already present in the GenomeLocusSet."
        );
    }
    if( values.empty() ) {
        throw std::invalid_argument(
            "Cannot add region via chromosome and Bitvector, as given Bitvector is empty."
        );
    }
    assert( values.size() > 0 );
    if( values.size() == 1 && !values.get(0) ) {
        throw std::invalid_argument(
            "Cannot add region via chromosome and Bitvector, as given Bitvector has [0]==false."
        );
    }

    // Set.
    locus_map_[ chromosome ] = values;
}

// =================================================================================================
//     Operations
// =================================================================================================

void GenomeLocusSet::set_intersect( GenomeLocusSet const& rhs )
{
    using namespace genesis::utils;

    // Shorthand for clarity and for ease of refactoring
    // if we need to make this a free function later.
    auto& lhs = *this;

    // Make a list of all chromosomes in the current list.
    // The ones that are not in rhs or end up empty will be deleted later.
    std::unordered_set<std::string> chrs_to_delete;
    for( auto const& chr : lhs.locus_map_ ) {
        assert( chrs_to_delete.count( chr.first ) == 0 );
        chrs_to_delete.insert( chr.first );
    }

    // Go through all chromosomes of the rhs.
    for( auto const& chr : rhs.locus_map_ ) {
        // Skip chromosomes that are not in the current list. The intersection of a chromosome
        // that is only in the rhs but not in lhs is empty anyway, so nothing to do.
        if( ! chrs_to_delete.count( chr.first ) ) {
            assert( ! lhs.has_chromosome( chr.first ));
            continue;
        }
        assert( lhs.has_chromosome( chr.first ));
        assert( rhs.has_chromosome( chr.first ));

        // Whenever a bitvector is set for a chromosome, we give it at least size 1,
        // so we can at least always access the bit 0. Assert this.
        assert( lhs.locus_map_.at( chr.first ).size() > 0 );
        assert( rhs.locus_map_.at( chr.first ).size() > 0 );

        // Get some shorthands, for readability, and a bit for speed as well.
        auto&       lhs_bits  = lhs.locus_map_.at( chr.first );
        auto const& rhs_bits  = rhs.locus_map_.at( chr.first );
        auto const  lhs_bit_0 = lhs_bits.get( 0 );
        auto const  rhs_bit_0 = rhs_bits.get( 0 );
        assert( &rhs_bits == &chr.second );

        // We found a chromosome that is in both lists, let's process it.
        if( lhs_bit_0 && rhs_bit_0 ) {
            // If both have the full chromosome, we use this opportunity to shorten the vector.
            // Make a new bitvector that just marks this chromosome.
            lhs_bits = Bitvector( 1, true );
        } else if( lhs_bit_0 && !rhs_bit_0 ) {
            // lhs uses the whole chromosome, rhs not. The intersection of this is rhs.
            lhs_bits = rhs_bits;
            assert( lhs_bits.size() > 0 );
            assert( !lhs_bits.get(0) );
        } else if( !lhs_bit_0 && rhs_bit_0 ) {
            // lhs does not use the whole chromosome, but rhs does.
            // The intersection of this is just lhs again, so nothing to do here.
            assert( lhs_bits.size() > 0 );
            assert( !lhs_bits.get(0) );
        } else {
            assert( !lhs_bit_0 && !rhs_bit_0 );
            // Actual intersection of the two vectors.
            // We use the smaller one as our target size, hence `use_larger == false` here.
            // Everything behind those positions will end up false anyway when intersecting.
            lhs_bits = bitwise_and( lhs_bits, rhs_bits, false );
            assert( lhs_bits.size() > 0 );
            assert( !lhs_bits.get(0) );
        }

        // If the result has any positions set, this is still a chromsome that we want to keep,
        // so remove it from the to-delete list. If all its bits are 0, we have eliminated
        // all positions from the filter, so we might as well delete the whole vector; in that
        // case, we simply keept it in the to-delete list and then it gets removed below.
        if( lhs_bits.count() > 0 ) {
            chrs_to_delete.erase( chr.first );
        }
    }

    // Delete all chromosomes from lhs that were not also in rhs, or ended up empty.
    for( auto const& chr : chrs_to_delete ) {
        assert( lhs.has_chromosome( chr ));
        lhs.locus_map_.erase( chr );
    }
}

void GenomeLocusSet::set_union( GenomeLocusSet const& rhs )
{
    // Remark on the name: `union` is a C++ keyword, so we cannot name the function just that...
    using namespace genesis::utils;

    // Shorthand for clarity and for ease of refactoring
    // if we need to make this a free function later.
    auto& lhs = *this;

    // Go through all chromosomes of the rhs.
    for( auto const& chr : rhs.locus_map_ ) {
        // Shorthands. We cannot access the lhs positions yet, as they might not exist.
        auto const& rhs_bits = rhs.locus_map_.at( chr.first );
        assert( &rhs_bits == &chr.second );

        if( lhs.has_chromosome( chr.first )) {
            assert( lhs.locus_map_.count( chr.first ) > 0 );
            auto& lhs_bits = lhs.locus_map_.at( chr.first );
            if( lhs_bits.get(0) || rhs_bits.get(0) ) {
                // We check the special 0 bit case here, meaning that if either of the vectors
                // has the bit set, we shorten the vector here, to save some memory.
                lhs_bits = Bitvector( 1, true );
            } else {
                // Compute actual union of the two vectors.
                // Here, we use `use_larger == true`, so that the longer vector is used,
                // with all its bits that are not in the other one.
                lhs_bits = bitwise_or( lhs_bits, rhs_bits, true );
            }
        } else {
            // lhs does not have the chromosome, so we just copy. We here use the array
            // operator to create the entry in the map first. We also again do a special case
            // and shorten all-chromosome vectors here, while we are at it.
            assert( lhs.locus_map_.count( chr.first ) == 0 );
            auto& lhs_bits = lhs.locus_map_[ chr.first ];
            if( rhs_bits.get(0) ) {
                lhs_bits = Bitvector( 1, true );
            } else {
                lhs_bits = rhs_bits;
            }
        }
    }
}

} // namespace population
} // namespace genesis
