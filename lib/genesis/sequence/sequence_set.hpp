#ifndef GENESIS_SEQUENCE_SEQUENCE_SET_H_
#define GENESIS_SEQUENCE_SEQUENCE_SET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/sequence/sequence.hpp"

#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class SequenceSet;
void swap( SequenceSet& lhs, SequenceSet& rhs );

// =================================================================================================
//     SequenceSet
// =================================================================================================

/**
 * @brief Store a set of Sequence%s.
 *
 * A SequenceSet is a simple container that allows to add(), remove() and iterate over Sequence%s.
 * Sequences are kept in the order in which they were added and can be accessed via an index, see
 * at().
 */
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

    friend void swap( SequenceSet& lhs, SequenceSet& rhs );

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * Return the number of Sequence%s in the SequenceSet.
     */
    size_t size() const;

    /**
     * Return whether the SequenceSet is empty, i.e. whether its size() is 0.
     */
    bool empty() const;

    reference       at ( size_t index );
    const_reference at ( size_t index ) const;

    reference       operator[] ( size_t index );
    const_reference operator[] ( size_t index ) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Add a Sequence to the SequenceSet by copying it, and return a reference to it.
     */
    reference add( Sequence const& s );

    /**
    * @brief Add a Sequence to the SequenceSet by moving it, and return a reference to it.
    */
    reference add( Sequence &&     s );

    /**
     * @brief Remove the Sequence at a given `index` from the SequenceSet.
     */
    void remove( size_t index );

    /**
    * @brief Remove the Sequence%s between the `first_index` (inclusive) and the `last_index`
    * (exclusive) from the SequenceSet.
    */
    void remove( size_t first_index, size_t last_index );

    /**
     * @brief Remove the Sequence at a given iterator `position` from the SequenceSet.
     */
    void remove( iterator position );

    /**
    * @brief Remove the Sequence%s between the `first` (inclusive) and the `last`
    * (exclusive) iterator position from the SequenceSet.
    */
    void remove( iterator first, iterator last );

    /**
     * @brief Remove all Sequence%s from the SequenceSet, leaving it with a size() of 0.
     */
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
