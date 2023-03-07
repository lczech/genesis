#ifndef GENESIS_SEQUENCE_SEQUENCE_REFERENCE_GENOME_H_
#define GENESIS_SEQUENCE_SEQUENCE_REFERENCE_GENOME_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * @ingroup sequence
 */

#include "genesis/sequence/sequence.hpp"

#include "genesis/utils/text/char.hpp"

#include <cassert>
#include <list>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace genesis {
namespace sequence {

// =================================================================================================
//     ReferenceGenome
// =================================================================================================

/**
 * @brief Lookup of Sequence%s of a reference genome.
 *
 * The class stores Sequence%s in the order they are added, but also stores a hash map for quickly
 * finding a Sequence given its name/label.
 */
class ReferenceGenome
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using       iterator = typename std::list<Sequence>::iterator;
    using const_iterator = typename std::list<Sequence>::const_iterator;

    using       reference = Sequence&;
    using const_reference = Sequence const&;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    ReferenceGenome() = default;
    ~ReferenceGenome() = default;

    ReferenceGenome( ReferenceGenome const& ) = delete;
    ReferenceGenome( ReferenceGenome&& )      = default;

    ReferenceGenome& operator= ( ReferenceGenome const& ) = delete;
    ReferenceGenome& operator= ( ReferenceGenome&& )      = default;

    friend void swap( ReferenceGenome& lhs, ReferenceGenome& rhs )
    {
        using std::swap;
        swap( lhs.sequences_, rhs.sequences_ );
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * Return the number of Sequence%s in the ReferenceGenome.
     */
    size_t size() const
    {
        return sequences_.size();
    }

    /**
     * Return whether the ReferenceGenome is empty, i.e. whether its size() is 0.
     */
    bool empty() const
    {
        return sequences_.empty();
    }

    bool contains( std::string const& label ) const
    {
        return lookup_.count( label );
    }

    /**
     * @brief Return an iterator to the Sequence with the given @p label, or an iterator to end()
     * if no Sequence with that label is present.
     */
    const_iterator find( std::string const& label ) const
    {
        // Try to get the sequence from the cache, for speed.
        if( cache_ != sequences_.cend() && cache_->label() == label ) {
            return cache_;
        }

        // If not cached, do a normal lookup, and set the cache.
        auto lit = lookup_.find( label );
        if( lit == lookup_.end() ) {
            return sequences_.end();
        }
        assert( lit->first == label );
        cache_ = lit->second;
        return lit->second;
    }

    /**
     * @brief Same as find(), but returns the sequence directly, or throws if not present.
     */
    inline const_reference get( std::string const& label ) const
    {
        auto const it = find( label );
        if( it == sequences_.end() ) {
            throw std::runtime_error(
                "Reference Genome does not contain requested sequence \"" + label + "\""
            );
        }
        return *it;
    }

    /**
     * @brief Get a particular base at a given chromosome and position.
     *
     * Reference genomes are often used to look up a particular base, so we offer this functionality
     * here directly. The function throws if either the chromosome is not part of the genome,
     * or if the position is outside of the size of the chromosome.
     *
     * Important: We here use 1-based indexing for the position, which differs from a direct lookup
     * using the sites of the sequence directly, but is more in line with the usage in our
     * population functions.
     */
    inline char get_base( std::string const& chromosome, size_t position, bool to_upper = true ) const
    {
        auto const& ref_seq = get( chromosome );
        if( position == 0 || position - 1 >= ref_seq.length() ) {
            throw std::runtime_error(
                "Reference Genome sequence \"" + chromosome + "\" is " +
                std::to_string( ref_seq.length() ) +
                " bases long, which is shorter than then requested (1-based) position " +
                std::to_string( position )
            );
        }
        assert( position - 1 < ref_seq.length() );
        if( to_upper ) {
            return utils::to_upper( ref_seq[ position - 1 ]);
        }
        return ref_seq[ position - 1 ];
    }

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Add a Sequence to the ReferenceGenome by copying it, and return a const_reference to it.
     */
    const_reference add( Sequence const& seq )
    {
        return add( Sequence{seq} );
    }

    /**
    * @brief Add a Sequence to the ReferenceGenome by moving it, and return a const_reference to it.
    */
    const_reference add( Sequence&& seq )
    {
        // Basic checks
        auto const label = seq.label();
        if( lookup_.count( label ) > 0 ) {
            throw std::runtime_error(
                "Reference Genome contains duplicate sequence name \"" + label + "\", "
                "which cannot be added again."
            );
        }
        assert( lookup_.count( label ) == 0 );

        // Add the sequence to the list, and to the lookup
        // We also need to reset the cache, to point to the new end of the list.
        sequences_.push_back( std::move(seq) );
        assert( sequences_.size() > 0 );
        lookup_[ label ] = std::prev( sequences_.cend() );
        assert( lookup_.count( label ) > 0 );
        cache_ = sequences_.cend();

        // Now return the sequence that was just added.
        return sequences_.back();
    }

    /**
     * @brief Remove all Sequence%s from the ReferenceGenome, leaving it with a size() of 0.
     */
    void clear()
    {
        sequences_.clear();
        lookup_.clear();
    }

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    const_iterator begin() const
    {
        return sequences_.cbegin();
    }

    const_iterator end() const
    {
        return sequences_.cend();
    }

    const_iterator cbegin() const
    {
        return sequences_.cbegin();
    }

    const_iterator cend() const
    {
        return sequences_.cend();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Using a list here to get stable iterators to be used in the map below.
    // We don't need random access (I think...), so that's fine.
    std::list<Sequence> sequences_;
    std::unordered_map<std::string, const_iterator> lookup_;

    // We keep a cache of the last sequence name that was requested,
    // for speeding up lookups on the same chromosome, which is the most typical case.
    mutable const_iterator cache_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
