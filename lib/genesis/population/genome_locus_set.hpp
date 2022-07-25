#ifndef GENESIS_POPULATION_GENOME_LOCUS_SET_H_
#define GENESIS_POPULATION_GENOME_LOCUS_SET_H_

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

#include <cassert>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "genesis/population/genome_locus.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/genome_region_list.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Locus Set
// =================================================================================================

/**
 * @brief List of positions/coordinates in a genome, for each chromosome.
 *
 * The data structure stores a list of genome positions/coordinates, and allows fast querying,
 * that is, whether a certain position on a chromosome is stored here. Internally, we use a
 * @link genesis::utils::Bitvector Bitvector@endlink for each chromosome, marking its positions
 * as set or not set.
 *
 * Positions are 1-based. We also offer the special case to add a whole chromosome, in which case
 * the is_covered() function will return `true` for all positions on that chromosome (without
 * checking that the position is in fact part of the chromosome - as we do not use information
 * on the lengths of chromosomes in this class). We use position `0` to mark this special
 * whole-chromosome case - be aware of that when adding positions to the list.
 * See also GenomeRegionList, GenomeLocus and GenomeRegion for related classes that have the same
 * special cases.
 *
 * @see GenomeLocus
 * @see GenomeRegion
 * @see GenomeRegionList
 */
