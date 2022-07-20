#ifndef GENESIS_POPULATION_GENOME_REGION_LIST_H_
#define GENESIS_POPULATION_GENOME_REGION_LIST_H_

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

#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "genesis/population/genome_locus.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/utils/containers/interval_tree.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Data
// =================================================================================================

/**
 * @brief Helper struct to define a default empty data for the classes GenomeLocus, GenomeRegion,
 * and GenomeRegionList.
 */
struct EmptyGenomeData
{};

// =================================================================================================
//     Genome Region List
// =================================================================================================

/**
 * @brief List of regions in a genome, for each chromosome.
 *
 * The data structure stores a list of genome regions, such as coming from BED or GFF files.
 * It allows fast querying, that is, whether a certain position on a chromosome is part of one of
 * the stored regions. Furthermore, the class allows to iterate through the regions of each
 * chromosome.
 *
 * Positions in the interval of each region are 1-based and inclusive, that is, we used closed
 * intervals. We also offer the special case to add a whole chromosome as a region, in which case
 * the is_covered() function will return `true` for all positions on that chromosome (without
 * checking that the position is in fact part of the chromosome - as we do not use information
 * on the lengths of chromosomes in this class). We use start and end positions equal to 0 to
 * mark these special whole-chromosome cases - be aware of that when adding regions to the list.
 * See also GenomeLocus and GenomeRegion for related classes that have the same special cases.
 *
 * Interally, we use an @link genesis::utils::IntervalTree IntervalTree@endlink to represent the
 * regions of each chromosome, stored in a map from chromosome name to IntervalTree.
 * This is so that access and querying of contained positions is as fast as possible,
 * and so that we do not store the chromosome name string with every region.
 *
 * @see GenomeLocus
 * @see GenomeRegion
 */
