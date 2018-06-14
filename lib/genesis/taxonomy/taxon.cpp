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
 * @brief Implementation of Taxon class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxon.hpp"

#include <algorithm>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// ================================================================================================
//     Constructors and Rule of Five
// ================================================================================================

Taxon::Taxon()
    : Taxonomy()
    , name_()
    , rank_()
    , id_()
    , parent_( nullptr )
    , data_( nullptr )
{}

Taxon::Taxon( std::string const& name, std::string const& rank, std::string const& id )
    : Taxonomy()
    , name_( name )
    , rank_( rank )
    , id_( id )
    , parent_( nullptr )
    , data_( nullptr )
{}

Taxon::Taxon( Taxon const& other )
    : Taxonomy( other )
    , name_( other.name_ )
    , rank_( other.rank_ )
    , id_( other.id_ )
    , parent_( nullptr )
{
    if( other.has_data() ) {
        reset_data( other.data_->clone() );
    }
    reset_parent_pointers_( this );
}

Taxon::Taxon( Taxon&& other )
    : Taxonomy( std::move( other ))
    , name_(    std::move( other.name_ ))
    , rank_(    std::move( other.rank_ ))
    , id_(    std::move( other.id_ ))
    , parent_(  other.parent_ )
    , data_(    std::move( other.data_ ))
{
    reset_parent_pointers_( this );
}

Taxon& Taxon::operator= ( Taxon const& other )
{
    Taxonomy::operator=( static_cast< Taxonomy const& >( other ));
    name_ = other.name_;
    rank_ = other.rank_;
    id_ = other.id_;
    parent_ = nullptr;
    if( other.has_data() ) {
        reset_data( other.data_->clone() );
    }
    reset_parent_pointers_( this );
    return *this;
}

Taxon& Taxon::operator= ( Taxon&& other )
{
    Taxonomy::operator=( static_cast< Taxonomy&& >( std::move( other )));
    name_ = std::move( other.name_ );
    rank_ = std::move( other.rank_ );
    id_ = std::move( other.id_ );
    parent_ = other.parent_;
    data_ = std::move( other.data_ );
    reset_parent_pointers_( this );
    return *this;
}

void swap( Taxon& lhs, Taxon& rhs )
{
    using std::swap;
    swap( static_cast< Taxonomy& >( lhs ), static_cast< Taxonomy& >( rhs ) );

    swap( lhs.name_,   rhs.name_ );
    swap( lhs.rank_,   rhs.rank_ );
    swap( lhs.id_,   rhs.id_ );
    swap( lhs.parent_, rhs.parent_ );
    swap( lhs.data_,   rhs.data_ );
}

// ================================================================================================
//     Properties
// ================================================================================================

// -----------------------------------------------------
//     Name
// -----------------------------------------------------

std::string const& Taxon::name() const
{
    return name_;
}

void Taxon::name( std::string const& value )
{
    name_ = value;
}

// -----------------------------------------------------
//     Rank
// -----------------------------------------------------

std::string const& Taxon::rank() const
{
    return rank_;
}

void Taxon::rank( std::string const& value )
{
    rank_ = value;
}

// -----------------------------------------------------
//     ID
// -----------------------------------------------------

std::string const& Taxon::id() const
{
    return id_;
}

void Taxon::id( std::string const& value )
{
    id_ = value;
}

// -----------------------------------------------------
//     Parent
// -----------------------------------------------------

Taxon const* Taxon::parent () const
{
    return parent_;
}

Taxon* Taxon::parent ()
{
    return parent_;
}

// ================================================================================================
//     Data
// ================================================================================================

bool Taxon::has_data() const
{
    return data_.get() != nullptr;
}

BaseTaxonData* Taxon::data_ptr()
{
    return data_.get();
}

BaseTaxonData const* Taxon::data_ptr() const
{
    return data_.get();
}

Taxon& Taxon::reset_data( std::unique_ptr< BaseTaxonData > data )
{
    data_ = std::move( data );
    return *this;
}

// ================================================================================================
//     Protected Implementation Details
// ================================================================================================

Taxon& Taxon::add_child_( Taxon const& child, bool merge_duplicates )
{
    auto& c = Taxonomy::add_child_( child, merge_duplicates );
    // c.parent_ = this;

    // We added to the container. This might have caused relocation of the contant.
    // Need to update parent pointers!
    reset_parent_pointers_( this );
    return c;
}

} // namespace taxonomy
} // namespace genesis
