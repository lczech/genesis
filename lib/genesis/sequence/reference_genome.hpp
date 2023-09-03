#ifndef GENESIS_SEQUENCE_REFERENCE_GENOME_H_
#define GENESIS_SEQUENCE_REFERENCE_GENOME_H_

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

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     ReferenceGenome
// =================================================================================================

/**
 * @brief Lookup of Sequence%s of a reference genome.
 *
 * The class stores Sequence%s in the order they are added, but also stores a hash map for quickly
 * finding a Sequence given its name/label, as well as a lookup of bases at positions in the genome.
 *
 * @see SequenceDict
 */
class ReferenceGenome
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using       iterator = typename std::vector<Sequence>::iterator;
    using const_iterator = typename std::vector<Sequence>::const_iterator;

    using       reference = Sequence&;
    using const_reference = Sequence const&;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    ReferenceGenome()
    {
        guard_ = genesis::utils::make_unique<std::mutex>();
    }

    ~ReferenceGenome() = default;

    ReferenceGenome( ReferenceGenome const& ) = delete;
    ReferenceGenome( ReferenceGenome&& )      = default;

    ReferenceGenome& operator= ( ReferenceGenome const& ) = delete;
    ReferenceGenome& operator= ( ReferenceGenome&& )      = default;

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
        // Lock access to the cache. Released at the end of the scope.
        assert( guard_ );
        std::lock_guard<std::mutex> lock( *guard_ );

        // Try to get the sequence from the cache, for speed.
        assert( cache_ == std::numeric_limits<size_t>::max() || cache_ < sequences_.size() );
        if( cache_ != std::numeric_limits<size_t>::max() && sequences_[cache_].label() == label ) {
            assert( cache_ < sequences_.size() );
            return sequences_.begin() + cache_;
        }

        // If not cached, do a normal lookup, and set the cache.
        auto lit = lookup_.find( label );
        if( lit == lookup_.end() ) {
            return sequences_.end();
        }
        assert( lit->first == label );
        assert( lit->second < sequences_.size() );
        cache_ = lit->second;
        return sequences_.begin() + lit->second;
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
     *
     * If @p also_look_up_first_word is set (true by default), we add an additional look up name for
     * the added sequence:
     * In addition to its full name, it can also be looked up with just the first word, that is,
     * until the first tab or space character, in case there are any, as this is what typical fasta
     * indexing tools also seem to do. The sequence is still stored with its original name though,
     * and just that additional lookup is added for using find() or get().
     */
    const_reference add( Sequence const& seq, bool also_look_up_first_word = true )
    {
        return add( Sequence{seq}, also_look_up_first_word );
    }

    /**
    * @brief Add a Sequence to the ReferenceGenome by moving it, and return a const_reference to it.
    *
    * @copydetails add( Sequence const&, bool )
    */
    const_reference add( Sequence&& seq, bool also_look_up_first_word = true )
    {
        // Get and check the original form of the label.
        auto const label1 = seq.label();
        if( lookup_.count( label1 ) > 0 ) {
            throw std::runtime_error(
                "Reference Genome already contains sequence name \"" + label1 + "\", "
                "which cannot be added again."
            );
        }
        assert( lookup_.count( label1 ) == 0 );

        // Do the same for the first-word form as well. We always compute the label here,
        // even if not used later, so that we can do the check before actually modifying our content.
        // Slightly cleaner that way.
        auto const label2 = utils::split( seq.label(), "\t " )[0];
        if( also_look_up_first_word && lookup_.count( label2 ) > 0 ) {
            throw std::runtime_error(
                "Reference Genome already contains sequence name \"" + label2 + "\", "
                "which is the shortened version of the original name \"" + label1 + "\"."
            );
        }

        // Lock access to the cache. Probably not needed here, as adding sequences from
        // mutliple threads is unlikely, but doesn't hurt to have it here.
        // Released at the end of the scope.
        assert( guard_ );
        std::lock_guard<std::mutex> lock( *guard_ );

        // Add the sequence to the list. We also need to reset the cache.
        sequences_.push_back( std::move(seq) );
        cache_ = std::numeric_limits<size_t>::max();

        // Also add the sequence name to the lookup. If we also add a first-word-only version of it,
        // we might have cases where this is the same as the original (when the name does not
        // contain any tabs or spaces), but that doesn't matter; we'd just add the same label
        // twice (which would overwrite it in the map), pointing to the same sequence either way.
        assert( sequences_.size() > 0 );
        lookup_[ label1 ] = sequences_.size() - 1;
        assert( lookup_.count( label1 ) > 0 );
        if( also_look_up_first_word ) {
            lookup_[ label2 ] = sequences_.size() - 1;
            assert( lookup_.count( label2 ) > 0 );
        }

        // Now return the sequence that was just added.
        return sequences_.back();
    }

    /**
     * @brief Remove all Sequence%s from the ReferenceGenome, leaving it with a size() of 0.
     */
    void clear()
    {
        assert( guard_ );
        std::lock_guard<std::mutex> lock( *guard_ );
        sequences_.clear();
        lookup_.clear();
        cache_ = std::numeric_limits<size_t>::max();
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

    // Keep the sequences, as well as a lookup from names to indices in the vector.
    std::vector<Sequence> sequences_;
    std::unordered_map<std::string, size_t> lookup_;

    // We keep a cache of the last sequence name that was requested,
    // for speeding up lookups on the same chromosome, which is the most typical case.
    // Needs to be mutexed, as otherwise multiple threads might clash when accessing the cache.
    mutable size_t cache_;
    mutable std::unique_ptr<std::mutex> guard_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