// template<class DataType = EmptyGenomeData>
class GenomeRegionList
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    // using data_type = DataType;
    using data_type = EmptyGenomeData;
    using numerical_type = size_t;
    using tree_type = genesis::utils::IntervalTree<
        data_type, numerical_type, genesis::utils::IntervalClosed
    >;
    using self_type = GenomeRegionList;

    using iterator               = typename tree_type::iterator;
    using const_iterator         = typename tree_type::const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GenomeRegionList() = default;
    ~GenomeRegionList() = default;

    GenomeRegionList( GenomeRegionList const& ) = default;
    GenomeRegionList( GenomeRegionList&& )      = default;

    GenomeRegionList& operator= ( GenomeRegionList const& ) = default;
    GenomeRegionList& operator= ( GenomeRegionList&& )      = default;

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
        // Check chromosome.
        if( chromosome.empty() ) {
            throw std::invalid_argument(
                "Cannot add region to GenomeRegionList with empty chromosome name, "
                "as this denotes an invalid chromosome."
            );
        }

        // We use the special value 0 to denote that we want the whole chromosome.
        regions_[ chromosome ].insert({ 0, 0 });
    }

    /**
     * @brief Add a GenomeRegion to the list, given its chromosome, and start and end positions.
     *
     * The @p chromosome cannot be empty, and we expect @p start < @p end. Both @p start and @p end
     * are 1-based, and inclusive, that is, the interval between them is closed. The special case
     * of both @p start and @p end equal to 0 means that the whole chromosome is added as an interval.
     *
     * @copydetails GenomeRegionList::add( GenomeLocus const&, bool )
     */
    void add(
        std::string const& chromosome,
        numerical_type start,
        numerical_type end,
        bool overlap = false
    ) {
        // Check chromosome.
        if( chromosome.empty() ) {
            throw std::invalid_argument(
                "Cannot add region to GenomeRegionList with empty chromosome name, "
                "as this denotes an invalid chromosome."
            );
        }

        // Check positions.
        // The start and end are also checked in the interval tree, but let's do it here
        // so that the error message is nicer in case they are wrong.
        if( start > end ) {
            throw std::invalid_argument(
                "Cannot add region to GenomeRegionList with start == " +
                std::to_string( start ) + " > end == " + std::to_string( end )
            );
        }
        if(( start == 0 ) ^ ( end == 0 )) {
            throw std::invalid_argument(
                "Cannot add region to GenomeRegionList with either start == 0 or end == 0, "
                "but not both, as we use 1-base indexing, with both being 0 being interpreted "
                "as the special case of denoting the whole chromosome. "
                "Hence either both start and end have to be 0, or neither."
            );
        }

        // Insert, either by merging with an existing, or as a new region.
        // We just get the chromosome from the map via array access, which will create it
        // if it is not yet present.
        if( overlap ) {
            regions_[ chromosome ].insert_overlap({ start, end });
        } else {
            regions_[ chromosome ].insert({ start, end });
        }
    }

    // -------------------------------------------
    //         Add Locus
    // -------------------------------------------

    /**
     * @brief Add a single Locus, that is, an interval covering one position on a chromosome.
     *
     * If @p overlap is set, we first check if there is a region already in the list that overlaps
     * with the one that is to be added. If so, the new region is merged with existing one,
     * instead of inserting it. This is more useful of the region list is used to determine coverage,
     * and less useful if regions are meant to indicate some specific parts of the genome,
     * such as genes.
     */
    void add( GenomeLocus const& locus, bool overlap = false )
    {
        add( locus.chromosome, locus.position, locus.position, overlap );
    }

    /**
     * @brief Add an interval between two Loci on the same chromosome.
     *
     * @copydetails GenomeRegionList::add( GenomeLocus const&, bool )
     */
    void add( GenomeLocus const& start, GenomeLocus const& end, bool overlap = false )
    {
        if( start.chromosome != end.chromosome ) {
            throw std::invalid_argument(
                "Cannot use two GenomeLocus instances with different chromosomes ( start == \"" +
                start.chromosome + "\", end == \"" + end.chromosome + "\") as an entry in a "
                "GenomeRegionList."
            );
        }
        add( start.chromosome, start.position, end.position, overlap );
    }

    // -------------------------------------------
    //         Add Region
    // -------------------------------------------

    /**
     * @brief Add a GenomeRegion to the list.
     *
     * This function ensures that regions are valid (`start < end`).
     *
     * @copydetails GenomeRegionList::add( GenomeLocus const&, bool )
     */
    void add( GenomeRegion const& region, bool overlap = false )
    {
        add( region.chromosome, region.start, region.end, overlap );
    }

    // -------------------------------------------
    //         Add Region List
    // -------------------------------------------

    /**
     * @brief Add a complete GenomeRegionList to this list.
     *
     * This function copies all entries of the @p list.
     *
     * @copydetails GenomeRegionList::add( GenomeLocus const&, bool )
     */
    void add( GenomeRegionList const& other, bool overlap = false )
    {
        for( auto const& chr : other.regions_ ) {
            for( auto const& interval : chr.second ) {
                add( chr.first, interval.low(), interval.high(), overlap );
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
        regions_.clear();
    }

    /**
     * @brief Remove the regions of the specified @p chromosome.
     */
    void clear( std::string const& chromosome )
    {
        if( regions_.count( chromosome ) == 0 ) {
            throw std::invalid_argument(
                "Chromosome name \"" + chromosome + "\" not found in GenomeRegionList"
            );
        }
        regions_.erase( chromosome );
    }

    // -------------------------------------------------------------------------
    //     Locus Queries
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether a given position on a chromosome is part of any of the regions stored.
     */
    bool is_covered( std::string const& chromosome, numerical_type position ) const
    {
        // Using find(), so we only have to search in the map once, for speed.
        auto const it = regions_.find( chromosome );
        if( it == regions_.end() ) {
            return false;
        }
        auto const& chrom_tree = it->second;

        // If the chromosome in our interval tree contains the 0 interval, we consider that
        // as having the whole chromosome covered.
        if( chrom_tree.overlap_find( 0 ) != chrom_tree.end() ) {
            return true;
        }

        // If the above is not the case, check the actual position.
        return chrom_tree.overlap_find( position ) != chrom_tree.end();
    }

    /**
     * @brief Return whether a whole chromosome is covered.
     */
    bool is_covered( std::string const& chromosome ) const
    {
        auto const it = regions_.find( chromosome );
        if( it == regions_.end() ) {
            return false;
        }
        auto const& chrom_tree = it->second;
        return chrom_tree.overlap_find( 0 ) != chrom_tree.end();
    }

    // Not used at the moment, as we have no access to the end iterator to check for a valid find.

    // /* *
    //  * @brief Return an iterator to the GenomeRegion that covers the given position on the
    //  * chromosome, or an end iterator if there is no region that covers the position.
    //  *
    //  * Throws an exception if the chromosome is not in the list at all.
    //  */
    // const_iterator find( std::string const& chromosome, size_t position ) const
    // {
    //     // Using find(), so we only have to search in the map once, for speed.
    //     auto const it = regions_.find( chromosome );
    //     if( it == regions_.end() ) {
    //         throw std::invalid_argument(
    //             "GenomeRegionList does not contain chromosome \"" + chromosome + "\""
    //         );
    //     }
    //     auto const& chrom_tree = it->second;
    //     return chrom_tree.overlap_find( position );
    // }

    // -------------------------------------------------------------------------
    //     Chromosome Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether there are chromosomes with regions stored.
     */
    bool empty() const
    {
        return regions_.empty();
    }

    /**
     * @brief Return the number of chromosomes for which there are regions stored.
     */
    size_t chromosome_count() const
    {
        return regions_.size();
    }

    /**
     * @brief Get a list of all stored chromosome names.
     */
    std::vector<std::string> chromosome_names() const
    {
        std::vector<std::string> result;
        for( auto const& p : regions_ ) {
            result.push_back( p.first );
        }
        return result;
    }

    /**
     * @brief Return whether a chromosome is stored.
     */
    bool has_chromosome( std::string const& chromosome ) const
    {
        return regions_.count( chromosome ) > 0;
    }

    /**
     * @brief For a given chromosome, return the
     * @link genesis::utils::IntervalTree IntervalTree@endlink that stores its regions.
     */
    tree_type const& chromosome_regions( std::string const& chromosome ) const
    {
        return regions_.at( chromosome );
    }

    /**
     * @brief For a given chromosome, return the
     * @link genesis::utils::IntervalTree IntervalTree@endlink that stores its regions.
     *
     * Note that this exposes the underlying container, and hence has to be used with caution.
     * In particular position 0 is considered special in this GenomeRegionList class:
     * any chromosome for which we have stored an interval that covers 0 is considered to be
     * fully covered for all its positions.
     */
    tree_type& chromosome_regions( std::string const& chromosome )
    {
        return regions_.at( chromosome );
    }

    /**
     * @brief Return the number of regions stored for the specified @p chromosome.
     */
    size_t region_count( std::string const& chromosome ) const
    {
        if( regions_.count( chromosome ) == 0 ) {
            throw std::invalid_argument(
                "Chromosome name \"" + chromosome + "\" not found in GenomeRegionList"
            );
        }
        return regions_.at( chromosome ).size();
    }

    /**
     * @brief Return the number of regions stored in total, across all chromosomes.
     */
    size_t total_region_count() const
    {
        size_t cnt = 0;
        for( auto const& reg : regions_ ) {
            cnt += reg.second.size();
        }
        return cnt;
    }

    /**
     * @brief Access the underlying container directly.
     *
     * Expose the map from chromosome names to the
     * @link genesis::utils::IntervalTree IntervalTree@endlink that stores the regions of each
     * chromosome.
     * This is okay to expose, as this class is merely a thin convenience wrapper around it anyway.
     * If the class ever changes to be more than that, we might remove access to this.
     *
     * @see chromosome_regions( std::string const& )
     */
    std::map<std::string, tree_type> const& chromosome_map() const
    {
        return regions_;
    }

    /**
     * @copydoc chromosome_map() const
     */
    std::map<std::string, tree_type>& chromosome_map()
    {
        return regions_;
    }

    // -------------------------------------------------------------------------
    //     Iterator
    // -------------------------------------------------------------------------

    // iterator begin()
    // {
    //     return regions_.begin();
    // }

    // const_iterator begin() const
    // {
    //     return regions_.begin();
    // }

    // iterator end()
    // {
    //     return regions_.end();
    // }

    // const_iterator end() const
    // {
    //     return regions_.end();
    // }

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    // bool allow_overlap() const
    // {
    //     return allow_overlap_;
    // }
    //
    // self_type& allow_overlap( bool value )
    // {
    //     when set this way, it will only affect newly added regions...
    //     better in constructor and const?
    //     allow_overlap_ = value;
    //     return *this;
    // }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::map<std::string, tree_type> regions_;

    // bool allow_overlap_ = true;

};

} // namespace population
} // namespace genesis

#endif // include guard
