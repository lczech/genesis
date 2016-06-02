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

// -----------------------------------------------------
//     Clear
// -----------------------------------------------------

void Sequence::clear()
{
    label_    = "";
    metadata_ = "";
    sites_    = "";
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
