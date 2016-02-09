/**
 * @brief Implementation of Sequence class.
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/sequence.hpp"

namespace genesis {
namespace sequence {

// ================================================================================================
//     Properties
// ================================================================================================

std::string const& Sequence::label() const
{
    return label_;
}

void Sequence::label( std::string const& value )
{
    label_ = value;
}

std::string const& Sequence::metadata() const
{
    return metadata_;
}

void Sequence::metadata( std::string const& value )
{
    metadata_ = value;
}

std::string const& Sequence::sites() const
{
    return sites_;
}

void Sequence::sites( std::string const& value )
{
    sites_ = value;
}

void Sequence::sites( std::string && value )
{
    sites_ = std::move(value);
}

// ================================================================================================
//     Accessors
// ================================================================================================

size_t Sequence::length() const
{
    return sites_.size();
}

char Sequence::site_at( size_t index ) const
{
    return sites_.at(index);
}

// =============================================================================
//     Iterators
// =============================================================================

Sequence::iterator Sequence::begin()
{
    return sites_.begin();
}

Sequence::iterator Sequence::end()
{
    return sites_.end();
}

Sequence::const_iterator Sequence::begin() const
{
    return sites_.cbegin();
}

Sequence::const_iterator Sequence::end() const
{
    return sites_.cend();
}

Sequence::const_iterator Sequence::cbegin() const
{
    return sites_.cbegin();
}

Sequence::const_iterator Sequence::cend() const
{
    return sites_.cend();
}

} // namespace sequence
} // namespace genesis
