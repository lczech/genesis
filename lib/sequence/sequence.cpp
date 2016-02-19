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

// -----------------------------------------------------
//     Label
// -----------------------------------------------------

std::string const& Sequence::label() const
{
    return label_;
}

void Sequence::label( std::string const& value )
{
    label_ = value;
}

// -----------------------------------------------------
//     Metadata
// -----------------------------------------------------

std::string const& Sequence::metadata() const
{
    return metadata_;
}

void Sequence::metadata( std::string const& value )
{
    metadata_ = value;
}

// -----------------------------------------------------
//     Sites
// -----------------------------------------------------

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

std::string& Sequence::sites()
{
    return sites_;
}

// ================================================================================================
//     Accessors
// ================================================================================================

/**
* @brief Return the length (number of sites) of this sequence.
*/
size_t Sequence::length() const
{
    return sites_.size();
}

/**
 * @brief Alias for length().
 */
size_t Sequence::size() const
{
    return sites_.size();
}

char Sequence::site_at( size_t index ) const
{
    return sites_.at(index);
}

char Sequence::operator [] (size_t index) const
{
    return sites_[index];
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
