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

#include <algorithm>
#include <string>
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

    void add( std::string const& name, size_t length )
    {
        Entry e;
        e.name = name;
        e.length = length;
        entries_.push_back( std::move( e ));
    }

    void add( Entry const& entry )
    {
        entries_.push_back( entry );
    }

    void add( Entry&& entry )
    {
        entries_.push_back( std::move( entry ));
    }

    void clear()
    {
        entries_.clear();
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

    bool contains( std::string const& name ) const
    {
        return find( name ) != entries_.end();
    }

    const_iterator find( std::string const& name ) const
    {
        return std::find_if(
            entries_.begin(), entries_.end(),
            [&name]( Entry const& entry ) {
                return entry.name == name;
            }
        );
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

    std::vector<Entry> entries_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
