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

/**
 * @brief Copy constructor.
 *
 * We need a custom version of this in order to set the Taxon::parent() pointers of all children
 * correctly when copying.
 */
Taxonomy::Taxonomy( Taxonomy const& other )
    : children_( other.children_ )
{
    reset_parent_pointers_( nullptr );
}

/**
 * @brief Move constructor.
 *
 * We need a custom version of this in order to set the Taxon::parent() pointers of all children
 * correctly when copying.
 */
Taxonomy::Taxonomy( Taxonomy&& other )
    : children_( std::move( other.children_ ))
{
    reset_parent_pointers_( nullptr );
}

/**
 * @brief Copy assigment operator.
 *
 * We need a custom version of this in order to set the Taxon::parent() pointers of all children
 * correctly when copying.
 */
Taxonomy& Taxonomy::operator= ( Taxonomy const& other )
{
    children_ = other.children_;
    reset_parent_pointers_( nullptr );
    return *this;
}

/**
 * @brief Move assignment operator.
 *
 * We need a custom version of this in order to set the Taxon::parent() pointers of all children
 * correctly when copying.
 */
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
 * @brief Return whether an immediate child Taxon with the given name exists.
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
 * @brief Return the child Taxon with a given name if it exists, or throw otherwise.
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
 * @brief Return the child Taxon with a given name if it exists, or throw otherwise.
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
 * @brief Return the child Taxon with a given name if it exists, or throw otherwise.
 */
Taxon const& Taxonomy::operator [] ( std::string name ) const
{
    return get_child( name );
}

/**
 * @brief Return the child Taxon with a given name if it exists, or throw otherwise.
 */
Taxon& Taxonomy::operator [] ( std::string name )
{
    return get_child( name );
}

/**
 * @brief Return the child Taxon at the given index.
 *
 * The function throws an exception if the index in invalid, i.e., `>=` size().
 */
Taxon const& Taxonomy::at ( size_t index  ) const
{
    return children_.at( index );
}

/**
 * @brief Return the child Taxon at the given index.
 *
 * The function throws an exception if the index in invalid, i.e., `>=` size().
 */
Taxon& Taxonomy::at ( size_t index  )
{
    return children_.at( index );
}

/**
 * @brief Return the child Taxon at the given index.
 *
 * The function does not check whether the provided index is within the valid range of size().
 */
Taxon const& Taxonomy::operator [] ( size_t index ) const
{
    return children_[ index ];
}

/**
 * @brief Return the child Taxon at the given index.
 *
 * The function does not check whether the provided index is within the valid range of size().
 */
Taxon& Taxonomy::operator [] ( size_t index )
{
    return children_[ index ];
}

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Add a child Taxon as a copy of a given Taxon and return it.
 *
 * If a child Taxon with the same name already exists, it is recursively merged with the given Taxon.
 */
Taxon& Taxonomy::add_child( Taxon const& child )
{
    return add_child_( child );
}

/**
 * @brief Add a child Taxon by creating a new one with the given name and return it.
 *
 * If a child Taxon with the same name already exists, nothing happens.
 */
Taxon& Taxonomy::add_child( std::string const& name )
{
    return add_child_( Taxon( name ));
}

/**
 * @brief Remove a child Taxon with a certain name.
 *
 * The Taxon (and all its sub-taxa) are destroyed.
 * Throws an `std::runtime_error` if there is no Taxon with the given name.
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

    // We probably don't need to call reset_parent_pointers_() here. The removal causes all
    // following elements in the container to move, so that their particular move constructors
    // (or assignment operators) are called, causing all their children to be updated. The
    // elemetns themselves do not need an update, as their parent didn't move!
    // (Hopefully, that's true... The tests don't fail, so that's a good sign!)
}

/**
 * @brief Remove all children.
 */
void Taxonomy::clear_children()
{
    children_.clear();
}

// =================================================================================================
//     Iterators
// =================================================================================================

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

// =================================================================================================
//     Internal Implementation Details
// =================================================================================================

/**
 * @brief Virtual implementation function for adding a child Taxon.
 *
 * This function is invoked by all add_child() functions in order to implement the non-virtual
 * interface pattern.
 *
 * It needs to be virtual because adding a child Taxon differs for Taxonomy and Taxon.
 * In the latter case, the additional @link Taxon::parent() parent @endlink property has to be set.
 * Thus, this function is overridden by Taxon, see Taxon::add_child_().
 *
 * If a child Taxon with the same name already exists, it is recursively merged with the given Taxon.
 * The function returns the child.
 */
Taxon& Taxonomy::add_child_( Taxon const& child )
{
    // Check if a child taxon with the given name already exists.
    for( auto& c : children_ ) {
        if( c.name() == child.name() ) {

            // If so, add the children of the new child to it (recursively), and return it.
            for( auto& child_children : child ) {
                c.add_child_( child_children );
            }
            return c;
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

/**
 * @brief Internal helper function that resets the parent pointer of all stored Taxa.
 *
 * This function is used whenever the `children_` container is changed (copy, move, add elements)
 * in order to make sure that all parent pointers are correct.
 *
 * As all constructors and assignment operators use this function, we also make sure that
 * adding or removing elements or changing their order (sort etc) does not break the correctness of
 * the parent pointers. This might be slightly inefficient if many such moves (and thus, reset
 * operations) are done in sequence. However, this is acceptable in order to keep the object
 * stable at any time.
 */
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
