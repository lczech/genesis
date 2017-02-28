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
 * @brief Implementation of SequenceSet class.
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/sequence_set.hpp"

#include <stdexcept>

namespace genesis {
namespace sequence {

// =============================================================================
//     Constructors and Rule of Five
// =============================================================================

void swap( SequenceSet& lhs, SequenceSet& rhs )
{
    using std::swap;
    swap( lhs.sequences_, rhs.sequences_ );
}

// =============================================================================
//     Accessors
// =============================================================================

size_t SequenceSet::size() const
{
    return sequences_.size();
}

bool SequenceSet::empty() const
{
    return sequences_.empty();
}

SequenceSet::reference SequenceSet::at ( size_t index )
{
    return sequences_.at( index );
}

SequenceSet::const_reference SequenceSet::at ( size_t index ) const
{
    return sequences_.at( index );
}

SequenceSet::reference SequenceSet::operator[] ( size_t index )
{
    return sequences_[ index ];
}

SequenceSet::const_reference SequenceSet::operator[] ( size_t index ) const
{
    return sequences_[ index ];
}

// =============================================================================
//     Modifiers
// =============================================================================

SequenceSet::reference SequenceSet::add( Sequence const& s )
{
    sequences_.push_back( s );
    return sequences_.back();
}

SequenceSet::reference SequenceSet::add( Sequence && s )
{
    sequences_.push_back( std::move(s) );
    return sequences_.back();
}

void SequenceSet::remove( size_t index )
{
    if( index >= sequences_.size() ) {
        throw std::out_of_range( "Index out of range for removing from SequenceSet." );
    }

    sequences_.erase( sequences_.begin() + index );
}

void SequenceSet::remove( size_t first_index, size_t last_index )
{
    if( first_index    >= sequences_.size()
        || last_index  >= sequences_.size()
        || first_index >= last_index
    ) {
        throw std::out_of_range( "Invalid indices for removing from SequenceSet." );
    }

    sequences_.erase( sequences_.begin() + first_index, sequences_.begin() + last_index );
}

void SequenceSet::remove( iterator position )
{
    sequences_.erase( position );

}

void SequenceSet::remove( iterator first, iterator last )
{
    sequences_.erase( first, last );
}

void SequenceSet::clear()
{
    sequences_.clear();
}

// =============================================================================
//     Iterators
// =============================================================================

SequenceSet::iterator SequenceSet::begin()
{
    return sequences_.begin();
}

SequenceSet::iterator SequenceSet::end()
{
    return sequences_.end();
}

SequenceSet::const_iterator SequenceSet::begin() const
{
    return sequences_.cbegin();
}

SequenceSet::const_iterator SequenceSet::end() const
{
    return sequences_.cend();
}

SequenceSet::const_iterator SequenceSet::cbegin() const
{
    return sequences_.cbegin();
}

SequenceSet::const_iterator SequenceSet::cend() const
{
    return sequences_.cend();
}

} // namespace sequence
} // namespace genesis
