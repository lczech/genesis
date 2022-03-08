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
 * @brief
 *
 * Interally, we use an @link genesis::utils::IntervalTree IntervalTree@endlink to represent the
 * regions of each chromosome. This is so that access and querying of contained positions is as fast
 * as possible, and so that we do not store the chromosome name string with every region.
 * However, that means that iterating over this class does not yield GenomeRegion%s immediately;
 * if needed, those have to be constructed from the intervals.
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
    using tree_type = genesis::utils::IntervalTree<data_type, numerical_type>;
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
     * @brief Add a GenomeRegion to the list, given its chromosome, and start and end positions.
     */
    void add( std::string const& chromosome, numerical_type start = 0, numerical_type end = 0 )
    {
        regions_[ chromosome ].insert({ start, end });
    }

    // -------------------------------------------
    //         Add Locus
    // -------------------------------------------

    /**
     * @brief Add a single Locus, that is, an interval covering one position on a chromosome.
     */
    void add( GenomeLocus const& locus )
    {
        regions_[ locus.chromosome ].insert({ locus.position, locus.position });
    }

    /**
     * @brief Add an interval between two Loci on the same chromosome.
     */
    void add( GenomeLocus const& start, GenomeLocus const& end )
    {
        if( start.chromosome != end.chromosome ) {
            throw std::invalid_argument(
                "Cannot use two GenomeLocus instances with different chromosomes "
                "as an entry in a GenomeRegionList."
            );
        }
        regions_[ start.chromosome ].insert({ start.position, end.position });
    }

    // -------------------------------------------
    //         Add Region
    // -------------------------------------------

    /**
     * @brief Add a GenomeRegion to the list.
     *
     * This function ensures that regions are valid (`start < end`), and keeps the list sorted.
     */
    void add( GenomeRegion const& region )
    {
        regions_[ region.chromosome ].insert({ region.start, region.end });
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
                "Chromosome name '" + chromosome + "' not found in GenomeRegionList"
            );
        }
        regions_.erase( chromosome );
    }

    // -------------------------------------------------------------------------
    //     Queries
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
        return chrom_tree.overlap_find( position ) != chrom_tree.end();
    }

    /**
     * @brief Return an iterator to the GenomeRegion that covers the given position on the
     * chromosome, or an end iterator if there is no region that covers the position.
     *
     * Throws an exception if the chromosome is not in the list at all.
     */
    const_iterator find( std::string const& chromosome, size_t position ) const
    {
        // Using find(), so we only have to search in the map once, for speed.
        auto const it = regions_.find( chromosome );
        if( it == regions_.end() ) {
            throw std::invalid_argument(
                "GenomeRegionList does not contain chromosome '" + chromosome + "'"
            );
        }
        auto const& chrom_tree = it->second;
        return chrom_tree.overlap_find( position );
    }

    tree_type& get_chromosome_regions( std::string const& chromosome )
    {
        return regions_[ chromosome ];
    }

    tree_type const& get_chromosome_regions( std::string const& chromosome ) const
    {
        return regions_.at( chromosome );
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    // /**
    //  * @brief Return the size of the list, in number of chromosomes that it contains.
    //  *
    //  * This is the size of the range of the begin() and end() iterators, and hence, for compliance
    //  * with the standard, what we return here. For the actual count of the contained regions
    //  * (intervals), see region_count().
    //  */
    // size_t size() const
    // {
    //     return regions_.size();
    // }

    /**
     * @brief Return the number of chromosomes for which there are regions stored.
     *
     * Same as size().
     */
    size_t chromosome_count() const
    {
        return regions_.size();
    }

    /**
     * @brief Return the number of regions stored for the specified @p chromosome.
     */
    size_t region_count( std::string const& chromosome ) const
    {
        if( regions_.count( chromosome ) == 0 ) {
            throw std::invalid_argument(
                "Chromosome name '" + chromosome + "' not found in GenomeRegionList"
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
     * @brief Return whether there are chromosomes with regions stored.
     */
    bool empty() const
    {
        return regions_.empty();
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
