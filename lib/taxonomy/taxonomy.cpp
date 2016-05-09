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
 * @brief Implementation of Taxonomy class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "taxonomy/taxonomy.hpp"

#include "taxonomy/rank.hpp"

#include <algorithm>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// ================================================================================================
//     Accessors
// ================================================================================================

/**
 * @brief Return the number of child ranks.
 */
size_t Taxonomy::size() const
{
    return children_.size();
}

/**
 * @brief Return whether an immediate child rank with the given name exists.
 */
bool Taxonomy::has_child ( std::string name ) const
{
    return children_.end() != std::find_if(
        children_.begin(),
        children_.end(),
        [ &name ] ( Rank const& r ) {
            return r.name() == name;
        }
    );
}

/**
 * @brief Return the child rank with a given name if it exists, or throw otherwise.
 */
Rank const& Taxonomy::get_child ( std::string name ) const
{
    for( auto const& c : children_ ) {
        if( c.name() == name ) {
            return c;
        }
    }
    throw std::runtime_error( "Rank has no child named '" + name + "'." );
}

/**
 * @brief Return the child rank with a given name if it exists, or throw otherwise.
 */
Rank& Taxonomy::get_child ( std::string name )
{
    for( auto& c : children_ ) {
        if( c.name() == name ) {
            return c;
        }
    }
    throw std::runtime_error( "Rank has no child named '" + name + "'." );
}

/**
 * @brief Return the child rank with a given name if it exists, or throw otherwise.
 */
Rank const& Taxonomy::operator [] ( std::string name ) const
{
    return get_child( name );
}

/**
 * @brief Return the child rank with a given name if it exists, or throw otherwise.
 */
Rank& Taxonomy::operator [] ( std::string name )
{
    return get_child( name );
}

// ================================================================================================
//     Modifiers
// ================================================================================================

/**
 * @brief Add a child rank as a copy of a given rank and return it.
 *
 * If a child rank with the same name already exists, it is recursively merged with the given rank.
 */
Rank& Taxonomy::add_child( Rank const& child )
{
    return add_child_( Rank( child ));
}

/**
 * @brief Add a child rank by moving from a given rank and return it.
 *
 * If a child rank with the same name already exists, it is recursively merged with the given rank.
 */
Rank& Taxonomy::add_child( Rank&& child )
{
    return add_child_( std::move( child ));
}

/**
 * @brief Add a child rank by creating a new one with the given name and return it.
 *
 * If a child rank with the same name already exists, nothing happens.
 */
Rank& Taxonomy::add_child( std::string const& name )
{
    return add_child_( Rank( name ));
}

/**
 * @brief Remove a child rank with a certain name.
 *
 * The rank (and all its sub-ranks) are destroyed.
 * Throws an `std::runtime_error` if there is no rank with the given name.
 */
void Taxonomy::remove_child( std::string const& name )
{
    auto it = std::find_if(
        children_.begin(),
        children_.end(),
        [ &name ] ( Rank const& r ) {
            return r.name() == name;
        }
    );
    if( it == children_.end() ) {
        throw std::runtime_error( "Rank has no child named '" + name + "'." );
    }
    children_.erase( it );
}

// ================================================================================================
//     Iterators
// ================================================================================================

/**
 * @brief Return an iterator to the beginning of the child ranks.
 */
Taxonomy::iterator Taxonomy::begin()
{
    return children_.begin();
}

/**
 * @brief Return an iterator to the end of the child ranks.
 */
Taxonomy::iterator Taxonomy::end()
{
    return children_.end();
}

/**
 * @brief Return a const iterator to the beginning of the child ranks.
 */
Taxonomy::const_iterator Taxonomy::begin() const
{
    return children_.cbegin();
}

/**
 * @brief Return a const iterator to the end of the child ranks.
 */
Taxonomy::const_iterator Taxonomy::end() const
{
    return children_.cend();
}

/**
 * @brief Return a const iterator to the beginning of the child ranks.
 */
Taxonomy::const_iterator Taxonomy::cbegin() const
{
    return children_.cbegin();
}

/**
 * @brief Return a const iterator to the end of the child ranks.
 */
Taxonomy::const_iterator Taxonomy::cend() const
{
    return children_.cend();
}

// ================================================================================================
//     Protected Implementation Details
// ================================================================================================

/**
 * @brief Virtual implementation function for adding a child rank.
 *
 * This function is invoked by all add_child() functions in order to implement the non-virtual
 * interface pattern.
 * It is needed because adding a child rank differs for Taxonomy and Rank. In the latter case,
 * the additional @link Rank::parent() parent @endlink property has to be set. Thus, this function
 * is overridden by Rank, see Rank::add_child_().
 *
 * If a child rank with the same name already exists, it is recursively merged with the given rank.
 * The function returns the child.
 */
Rank& Taxonomy::add_child_( Rank&& child )
{
    // Check if a child rank with the given name already exists.
    for( auto& c : children_ ) {
        if( c.name() == child.name() ) {

            // If so, add the children of the new child to it (recursively), and return it.
            for( auto& child_children : child ) {
                c.add_child_( std::move( child_children ));
            }
            return c;
        }
    }

    // If not, add it as a a new child and return it.
    children_.push_back( std::move( child ));
    children_.back().parent_ = nullptr;
    return children_.back();
}

} // namespace taxonomy
} // namespace genesis
