/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/taxonomy/taxonomy.hpp"

#include "genesis/taxonomy/taxon.hpp"

#include <algorithm>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

Taxonomy::Taxonomy( Taxonomy const& other )
    : children_( other.children_ )
{
    reset_parent_pointers_( nullptr );
}

Taxonomy::Taxonomy( Taxonomy&& other )
    : children_( std::move( other.children_ ))
{
    reset_parent_pointers_( nullptr );
}

Taxonomy& Taxonomy::operator= ( Taxonomy const& other )
{
    children_ = other.children_;
    reset_parent_pointers_( nullptr );
    return *this;
}

Taxonomy& Taxonomy::operator= ( Taxonomy&& other )
{
    children_ = std::move( other.children_ );
    reset_parent_pointers_( nullptr );
    return *this;
}

/**
 * @brief Swapperator for Taxonomy.
 */
void swap( Taxonomy& lhs, Taxonomy& rhs )
{
    using std::swap;
    swap( lhs.children_, rhs.children_ );
}

// =================================================================================================
//     Accessors
// =================================================================================================

size_t Taxonomy::size() const
{
    return children_.size();
}

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

Taxon const& Taxonomy::get_child ( std::string name ) const
{
    for( auto const& c : children_ ) {
        if( c.name() == name ) {
            return c;
        }
    }
    throw std::runtime_error( "Taxon has no child named '" + name + "'." );
}

Taxon& Taxonomy::get_child ( std::string name )
{
    for( auto& c : children_ ) {
        if( c.name() == name ) {
            return c;
        }
    }
    throw std::runtime_error( "Taxon has no child named '" + name + "'." );
}

Taxon const& Taxonomy::operator [] ( std::string name ) const
{
    return get_child( name );
}

Taxon& Taxonomy::operator [] ( std::string name )
{
    return get_child( name );
}

Taxon const& Taxonomy::at ( size_t index ) const
{
    if( index >= children_.size() ) {
        throw std::invalid_argument(
            "Index out of bounds for accessing Taxonomy children: " + std::to_string( index ) +
            " >= " + std::to_string( children_.size() )
        );
    }
    auto it = std::next( children_.begin(), index );
    return *it;
}

Taxon& Taxonomy::at ( size_t index )
{
    if( index >= children_.size() ) {
        throw std::invalid_argument(
            "Index out of bounds for accessing Taxonomy children: " + std::to_string( index ) +
            " >= " + std::to_string( children_.size() )
        );
    }
    auto it = std::next( children_.begin(), index );
    return *it;
}

Taxon const& Taxonomy::operator [] ( size_t index ) const
{
    return at( index );
}

Taxon& Taxonomy::operator [] ( size_t index )
{
    return at( index );
}

size_t Taxonomy::index_of( std::string const& name ) const
{
    auto it = children_.begin();
    size_t i = 0;
    while( it != children_.end() ) {
        if( it->name() == name ) {
            return i;
        }
        ++it;
        ++i;
    }
    throw std::runtime_error( "Taxon has no child named '" + name + "'." );
}

// =================================================================================================
//     Modifiers
// =================================================================================================

Taxon& Taxonomy::add_child( Taxon const& child, bool merge_duplicates )
{
    return add_child_( child, merge_duplicates );
}

Taxon& Taxonomy::add_child( std::string const& name, bool merge_duplicates )
{
    return add_child_( Taxon( name ), merge_duplicates );
}

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

    // We probably don't need to call reset_parent_pointers_() here. The removal causes all
    // following elements in the container to move, so that their particular move constructors
    // (or assignment operators) are called, causing all their children to be updated. The
    // elemetns themselves do not need an update, as their parent didn't move!
    // (Hopefully, that's true... The tests don't fail, so that's a good sign!)
}

void Taxonomy::remove_at( size_t index )
{
    if( index >= children_.size() ) {
        throw std::runtime_error( "Invalid Taxon index." );
    }

    auto it = children_.begin();
    std::advance( it, index );
    children_.erase( it );
}

void Taxonomy::clear_children()
{
    children_.clear();
}

// =================================================================================================
//     Iterators
// =================================================================================================

Taxonomy::iterator Taxonomy::begin()
{
    return children_.begin();
}

Taxonomy::iterator Taxonomy::end()
{
    return children_.end();
}

Taxonomy::const_iterator Taxonomy::begin() const
{
    return children_.cbegin();
}

Taxonomy::const_iterator Taxonomy::end() const
{
    return children_.cend();
}

Taxonomy::const_iterator Taxonomy::cbegin() const
{
    return children_.cbegin();
}

Taxonomy::const_iterator Taxonomy::cend() const
{
    return children_.cend();
}

// =================================================================================================
//     Internal Implementation Details
// =================================================================================================

Taxon& Taxonomy::add_child_( Taxon const& child, bool merge_duplicates )
{
    // Check if a child taxon with the given name already exists.
    if( merge_duplicates ) {
        for( auto& c : children_ ) {
            if( c.name() == child.name() ) {

                // If so, add the children of the new child to it (recursively), and return it.
                for( auto& child_children : child ) {
                    c.add_child_( child_children, merge_duplicates );
                }
                return c;
            }
        }
    }

    // If not, add it as a a new child.
    children_.push_back( child );
    // children_.back().parent_ = nullptr;

    // We added to the container. This might have cause relocation of the contant.
    // Need to update parent pointers!
    reset_parent_pointers_( nullptr );
    return children_.back();
}

void Taxonomy::reset_parent_pointers_( Taxon* parent )
{
    for( auto& taxon : children_ ) {
        taxon.parent_ = parent;
        // Probably don't need recursion here, as this function will be called for the sub-objects
        // anyway if needed.
        // The following line is left here in case it turns out we need it after all...
        // taxon.reset_parent_pointers_( &taxon );
    }
}

} // namespace taxonomy
} // namespace genesis
