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
 * @brief Implementation of Taxopath class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxopath.hpp"

#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

/**
 * @brief Fill constructor that uses the given strings to initialize the taxonomic elements.
 *
 * The strings are copied and used as Taxopath elements in the order in which they are stored
 * in the vector.
 */
Taxopath::Taxopath( std::vector< std::string > const& elements )
    : elements_( elements )
{}

/**
 * @brief Fill constructor that uses the given strings to initialize the taxonomic elements.
 *
 * The strings are moved and used as Taxopath elements in the order in which they are stored
 * in the vector.
 */
Taxopath::Taxopath( std::vector< std::string > && elements )
    : elements_( std::move( elements ))
{}

/**
 * @brief Swap the elements of two Taxopath%s.
 */
void Taxopath::swap( Taxopath& other )
{
    using std::swap;
    swap( elements_, other.elements_ );
}

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return whether the Taxopath is empty, i.e., does not contain any elements.
 */
bool Taxopath::empty() const
{
    return elements_.empty();
}

/**
 * @brief Return the number of elements of this Taxopath.
 */
size_t Taxopath::size() const
{
    return elements_.size();
}

/**
 * @brief Return the taxonomic element at a certain position.
 *
 * The function throws if the given index is not within the size of the taxonomic elements.
 */
std::string const& Taxopath::at ( size_t index ) const
{
    return elements_.at( index );
}

/**
 * @brief Return the taxonomic element at a certain position.
 *
 * The function throws if the given index is not within the size of the taxonomic elements.
 */
std::string& Taxopath::at ( size_t index )
{
    return elements_.at( index );
}

/**
 * @brief Return the taxonomic element at a certain position.
 *
 * The function does not check whether the given index is valid.
 */
std::string const& Taxopath::operator [] ( size_t index ) const
{
    return elements_[ index ];
}

/**
 * @brief Return the taxonomic element at a certain position.
 *
 * The function does not check whether the given index is valid.
 */
std::string& Taxopath::operator [] ( size_t index )
{
    return elements_[ index ];
}

/**
 * @brief Return the elements of the Taxopath as a vector of strings.
 */
std::vector< std::string > const& Taxopath::elements() const
{
    return elements_;
}

/**
 * @brief Return the elements of the Taxopath as a vector of strings.
 */
std::vector< std::string > & Taxopath::elements()
{
    return elements_;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Replace the current elements of the Taxopath by a list of strings.
 *
 * The strings are copied and used as Taxopath elements in the order in which they are stored
 * in the vector.
 */
void Taxopath::assign( std::vector< std::string > const& from )
{
    elements_ = from;
}

/**
 * @brief Add an element to the end of the Taxopath by copying it.
 */
void Taxopath::push_back( std::string const& value )
{
    elements_.push_back( value );
}

/**
 * @brief Add an element to the end of the Taxopath by moving it.
 */
void Taxopath::push_back( std::string&& value )
{
    elements_.push_back( std::move( value ));
}

/**
 * @brief Remove the last element of the Taxopath and return its value.
 *
 * The returned value is obtained as a copy of the last element before it is removed.
 */
std::string Taxopath::pop_back()
{
    if( elements_.empty() ) {
        throw std::out_of_range( "Cannot pop last element of empty Taxopath." );
    }
    auto last = elements_.back();
    elements_.pop_back();
    return last;
}

/**
 * @brief Clear all taxonomic elements. This results in an empty Taxopath.
 */
void Taxopath::clear()
{
    elements_.clear();
}

// =================================================================================================
//     Iterators
// =================================================================================================

/**
 * @brief Return an iterator to the beginning of the taxonomic elements.
 */
Taxopath::iterator Taxopath::begin() noexcept
{
    return elements_.begin();
}

/**
 * @brief Return an iterator to the end of the taxonomic elements.
 */
Taxopath::iterator Taxopath::end() noexcept
{
    return elements_.end();
}

/**
 * @brief Return a const iterator to the beginning of the taxonomic elements.
 */
Taxopath::const_iterator Taxopath::begin() const noexcept
{
    return elements_.cbegin();
}

/**
 * @brief Return a const iterator to the end of the taxonomic elements.
 */
Taxopath::const_iterator Taxopath::end() const noexcept
{
    return elements_.cend();
}

/**
 * @brief Return a reverse iterator to the reverse beginning of the taxonomic elements.
 */
Taxopath::reverse_iterator Taxopath::rbegin() noexcept
{
    return elements_.rbegin();
}

/**
 * @brief Return a reverse iterator to the reverse end of the taxonomic elements.
 */
Taxopath::reverse_iterator Taxopath::rend() noexcept
{
    return elements_.rend();
}

/**
 * @brief Return a const reverse iterator to the reverse beginning of the taxonomic elements.
 */
Taxopath::const_reverse_iterator Taxopath::rbegin() const noexcept
{
    return elements_.crbegin();
}

/**
 * @brief Return a const reverse iterator to the reverse end of the taxonomic elements.
 */
Taxopath::const_reverse_iterator Taxopath::rend() const noexcept
{
    return elements_.crend();
}

} // namespace taxonomy
} // namespace genesis
