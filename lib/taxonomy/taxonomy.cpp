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

#include "taxonomy/taxon.hpp"

#include <algorithm>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// ================================================================================================
//     Accessors
// ================================================================================================

/**
 * @brief Return the number of immediate child @link Taxon Taxa@endlink.
 *
 * See total_taxa_count() for counting all Taxa, including all nested ones.
 */
size_t Taxonomy::size() const
{
    return children_.size();
}

/**
 * @brief Return whether an immediate child taxon with the given name exists.
 */
bool Taxonomy::has_child ( std::string name ) const
{
    return children_.end() != std::find_if(
        children_.begin(),
        children_.end(),
        [ &name ] ( Taxon const& r ) {
            return r.name() == name;
        }
    );
}

/**
 * @brief Return the child taxon with a given name if it exists, or throw otherwise.
 */
Taxon const& Taxonomy::get_child ( std::string name ) const
{
    for( auto const& c : children_ ) {
        if( c.name() == name ) {
            return c;
        }
    }
    throw std::runtime_error( "Taxon has no child named '" + name + "'." );
}

/**
 * @brief Return the child taxon with a given name if it exists, or throw otherwise.
 */
Taxon& Taxonomy::get_child ( std::string name )
{
    for( auto& c : children_ ) {
        if( c.name() == name ) {
            return c;
        }
    }
    throw std::runtime_error( "Taxon has no child named '" + name + "'." );
}

/**
 * @brief Return the child taxon with a given name if it exists, or throw otherwise.
 */
Taxon const& Taxonomy::operator [] ( std::string name ) const
{
    return get_child( name );
}

/**
 * @brief Return the child taxon with a given name if it exists, or throw otherwise.
 */
Taxon& Taxonomy::operator [] ( std::string name )
{
    return get_child( name );
}

// ================================================================================================
//     Modifiers
// ================================================================================================

/**
 * @brief Add a child taxon as a copy of a given taxon and return it.
 *
 * If a child taxon with the same name already exists, it is recursively merged with the given taxon.
 */
Taxon& Taxonomy::add_child( Taxon const& child )
{
    // We create a copy here and move it. Instead, we could also have taken the Taxon by value,
    // but then the interface is inconsistent, as we usually take by const ref.
    return add_child_( Taxon( child ));
}

/**
 * @brief Add a child taxon by moving from a given taxon and return it.
 *
 * If a child taxon with the same name already exists, it is recursively merged with the given taxon.
 */
Taxon& Taxonomy::add_child( Taxon&& child )
{
    return add_child_( std::move( child ));
}

/**
 * @brief Add a child taxon by creating a new one with the given name and return it.
 *
 * If a child taxon with the same name already exists, nothing happens.
 */
Taxon& Taxonomy::add_child( std::string const& name )
{
    return add_child_( Taxon( name ));
}

/**
 * @brief Remove a child taxon with a certain name.
 *
 * The taxon (and all its sub-taxa) are destroyed.
 * Throws an `std::runtime_error` if there is no taxon with the given name.
 */
void Taxonomy::remove_child( std::string const& name )
{
    auto it = std::find_if(
        children_.begin(),
        children_.end(),
        [ &name ] ( Taxon const& r ) {
            return r.name() == name;
        }
    );
    if( it == children_.end() ) {
        throw std::runtime_error( "Taxon has no child named '" + name + "'." );
    }
    children_.erase( it );
}

/**
 * @brief Remove all children.
 */
void Taxonomy::clear_children()
{
    children_.clear();
}

// ================================================================================================
//     Iterators
// ================================================================================================

/**
 * @brief Return an iterator to the beginning of the child taxa.
 */
Taxonomy::iterator Taxonomy::begin()
{
    return children_.begin();
}

/**
 * @brief Return an iterator to the end of the child taxa.
 */
Taxonomy::iterator Taxonomy::end()
{
    return children_.end();
}

/**
 * @brief Return a const iterator to the beginning of the child taxa.
 */
Taxonomy::const_iterator Taxonomy::begin() const
{
    return children_.cbegin();
}

/**
 * @brief Return a const iterator to the end of the child taxa.
 */
Taxonomy::const_iterator Taxonomy::end() const
{
    return children_.cend();
}

/**
 * @brief Return a const iterator to the beginning of the child taxa.
 */
Taxonomy::const_iterator Taxonomy::cbegin() const
{
    return children_.cbegin();
}

/**
 * @brief Return a const iterator to the end of the child taxa.
 */
Taxonomy::const_iterator Taxonomy::cend() const
{
    return children_.cend();
}

// ================================================================================================
//     Protected Implementation Details
// ================================================================================================

/**
 * @brief Virtual implementation function for adding a child taxon.
 *
 * This function is invoked by all add_child() functions in order to implement the non-virtual
 * interface pattern.
 * It is needed because adding a child taxon differs for Taxonomy and Taxon. In the latter case,
 * the additional @link Taxon::parent() parent @endlink property has to be set. Thus, this function
 * is overridden by Taxon, see Taxon::add_child_().
 *
 * If a child taxon with the same name already exists, it is recursively merged with the given taxon.
 * The function returns the child.
 */
Taxon& Taxonomy::add_child_( Taxon&& child )
{
    // Check if a child taxon with the given name already exists.
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
