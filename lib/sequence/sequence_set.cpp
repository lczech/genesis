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
 * @brief Implementation of SequenceSet class.
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/sequence_set.hpp"

namespace genesis {
namespace sequence {

// =============================================================================
//     Accessors
// =============================================================================

size_t SequenceSet::size() const
{
    return sequences_.size();
}

SequenceSet::reference SequenceSet::operator[] (size_t index)
{
    return sequences_[index];
}

SequenceSet::const_reference SequenceSet::operator[] (size_t index) const
{
    return sequences_[index];
}

// =============================================================================
//     Modifiers
// =============================================================================

void SequenceSet::push_back( Sequence const& s )
{
    sequences_.push_back(s);
}

void SequenceSet::push_back( Sequence && s )
{
    sequences_.push_back(std::move(s));
}

/**
 * @brief Delete all sequences from the set.
 */
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
