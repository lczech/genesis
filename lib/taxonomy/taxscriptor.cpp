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
 * @brief Implementation of Taxscriptor class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "taxonomy/taxscriptor.hpp"

#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

/**
 * @brief Fill constructor that uses the given strings to initialize the taxonomic elements.
 *
 * The strings are copied and used as Taxscriptor elements in the order in which they are stored
 * in the vector.
 */
Taxscriptor::Taxscriptor( std::vector< std::string > const& elements )
    : elements_( elements )
{}

/**
 * @brief Fill constructor that uses the given strings to initialize the taxonomic elements.
 *
 * The strings are moved and used as Taxscriptor elements in the order in which they are stored
 * in the vector.
 */
Taxscriptor::Taxscriptor( std::vector< std::string > && elements )
    : elements_( std::move( elements ))
{}

/**
 * @brief Swap the elements of two Taxscriptor%s.
 */
void Taxscriptor::swap( Taxscriptor& other )
{
    using std::swap;
    swap( elements_, other.elements_ );
}

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return whether the Taxscriptor is empty, i.e., does not contain any elements.
 */
bool Taxscriptor::empty() const
{
    return elements_.empty();
}

/**
 * @brief Return the number of elements of this Taxscriptor.
 */
size_t Taxscriptor::size() const
{
    return elements_.size();
}

/**
 * @brief Return the taxonomic element at a certain position.
 *
 * The function throws if the given index is not within the size of the taxonomic elements.
 */
std::string const& Taxscriptor::at ( size_t index ) const
{
    return elements_.at( index );
}

/**
 * @brief Return the taxonomic element at a certain position.
 *
 * The function throws if the given index is not within the size of the taxonomic elements.
 */
std::string& Taxscriptor::at ( size_t index )
{
    return elements_.at( index );
}

/**
 * @brief Return the taxonomic element at a certain position.
 *
 * The function does not check whether the given index is valid.
 */
std::string const& Taxscriptor::operator [] ( size_t index ) const
{
    return elements_[ index ];
}

/**
 * @brief Return the taxonomic element at a certain position.
 *
 * The function does not check whether the given index is valid.
 */
std::string& Taxscriptor::operator [] ( size_t index )
{
    return elements_[ index ];
}

/**
 * @brief Return the elements of the Taxscriptor as a vector of strings.
 */
std::vector< std::string > const& Taxscriptor::elements() const
{
    return elements_;
}

/**
 * @brief Return the elements of the Taxscriptor as a vector of strings.
 */
std::vector< std::string > & Taxscriptor::elements()
{
    return elements_;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Replace the current elements of the Taxscriptor by a list of strings.
 *
 * The strings are copied and used as Taxscriptor elements in the order in which they are stored
 * in the vector.
 */
void Taxscriptor::assign( std::vector< std::string > const& from )
{
    elements_ = from;
}

/**
 * @brief Add an element to the end of the Taxscriptor by copying it.
 */
void Taxscriptor::push_back( std::string const& value )
{
    elements_.push_back( value );
}

/**
 * @brief Add an element to the end of the Taxscriptor by moving it.
 */
void Taxscriptor::push_back( std::string&& value )
{
    elements_.push_back( std::move( value ));
}

/**
 * @brief Remove the last element of the Taxscriptor and return its value.
 *
 * The returned value is obtained as a copy of the last element before it is removed.
 */
std::string Taxscriptor::pop_back()
{
    if( elements_.empty() ) {
        throw std::out_of_range( "Cannot pop last element of empty Taxscriptor." );
    }
    auto last = elements_.back();
    elements_.pop_back();
    return last;
}

/**
 * @brief Clear all taxonomic elements. This results in an empty Taxscriptor.
 */
void Taxscriptor::clear()
{
    elements_.clear();
}

// =================================================================================================
//     Iterators
// =================================================================================================

/**
 * @brief Return an iterator to the beginning of the taxonomic elements.
 */
Taxscriptor::iterator Taxscriptor::begin() noexcept
{
    return elements_.begin();
}

/**
 * @brief Return an iterator to the end of the taxonomic elements.
 */
Taxscriptor::iterator Taxscriptor::end() noexcept
{
    return elements_.end();
}

/**
 * @brief Return a const iterator to the beginning of the taxonomic elements.
 */
Taxscriptor::const_iterator Taxscriptor::begin() const noexcept
{
    return elements_.cbegin();
}

/**
 * @brief Return a const iterator to the end of the taxonomic elements.
 */
Taxscriptor::const_iterator Taxscriptor::end() const noexcept
{
    return elements_.cend();
}

/**
 * @brief Return a reverse iterator to the reverse beginning of the taxonomic elements.
 */
Taxscriptor::reverse_iterator Taxscriptor::rbegin() noexcept
{
    return elements_.rbegin();
}

/**
 * @brief Return a reverse iterator to the reverse end of the taxonomic elements.
 */
Taxscriptor::reverse_iterator Taxscriptor::rend() noexcept
{
    return elements_.rend();
}

/**
 * @brief Return a const reverse iterator to the reverse beginning of the taxonomic elements.
 */
Taxscriptor::const_reverse_iterator Taxscriptor::rbegin() const noexcept
{
    return elements_.crbegin();
}

/**
 * @brief Return a const reverse iterator to the reverse end of the taxonomic elements.
 */
Taxscriptor::const_reverse_iterator Taxscriptor::rend() const noexcept
{
    return elements_.crend();
}

} // namespace taxonomy
} // namespace genesis
