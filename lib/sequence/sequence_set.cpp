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
