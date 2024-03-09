#ifndef GENESIS_SEQUENCE_SEQUENCE_DICT_H_
#define GENESIS_SEQUENCE_SEQUENCE_DICT_H_

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
 * @ingroup sequence
 */

#include "genesis/sequence/sequence.hpp"

#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace sequence {

/**
 * @brief Store dictionary/index data on sequence files, such as coming from `.fai` or `.dict` files.
 *
 * See DictReader and FaiReader for the input format readers. Furthermore, FastaReader also offers
 * a mode to read a `fasta` file, and just retaining the dict/index information.
 * Lastly, see sequence_set_to_dict() for a function to get this information from a given
 * SequenceSet.
 *
 * Currently, we only store the sequence names and their lengths, in the order as provided in the
 * input file. We might add further information such as offset in the fasta file in the future,
 * once we offer to read with jumps in fasta files.
 *
 * @see ReferenceGenome
 */
class SequenceDict
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    struct Entry
    {
        std::string name;
        size_t length = 0;

        // /**
        // * @brief Alias to get the @p name of the Entry.
        // */
        // inline std::string const& name() const
        // {
        //     return name;
        // }

        /**
         * @brief Alias to get the @p name of the Entry.
         */
        inline std::string const& label() const
        {
            return name;
        }

        // /**
        //  * @brief Alias to get the @p length of the Entry.
        //  */
        // inline size_t length() const
        // {
        //     return length;
        // }

        /**
         * @brief Alias to get the @p length of the Entry.
         */
        inline size_t size() const
        {
            return length;
        }
    };

    using const_iterator = std::vector<Entry>::const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SequenceDict() = default;
    ~SequenceDict() = default;

    SequenceDict( SequenceDict const& ) = default;
    SequenceDict( SequenceDict&& )      = default;

    SequenceDict& operator= ( SequenceDict const& ) = default;
    SequenceDict& operator= ( SequenceDict&& )      = default;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Add a Sequence to the dictionary.
     *
     * @copydetails add( Entry&&, bool )
     */
    void add( Sequence const& sequence, bool also_look_up_first_word = true )
    {
        add( sequence.label(), sequence.length(), also_look_up_first_word );
    }

    /**
     * @brief Add an entry to the dictionary, given its name and length.
     *
     * @copydetails add( Entry&&, bool )
     */
    void add( std::string const& name, size_t length, bool also_look_up_first_word = true )
    {
        Entry e;
        e.name = name;
        e.length = length;
        add( std::move( e ), also_look_up_first_word );
    }

    /**
     * @brief Add an entry to the dictionary.
     *
     * @copydetails add( Entry&&, bool )
     */
    void add( Entry const& entry, bool also_look_up_first_word = true )
    {
        add( Entry{ entry }, also_look_up_first_word);
    }

    /**
     * @brief Add an entry to the dictionary.
     *
     * If @p also_look_up_first_word is set (true by default), we add an additional look up name for
     * the added sequence:
     * In addition to its full name, it can also be looked up with just the first word, that is,
     * until the first tab or space character, in case there are any, as this is what typical fasta
     * indexing tools also seem to do. The sequence is still stored with its original name though,
     * and just that additional lookup is added for using find() or get().
     */
    void add( Entry&& entry, bool also_look_up_first_word = true )
    {
        // Check for duplicates. As we are using the unordered map for indicies anyway,
        // we can just rely on that, and do not have to check the vector again.
        if( indices_.count( entry.name ) > 0 ) {
            throw std::runtime_error(
                "Cannot add duplicate sequence name \"" + entry.name + "\" to SequenceDict."
            );
        }
        assert( indices_.count( entry.name ) == 0 );

        // Do the same for the first-word form as well. We always compute the label here,
        // even if not used later, so that we can do the check before actually modifying our content.
        // Slightly cleaner that way.
        auto const label2 = utils::split( entry.name, "\t " )[0];
        if( also_look_up_first_word && indices_.count( label2 ) > 0 ) {
            throw std::runtime_error(
                "Cannot add duplicate sequence name \"" + label2 + "\" to SequenceDict, "
                "which is the shortened version of the original name \"" + entry.name + "\"."
            );
        }

        // We first create the index map entry, before adding the entry to the dict.
        // In that order, the index that we want to store in the map is the current size of the
        // vector pre insertion. We do the same for the shortened name as well, if wanted.
        indices_[ entry.name ] = entries_.size();
        if( also_look_up_first_word ) {
            indices_[ label2 ] = entries_.size();
        }
        entries_.push_back( entry );
        assert( indices_.count( entry.name ) == 1 && indices_[ entry.name ] == entries_.size() - 1 );
        assert(
            ! also_look_up_first_word ||
            ( indices_.count( label2 ) == 1 && indices_[ label2 ] == entries_.size() - 1 )
        );
    }

    void clear()
    {
        entries_.clear();
        indices_.clear();
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t size() const
    {
        return entries_.size();
    }

    Entry const& operator [] (size_t index) const
    {
        return entries_[index];
    }

    Entry const& at (size_t index) const
    {
        return entries_.at(index);
    }

    Entry const& get( std::string const& name ) const
    {
        auto const idx = index_of( name );
        assert( idx < entries_.size() );
        return entries_[ idx ];
    }

    size_t index_of( std::string const& name ) const
    {
        auto const it = indices_.find( name );
        if( it == indices_.end() ) {
            throw std::runtime_error(
                "Sequence name \"" + name + "\" not found in SequenceDict."
            );
        }
        assert( entries_[it->second].name == name );
        return it->second;
    }

    bool contains( std::string const& name ) const
    {
        return indices_.count( name ) > 0;
    }

    const_iterator find( std::string const& name ) const
    {
        auto const it = indices_.find( name );
        if( it == indices_.end() ) {
            return entries_.end();
        }
        assert( entries_[it->second].name == name );
        return entries_.begin() + it->second;
    }

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    const_iterator begin() const
    {
        return entries_.cbegin();
    }

    const_iterator end() const
    {
        return entries_.cend();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // vector of entries, and a fast-ish lookup from names to the index in the vector.
    std::vector<Entry> entries_;
    std::unordered_map<std::string, size_t> indices_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
