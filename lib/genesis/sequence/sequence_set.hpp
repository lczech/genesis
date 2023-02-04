#ifndef GENESIS_SEQUENCE_SEQUENCE_SET_H_
#define GENESIS_SEQUENCE_SEQUENCE_SET_H_

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

#include <stdexcept>
#include <vector>

namespace genesis {
namespace sequence {

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

    using       iterator = std::vector<Sequence>::iterator;
    using const_iterator = std::vector<Sequence>::const_iterator;

    using       reference = Sequence&;
    using const_reference = Sequence const&;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SequenceSet() = default;
    ~SequenceSet() = default;

    SequenceSet( SequenceSet const& ) = default;
    SequenceSet( SequenceSet&& )      = default;

    SequenceSet& operator= ( SequenceSet const& ) = default;
    SequenceSet& operator= ( SequenceSet&& )      = default;

    friend void swap( SequenceSet& lhs, SequenceSet& rhs )
    {
        using std::swap;
        swap( lhs.sequences_, rhs.sequences_ );
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * Return the number of Sequence%s in the SequenceSet.
     */
    size_t size() const
    {
        return sequences_.size();
    }

    /**
     * Return whether the SequenceSet is empty, i.e. whether its size() is 0.
     */
    bool empty() const
    {
        return sequences_.empty();
    }

    reference at ( size_t index )
    {
        return sequences_.at( index );
    }

    const_reference at ( size_t index ) const
    {
        return sequences_.at( index );
    }

    reference operator[] ( size_t index )
    {
        return sequences_[ index ];
    }

    const_reference operator[] ( size_t index ) const
    {
        return sequences_[ index ];
    }

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Add a Sequence to the SequenceSet by copying it, and return a reference to it.
     */
    reference add( Sequence const& s )
    {
        sequences_.push_back( s );
        return sequences_.back();
    }

    /**
    * @brief Add a Sequence to the SequenceSet by moving it, and return a reference to it.
    */
    reference add( Sequence&& s )
    {
        sequences_.push_back( std::move(s) );
        return sequences_.back();
    }

    /**
     * @brief Remove the Sequence at a given `index` from the SequenceSet.
     */
    void remove( size_t index )
    {
        if( index >= sequences_.size() ) {
            throw std::out_of_range( "Index out of range for removing from SequenceSet." );
        }

        sequences_.erase( sequences_.begin() + index );
    }

    /**
    * @brief Remove the Sequence%s between the `first_index` (inclusive) and the `last_index`
    * (exclusive) from the SequenceSet.
    */
    void remove( size_t first_index, size_t last_index )
    {
        if( first_index    >= sequences_.size()
            || last_index  >= sequences_.size()
            || first_index >= last_index
        ) {
            throw std::out_of_range( "Invalid indices for removing from SequenceSet." );
        }

        sequences_.erase( sequences_.begin() + first_index, sequences_.begin() + last_index );
    }

    /**
     * @brief Remove the Sequence at a given iterator `position` from the SequenceSet.
     */
    void remove( iterator position )
    {
        sequences_.erase( position );
    }

    /**
    * @brief Remove the Sequence%s between the `first` (inclusive) and the `last`
    * (exclusive) iterator position from the SequenceSet.
    */
    void remove( iterator first, iterator last )
    {
        sequences_.erase( first, last );
    }

    /**
     * @brief Remove all Sequence%s from the SequenceSet, leaving it with a size() of 0.
     */
    void clear()
    {
        sequences_.clear();
    }

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin()
    {
        return sequences_.begin();
    }

    iterator end()
    {
        return sequences_.end();
    }

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

    std::vector<Sequence> sequences_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
