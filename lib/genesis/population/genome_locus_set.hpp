#ifndef GENESIS_POPULATION_GENOME_LOCUS_SET_H_
#define GENESIS_POPULATION_GENOME_LOCUS_SET_H_

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "genesis/population/genome_locus.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/genome_region_list.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/utils/math/bitvector.hpp"

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
 * checking that the position is in fact within the length of the chromosome - as we do not use
 * information on the lengths of chromosomes in this class). We use position `0` to mark this
 * special whole-chromosome case - be aware of that when adding positions to the list.
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
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using key_type    = std::string;
    using mapped_type = utils::Bitvector;
    using value_type  = std::pair<std::string const, utils::Bitvector>;

    using reference       = value_type&;
    using const_reference = value_type const&;

    using iterator       = typename std::unordered_map<std::string, utils::Bitvector>::iterator;
    using const_iterator = typename std::unordered_map<std::string, utils::Bitvector>::const_iterator;

    /**
     * @brief Position value to indicate that next_covered did not find any covered position.
     */
    static const size_t npos = std::numeric_limits<size_t>::max();
    static_assert(
        npos == utils::Bitvector::npos,
        "Differing definitions of GenomeLocusSet::npos and Bitvector::npos"
    );

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
     * @brief Add a single locus (position, coordinate) to the list.
     */
    void add( std::string const& chromosome, size_t position )
    {
        add( chromosome, position, position );
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
    );

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
    //         Add Bitvector
    // -------------------------------------------

    /**
     * @brief Add a chromosome to the list, given the full Bitvector representation of loci.
     *
     * This assumes that the data of the Bitvector has been assembled according to the
     * specifications of this class, i.e., respecting the special role of the 0th bit.
     */
    void add( std::string const& chromosome, utils::Bitvector const& values );

    /**
     * @copydoc add( std::string const&, utils::Bitvector const& )
     */
    void add( std::string const& chromosome, utils::Bitvector&& values );

    // -------------------------------------------
    //         Clear
    // -------------------------------------------

    /**
     * @brief Remove all stored regions from all chromosomes.
     */
    void clear()
    {
        locus_map_.clear();
    }

    /**
     * @brief Remove the regions of the specified @p chromosome.
     */
    void clear( std::string const& chromosome )
    {
        if( locus_map_.count( chromosome ) == 0 ) {
            throw std::invalid_argument(
                "Chromosome name \"" + chromosome + "\" not found in GenomeLocusSet"
            );
        }
        locus_map_.erase( chromosome );
    }

    // -------------------------------------------------------------------------
    //     Operations
    // -------------------------------------------------------------------------

    /**
     * @brief Compute the intersection with another GenomeLocusSet @p rhs.
     *
     * Any chromosomes that end up having no positions covered are removed.
     */
    void set_intersect( GenomeLocusSet const& rhs );

    /**
     * @brief Compute the union with another GenomeLocusSet @p rhs.
     */
    void set_union( GenomeLocusSet const& rhs );

    /**
     * @brief Invert all chromosome regions.
     *
     * This needs a means of getting the length of each chromosome, in order to know how many
     * positions towards the end of each chromosome need to be inverted. If the given
     * @p sequence_dict does not contain a chromosome that is present in this set here,
     * or the set contains set positions beyond the dict, an exception is thrown.
     */
    void invert( sequence::SequenceDict const& sequence_dict );

    // -------------------------------------------------------------------------
    //     Locus Covered
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether a given position on the provided @p bitvector is covered.
     *
     * @see is_covered( std::string const& chromosome, size_t position ) const
     *
     * This overload of the function accepts a Bitvector directly, without checking that it
     * belongs to any of the chromosomes stored (hence, it is a static function). The Bitvector
     * is expected to follow the convention of that class, that is, bit 0 is used to indicate that
     * the whole chromosome is covered, and all other bits correspond to 1-based positions.
     * This can for instance be used in combination with chromosome_positions().
     */
    static bool is_covered( utils::Bitvector const& bitvector, size_t position )
    {
        // Boundary check.
        if( bitvector.empty() ) {
            throw std::invalid_argument(
                "GenomeLocusSet::is_covered( Bitvector const&, size_t ) called with empty Bitvector"
            );
        }
        // if( position == 0 ) {
        //     throw std::invalid_argument(
        //         "GenomeLocusSet::is_covered( Bitvector const&, size_t ) called with position==0"
        //     );
        // }

        // If the chromosome has the 0th bit set, the whole chromosome is covered.
        if( bitvector.get( 0 ) ) {
            return true;
        }

        // If the above is not the case, check the actual position.
        // If the position is outside of the bitvector, it is not covered, obviously.
        if( position >= bitvector.size() ) {
            return false;
        }
        return bitvector.get( position );
    }

    /**
     * @brief Return whether a given position on the provided iterator is covered.
     *
     * @see is_covered( std::string const& chromosome, size_t position ) const
     *
     * This overload is meant to work in combination with find(), to avoid having to look up
     * the chromosome every time, in cases where this does not change. The function does not check
     * that the iterator is valid - in fact, it is static, and hence not tied to any object.
     */
    static bool is_covered( const_iterator const& it, size_t position )
    {
        return is_covered( it->second, position );
    }

    /**
     * @brief Return whether a given position on a chromosome is part of any of the regions stored.
     *
     * Note that @p position is 1-based.
     */
    bool is_covered( std::string const& chromosome, size_t position ) const
    {
        // Using find(), so we only have to search in the map once, for speed.
        auto const it = locus_map_.find( chromosome );
        if( it == locus_map_.end() ) {
            return false;
        }
        auto const& bv = it->second;
        assert( ! bv.empty() );
        return is_covered( bv, position );
    }

    /**
     * @brief Return whether a whole chromosome is covered.
     *
     * If the special 0th bit is set, we take that as the whole chromosome being covered, i.e.,
     * in cases where no individual positions were specified.
     */
    bool is_covered( std::string const& chromosome ) const
    {
        auto const it = locus_map_.find( chromosome );
        if( it == locus_map_.end() ) {
            return false;
        }
        auto const& bv = it->second;
        assert( ! bv.empty() );
        return bv.get( 0 );
    }

    // -------------------------------------------------------------------------
    //     Any Covered Locus
    // -------------------------------------------------------------------------

    /**
     * @brief Return if the given @p chromosome has any loci covered.
     */
    bool any_covered( std::string const& chromosome ) const
    {
        auto const it = locus_map_.find( chromosome );
        if( it == locus_map_.end() ) {
            return false;
        }
        auto const& bv = it->second;
        assert( ! bv.empty() );

        // We do not need to to an extra check for position 0 here.
        // If it is true, then so will be the result.
        return bv.any_set();
    }

    // -------------------------------------------------------------------------
    //     Next Covered Locus
    // -------------------------------------------------------------------------

    /**
     * @brief Return the next position (including the @p start_position) that is covered.
     *
     * @see next_covered( std::string const& chromosome, size_t start_position ) const
     *
     * @copydetails is_covered( utils::Bitvector const&, size_t )
     */
    static size_t next_covered( utils::Bitvector const& bitvector, size_t start_position )
    {
        // Boundary check.
        if( bitvector.empty() ) {
            throw std::invalid_argument(
                "GenomeLocusSet::next_covered() called with empty Bitvector"
            );
        }
        if( start_position == 0 ) {
            throw std::invalid_argument(
                "GenomeLocusSet::next_covered() called with start_position==0"
            );
        }

        // If the chromosome has the 0th bit set, the whole chromosome is covered,
        // so that the start_position we are at is also covered.
        if( bitvector.get( 0 ) ) {
            return start_position;
        }

        // If the above is not the case, check the actual start_position.
        // If the start_position is outside of the bitvector, it is not covered, obviously.
        return bitvector.find_next_set( start_position );
    }

    /**
     * @brief Return the next position (including the @p start_position) that is covered.
     *
     * @see next_covered( std::string const& chromosome, size_t start_position ) const
     *
     * @copydetails is_covered( const_iterator const&, size_t )
     */
    static size_t next_covered( const_iterator const& it, size_t start_position )
    {
        return next_covered( it->second, start_position );
    }

    /**
     * @brief Return the next position (including the @p start_position) that is covered.
     *
     * The function finds the next position after or including the @p start_position that is covered.
     * If the whole chromosome is covered (the 0th bit being true as the indicator for that),
     * then the @p start_position is returned. If no position after the @p start_position is covered
     * on the chromosome at all, or the chromosome is not in the set, then
     * GenomeLocusSet::npos is returned.
     */
    size_t next_covered( std::string const& chromosome, size_t start_position ) const
    {
        auto const it = locus_map_.find( chromosome );
        if( it == locus_map_.end() ) {
            return GenomeLocusSet::npos;
        }
        auto const& bv = it->second;
        assert( ! bv.empty() );
        return next_covered( bv, start_position );
    }

    // -------------------------------------------------------------------------
    //     Chromosome Iterators
    // -------------------------------------------------------------------------

    /**
     * @brief Return an iterator to the beginning of the map of chromosome names to Bitvectors.
     */
    const_iterator begin() const
    {
        return locus_map_.begin();
    }

    /**
     * @brief Return an iterator to the end of the map of chromosome names to Bitvectors.
     */
    const_iterator end() const
    {
        return locus_map_.end();
    }

    // -------------------------------------------------------------------------
    //     Chromosome Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether there are chromosomes with positions stored.
     */
    bool empty() const
    {
        return locus_map_.empty();
    }

    /**
     * @brief Return the number of chromosomes for which there are positions stored.
     */
    size_t chromosome_count() const
    {
        return locus_map_.size();
    }

    /**
     * @brief Get a list of all stored chromosome names.
     */
    std::vector<std::string> chromosome_names() const
    {
        std::vector<std::string> result;
        for( auto const& p : locus_map_ ) {
            result.push_back( p.first );
        }
        return result;
    }

    /**
     * @brief Return whether a chromosome is stored.
     */
    bool has_chromosome( std::string const& chromosome ) const
    {
        return locus_map_.count( chromosome ) > 0;
    }

    /**
     * @brief Find a chromosome in the map.
     *
     * Return an iterator to the entry, or to end() if the chromosome is not part of the set.
     */
    const_iterator find( std::string const& chromosome )
    {
        return locus_map_.find( chromosome );
    }

    /**
     * @brief For a given chromosome, return the
     * @link genesis::utils::Bitvector Bitvector@endlink that stores its positions.
     */
    utils::Bitvector const& chromosome_positions( std::string const& chromosome ) const
    {
        return locus_map_.at( chromosome );
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
        return locus_map_.at( chromosome );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Map from chromosome names to bitvectors representing which positions are in (true)
    // and out (false). Note that position 0 is special; if set, it means that we consider
    // the whole chromosome as covered.
    std::unordered_map<std::string, utils::Bitvector> locus_map_;

};

} // namespace population
} // namespace genesis

#endif // include guard