class GenomeLocusSet
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GenomeLocusSet() = default;
    ~GenomeLocusSet() = default;

    GenomeLocusSet( GenomeLocusSet const& ) = default;
    GenomeLocusSet( GenomeLocusSet&& )      = default;

    GenomeLocusSet& operator= ( GenomeLocusSet const& ) = default;
    GenomeLocusSet& operator= ( GenomeLocusSet&& )      = default;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    // -------------------------------------------
    //         Add Manually
    // -------------------------------------------

    /**
     * @brief Add a whole chromosome to the list, so that all its positions are considered to be
     * covered.
     */
    void add( std::string const& chromosome )
    {
        // We use the special value 0 to denote that we want the whole chromosome.
        add( chromosome, 0, 0 );
    }

    /**
     * @brief Add a region to the list, given its chromosome, and @p start and @p end positions.
     *
     * All positions between @p start and @p end are set.
     * The @p chromosome cannot be empty, and we expect @p start < @p end (or both equal to 0, for
     * the special case denoting that the whole chromosome is to be considered covered).
     * Both @p start and @p end are 1-based, and inclusive, that is, the interval between them is
     * closed.
     */
    void add(
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
        auto& bv = positions_[ chromosome ];
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
        for( size_t i = start; i <= end; ++i ) {
            bv.set( i );
        }
    }

    // -------------------------------------------
    //         Add Locus
    // -------------------------------------------

    /**
     * @brief Add a single GenomeLocus, that is, an interval covering one position on a chromosome.
     */
    void add( GenomeLocus const& locus )
    {
        add( locus.chromosome, locus.position, locus.position );
    }

    /**
     * @brief Add an interval between two @link GenomeLocus GenomeLoci@endlink on the same chromosome.
     */
    void add( GenomeLocus const& start, GenomeLocus const& end )
    {
        if( start.chromosome != end.chromosome ) {
            throw std::invalid_argument(
                "Cannot use two GenomeLocus instances with different chromosomes ( start == \"" +
                start.chromosome + "\", end == \"" + end.chromosome + "\") as an entry in a "
                "GenomeLocusSet."
            );
        }
        add( start.chromosome, start.position, end.position );
    }

    // -------------------------------------------
    //         Add Region
    // -------------------------------------------

    /**
     * @brief Add a GenomeRegion to the list.
     *
     * This function ensures that regions are valid (`start < end`).
     */
    void add( GenomeRegion const& region )
    {
        add( region.chromosome, region.start, region.end );
    }

    // -------------------------------------------
    //         Add Region List
    // -------------------------------------------

    /**
     * @brief Add a complete GenomeRegionList to this list.
     */
    void add( GenomeRegionList const& list )
    {
        for( auto const& chr : list.chromosome_map() ) {
            for( auto const& interval : chr.second ) {
                add( chr.first, interval.low(), interval.high() );
            }
        }
    }

    // -------------------------------------------
    //         Clear
    // -------------------------------------------

    /**
     * @brief Remove all stored regions from all chromosomes.
     */
    void clear()
    {
        positions_.clear();
    }

    /**
     * @brief Remove the regions of the specified @p chromosome.
     */
    void clear( std::string const& chromosome )
    {
        if( positions_.count( chromosome ) == 0 ) {
            throw std::invalid_argument(
                "Chromosome name \"" + chromosome + "\" not found in GenomeLocusSet"
            );
        }
        positions_.erase( chromosome );
    }

    // -------------------------------------------------------------------------
    //     Operations
    // -------------------------------------------------------------------------

    /**
     * @brief Compute the intersection with another GenomeLocusSet @p rhs.
     *
     * Any chromosomes that end up having no positions covered are removed.
     */
    void set_intersect( GenomeLocusSet const& rhs )
    {
        using namespace genesis::utils;

        // Shorthand for clarity and for ease of refactoring
        // if we need to make this a free function later.
        auto& lhs = *this;

        // Make a list of all chromosomes in the current list.
        // The ones that are not in rhs or end up empty will be deleted later.
        std::unordered_set<std::string> chrs_to_delete;
        for( auto const& chr : lhs.positions_ ) {
            assert( chrs_to_delete.count( chr.first ) == 0 );
            chrs_to_delete.insert( chr.first );
        }

        // Go through all chromosomes of the rhs.
        for( auto const& chr : rhs.positions_ ) {
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
            assert( lhs.positions_.at( chr.first ).size() > 0 );
            assert( rhs.positions_.at( chr.first ).size() > 0 );

            // Get some shorthands, for readability, and a bit for speed as well.
            auto&       lhs_bits  = lhs.positions_.at( chr.first );
            auto const& rhs_bits  = rhs.positions_.at( chr.first );
            auto const  lhs_bit_0 = lhs_bits.get( 0 );
            auto const  rhs_bit_0 = rhs_bits.get( 0 );
            assert( &rhs_bits == &chr.second );

            // We found a chromosome that is in both lists, let's process it.
            if( lhs_bit_0 && rhs_bit_0 ) {
                // If both have the full chromosome, we use this opportunity to shorten the vector.
                // Make a new bitvector that just marks this chromosome.
                lhs_bits = Bitvector( 1, true );
            } else if( lhs_bit_0 && !rhs_bit_0 ) {
                // lhs uses the whole chromosoe, rhs not. The intersection of this is rhs.
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
            lhs.positions_.erase( chr );
        }
    }

    /**
     * @brief Compute the union with another GenomeLocusSet @p rhs.
     */
    void set_union( GenomeLocusSet const& rhs )
    {
        // Remark on the name: `union` is a C++ keyword, so we cannot name the function just that...
        using namespace genesis::utils;

        // Shorthand for clarity and for ease of refactoring
        // if we need to make this a free function later.
        auto& lhs = *this;

        // Go through all chromosomes of the rhs.
        for( auto const& chr : rhs.positions_ ) {
            // Shorthands. We cannot access the lhs positions yet, as they might not exist.
            auto const& rhs_bits = rhs.positions_.at( chr.first );
            assert( &rhs_bits == &chr.second );

            if( lhs.has_chromosome( chr.first )) {
                assert( lhs.positions_.count( chr.first ) > 0 );
                auto& lhs_bits = lhs.positions_.at( chr.first );
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
                assert( lhs.positions_.count( chr.first ) == 0 );
                auto& lhs_bits = lhs.positions_[ chr.first ];
                if( rhs_bits.get(0) ) {
                    lhs_bits = Bitvector( 1, true );
                } else {
                    lhs_bits = rhs_bits;
                }
            }
        }
    }

    // -------------------------------------------------------------------------
    //     Locus Queries
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether a given position on a chromosome is part of any of the regions stored.
     */
    bool is_covered( std::string const& chromosome, size_t position ) const
    {
        // Using find(), so we only have to search in the map once, for speed.
        auto const it = positions_.find( chromosome );
        if( it == positions_.end() ) {
            return false;
        }
        auto const& bv = it->second;
        assert( ! bv.empty() );

        // If the chromosome in our interval tree has the 0th bit set,
        // we consider that as having the whole chromosome covered.
        if( bv.get( 0 ) ) {
            return true;
        }

        // If the above is not the case, check the actual position.
        // If the position is outside of the bitvector, it is not covered, obviously.
        if( position >= bv.size() ) {
            return false;
        }
        return bv.get( position );
    }

    /**
     * @brief Return whether a whole chromosome is covered.
     */
    bool is_covered( std::string const& chromosome ) const
    {
        auto const it = positions_.find( chromosome );
        if( it == positions_.end() ) {
            return false;
        }
        auto const& bv = it->second;
        assert( ! bv.empty() );
        return bv.get( 0 );
    }

    // -------------------------------------------------------------------------
    //     Chromosome Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether there are chromosomes with positions stored.
     */
    bool empty() const
    {
        return positions_.empty();
    }

    /**
     * @brief Return the number of chromosomes for which there are positions stored.
     */
    size_t chromosome_count() const
    {
        return positions_.size();
    }

    /**
     * @brief Get a list of all stored chromosome names.
     */
    std::vector<std::string> chromosome_names() const
    {
        std::vector<std::string> result;
        for( auto const& p : positions_ ) {
            result.push_back( p.first );
        }
        return result;
    }

    /**
     * @brief Return whether a chromosome is stored.
     */
    bool has_chromosome( std::string const& chromosome ) const
    {
        return positions_.count( chromosome ) > 0;
    }

    /**
     * @brief For a given chromosome, return the
     * @link genesis::utils::Bitvector Bitvector@endlink that stores its positions.
     */
    utils::Bitvector const& chromosome_positions( std::string const& chromosome ) const
    {
        return positions_.at( chromosome );
    }

    /**
     * @brief For a given chromosome, return the
     * @link genesis::utils::Bitvector Bitvector@endlink that stores its positions.
     *
     * Note that this exposes the underlying container, and hence has to be used with caution.
     * In particular position 0 is considered special: Any chromosome for which we have stored an
     * interval that covers 0 is considered to be fully covered for all its positions.
     */
    utils::Bitvector& chromosome_positions( std::string const& chromosome )
    {
        return positions_.at( chromosome );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Map from chromosome names to bitvectors representing which positions are in (true)
    // and out (false). Note that position 0 is special; if set, it means that we consider
    // the whole chromsome as covered.
    std::map<std::string, utils::Bitvector> positions_;

};

} // namespace population
} // namespace genesis

#endif // include guard
