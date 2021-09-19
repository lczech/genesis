#ifndef GENESIS_POPULATION_GENOME_REGION_H_
#define GENESIS_POPULATION_GENOME_REGION_H_

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

#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Region
// =================================================================================================

/**
 * @brief A region (between two positions) on a chromosome.
 *
 * This can be used to represent a gene, a feature, or just generally a region of interest.
 * We use a simple form with a chromosome name, and a start and end position, both inclusive.
 */
struct GenomeRegion
{
public:

    std::string chromosome;
    size_t start = 0;
    size_t end   = 0;

    GenomeRegion( std::string const& chr = "", size_t s = 0, size_t e = 0 )
        : chromosome( chr )
        , start(s)
        , end(e)
    {}
};

// =================================================================================================
//     Genome Region List
// =================================================================================================

/**
 * @brief A sorted list of genome regions.
 *
 * This list keeps GenomeRegion%s and sorts them by chromosome and interval. It (currently) does
 * not allow overlapping/nested regions. This list can be used either as simply a set of regions
 * to process, or, using find() and is_covered() to query whether certain positions on a chromosome
 * are part of any of the regions stored in the list.
 */
class GenomeRegionList
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type       = GenomeRegionList;
    using container       = std::vector<GenomeRegion>;

    using value_type      = GenomeRegion;
    using reference       = value_type&;
    using const_reference = value_type const&;

    using iterator               = typename container::iterator;
    using const_iterator         = typename container::const_iterator;
    using reverse_iterator       = typename container::reverse_iterator;
    using const_reverse_iterator = typename container::const_reverse_iterator;

    using difference_type = typename container::difference_type;
    using size_type       = typename container::size_type;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GenomeRegionList()
        : find_cache_( regions_.end() )
    {}

    ~GenomeRegionList() = default;

    GenomeRegionList( GenomeRegionList const& ) = default;
    GenomeRegionList( GenomeRegionList&& )      = default;

    GenomeRegionList& operator= ( GenomeRegionList const& ) = default;
    GenomeRegionList& operator= ( GenomeRegionList&& )      = default;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Add a GenomeRegion to the list.
     *
     * This function ensures that regions are valid (`start < end`), and keeps the list sorted.
     */
    void add( GenomeRegion const& region );

    /**
     * @brief Add a GenomeRegion to the list, given its chromosome, and start and end positions.
     */
    void add( std::string const& chromosome, size_t start = 0, size_t end = 0 );

    // -------------------------------------------------------------------------
    //     Queries
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether a given position on a chromosome is part of any of the regions stored.
     */
    bool is_covered( std::string const& chromosome, size_t position ) const;

    /**
     * @brief Return an iterator to the GenomeRegion that covers the given position on the
     * chromosome, or an end iterator if there is no region that covers the position.
     */
    const_iterator find( std::string const& chromosome, size_t position ) const;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t size() const
    {
        return regions_.size();
    }

    bool empty() const
    {
        return regions_.empty();
    }

    const_reference operator[]( size_t i ) const
    {
        return regions_[i];
    }

    const_reference at( size_t i ) const
    {
        return regions_.at(i);
    }

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    // iterator begin()
    // {
    //     return regions_.begin();
    // }

    const_iterator begin() const
    {
        return regions_.begin();
    }

    // iterator end()
    // {
    //     return regions_.end();
    // }

    const_iterator end() const
    {
        return regions_.end();
    }


    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    container regions_;
    mutable const_iterator find_cache_;

};

} // namespace population
} // namespace genesis

#endif // include guard
