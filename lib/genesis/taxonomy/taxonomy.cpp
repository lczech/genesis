/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
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
   : names_( other.names_ )
{
    children_.resize( other.children_.size() );
    for( size_t i = 0; i < other.children_.size(); ++i ) {
        assert( other.children_[i] );
        children_[i] = utils::make_unique<Taxon>( *other.children_[i] );
    }
   reset_parent_pointers_( nullptr );
}

// =================================================================================================
//     Modifiers
// =================================================================================================

Taxon& Taxonomy::add_child( Taxon const& child )
{
    return add_child_( Taxon( child ));
}

Taxon& Taxonomy::add_child( Taxon&& child )
{
    return add_child_( std::move( child ));
}

Taxon& Taxonomy::add_child( std::string const& name )
{
    return add_child_( Taxon( name ));
}

Taxon& Taxonomy::add_child( std::string&& name )
{
    return add_child_( Taxon( std::move( name )));
}

void Taxonomy::remove_child( std::string const& name )
{
    // Find the name and remove it from both containers.
    auto it = names_.find( name );
    if( it == names_.end() ) {
        throw std::runtime_error( "Taxon has no child named '" + name + "'." );
    }
    assert( it->second < children_.size() );
    children_.erase( children_.begin() + it->second );
    names_.erase( it );
    reset_name_indicies_();
}

void Taxonomy::remove_at( size_t index )
{
    if( index >= children_.size() ) {
        throw std::runtime_error( "Invalid Taxon index." );
    }
    remove_child( children_[index]->name() );
}

// =================================================================================================
//     Internal Implementation Details
// =================================================================================================

Taxon& Taxonomy::add_child_( Taxon&& child )
{
    // Check if a child taxon with the given name already exists.
    auto it = names_.find( child.name() );
    if( it != names_.end() ) {
        assert( it->second < children_.size() );
        assert( children_[it->second] );
        auto& taxon = *children_[it->second];

        // Use the new taxon to fill in gaps in the given one.
        if( taxon.rank().empty() ) {
            taxon.rank( child.rank() );
        }
        if( taxon.id().empty() ) {
            taxon.id( child.id() );
        }

        // Check that the new taxon is compatible with the existing one.
        if(
            ( !child.rank().empty() && taxon.rank() != child.rank() ) ||
            ( !child.id().empty() &&   taxon.id()   != child.id()   )
        ) {
            throw std::invalid_argument(
                "Cannot add taxon with existing name '" + child.name() + "' to taxonomy, "
                "as the rank and/or id differ between the existing and new taxon entry: "
                "Rank '" + taxon.rank() + "' != '" + child.rank() + "', " +
                "ID '" + taxon.id() + "' != '" + child.id() + "'"
            );
        }

        // If we have a valid taxon in the taxonomy already,
        // add the children of the new child to it (recursively), and return it.
        for( auto& child_children : child ) {
            taxon.add_child_( std::move( child_children ));
        }
        return taxon;
    }

    // If not, add it as a a new child.
    names_[ child.name() ] = children_.size();
    children_.push_back( utils::make_unique<Taxon>( std::move( child )));
    children_.back()->parent_ = nullptr;
    return *children_.back();
}

void Taxonomy::reset_parent_pointers_( Taxon* parent )
{
    for( auto& taxon : children_ ) {
        taxon->parent_ = parent;
        // Probably don't need recursion here, as this function will be called for the sub-objects
        // anyway if needed.
        // The following line is left here in case it turns out we need it after all...
        // taxon.reset_parent_pointers_( &taxon );
    }
}

void Taxonomy::reset_name_indicies_()
{
    // Need to reset the indices of the names map.
    assert( children_.size() == names_.size() );
    for( size_t i = 0; i < children_.size(); ++i ) {
        auto it = names_.find( children_[i]->name() );
        assert( it != names_.end() );
        it->second = i;
    }
}

} // namespace taxonomy
} // namespace genesis
