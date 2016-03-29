#ifndef GENESIS_SEQUENCE_SEQUENCE_SET_H_
#define GENESIS_SEQUENCE_SEQUENCE_SET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include <vector>

#include "sequence/sequence.hpp"

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sequence;

// =================================================================================================
//     SequenceSet
// =================================================================================================

class SequenceSet
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    typedef std::vector<Sequence>::iterator       iterator;
    typedef std::vector<Sequence>::const_iterator const_iterator;

    typedef Sequence&       reference;
    typedef Sequence const& const_reference;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SequenceSet() = default;
    ~SequenceSet() = default;

    SequenceSet( SequenceSet const& ) = default;
    SequenceSet( SequenceSet&& )      = default;

    SequenceSet& operator= ( SequenceSet const& ) = default;
    SequenceSet& operator= ( SequenceSet&& )      = default;

    void swap( SequenceSet& other )
    {
        using std::swap;
        swap( sequences_, other.sequences_ );
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t size() const;

    reference       operator[] (size_t index);
    const_reference operator[] (size_t index) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void push_back( Sequence const& s );
    void push_back( Sequence &&     s );

    void clear();

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<Sequence> sequences_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
