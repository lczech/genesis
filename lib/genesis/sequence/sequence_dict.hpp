#ifndef GENESIS_SEQUENCE_SEQUENCE_DICT_H_
#define GENESIS_SEQUENCE_SEQUENCE_DICT_H_

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

    void add( Sequence const& sequence )
    {
        add( sequence.label(), sequence.length() );
    }

    void add( std::string const& name, size_t length )
    {
        Entry e;
        e.name = name;
        e.length = length;
        add( std::move( e ));
    }

    void add( Entry const& entry )
    {
        add( Entry{ entry });
    }

    void add( Entry&& entry )
    {
        // Check for duplicates. As we are using the unordered map for indicies anyway,
        // we can just rely on that, and do not have to check the vector again.
        if( indices_.count( entry.name ) > 0 ) {
            throw std::runtime_error(
                "Cannot add duplicate sequence name \"" + entry.name + "\" to SequenceDict."
            );
        }

        // We first create the index map entry, making a copy of the name key (which the map needs
        // to make anyway), but then can move the string to the entries, for efficiency.
        // In that order, the index that we want to store in the map is the current size of the
        // vector pre insertion.
        indices_[ entry.name ] = entries_.size();
        entries_.push_back( std::move( entry ));
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
