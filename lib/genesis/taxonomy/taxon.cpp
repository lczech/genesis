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

/**
 * @brief Default constructor. Does nothing.
 */
Taxon::Taxon()
    : Taxonomy()
    , name_()
    , rank_()
    , parent_( nullptr )
    , data_( nullptr )
{}

/**
 * @brief Constructor that uses the given name for the Taxon.
 */
Taxon::Taxon( std::string const& name )
    : Taxonomy()
    , name_( name )
    , rank_()
    , parent_( nullptr )
    , data_( nullptr )
{}

/**
 * @brief Copy constructor.
 *
 * We need a custom version of this in order to set the Taxon::parent() pointers of all children
 * correctly when copying and to copy the data.
 *
 * Copying first sets the parent() pointer to `nullptr`. This is because we might want to get a
 * 'blank' copy, i.e., a Taxon that is not attached to a Taxonomy. This way, the functions
 * expecting a parent pointer don't get a potentially invalid pointer.
 *
 * If however we copy a Taxon into a Taxonomy (or into some other Taxon), reset_parent_pointers_()
 * is called later anyway, which then sets the parent pointer to the correct value.
 */
Taxon::Taxon( Taxon const& other )
    : Taxonomy( other )
    , name_( other.name_ )
    , rank_( other.rank_ )
    , parent_( nullptr )
{
    if( other.has_data() ) {
        reset_data( other.data_->clone() );
    }
    reset_parent_pointers_( this );
}

/**
 * @brief Move constructor.
 *
 * We need a custom version of this in order to set the Taxon::parent() pointers of all children
 * correctly, and to treat the data correctlty when copying.
 */
Taxon::Taxon( Taxon&& other )
    : Taxonomy( std::move( other ))
    , name_(    std::move( other.name_ ))
    , rank_(    std::move( other.rank_ ))
    , parent_(  other.parent_ )
    , data_(    std::move( other.data_ ))
{
    reset_parent_pointers_( this );
}

/**
 * @brief Copy assignment operator.
 *
 * We need a custom version of this in order to set the Taxon::parent() pointers of all children
 * correctly, and to treat the data correctlty when copying.
 *
 * See the @link Taxon( Taxon const& other ) move constructor@endlink for details.
 */
Taxon& Taxon::operator= ( Taxon const& other )
{
    Taxonomy::operator=( static_cast< Taxonomy const& >( other ));
    name_ = other.name_;
    rank_ = other.rank_;
    parent_ = nullptr;
    if( other.has_data() ) {
        reset_data( other.data_->clone() );
    }
    reset_parent_pointers_( this );
    return *this;
}

/**
 * @brief Move assignment operator.
 *
 * We need a custom version of this in order to set the Taxon::parent() pointers of all children
 * correctly, and to treat the data correctlty when copying.
 */
Taxon& Taxon::operator= ( Taxon&& other )
{
    Taxonomy::operator=( static_cast< Taxonomy&& >( std::move( other )));
    name_ = std::move( other.name_ );
    rank_ = std::move( other.rank_ );
    parent_ = other.parent_;
    data_ = std::move( other.data_ );
    reset_parent_pointers_( this );
    return *this;
}

/**
 * @brief Swapperator for Taxon.
 */
void swap( Taxon& lhs, Taxon& rhs )
{
    using std::swap;
    swap( static_cast< Taxonomy& >( lhs ), static_cast< Taxonomy& >( rhs ) );

    swap( lhs.name_,   rhs.name_ );
    swap( lhs.rank_,   rhs.rank_ );
    swap( lhs.parent_, rhs.parent_ );
    swap( lhs.data_,   rhs.data_ );
}

// ================================================================================================
//     Properties
// ================================================================================================

// -----------------------------------------------------
//     Name
// -----------------------------------------------------

/**
 * @brief Return the name of this taxon.
 */
std::string const& Taxon::name() const
{
    return name_;
}

/**
 * @brief Set the name of this taxon.
 */
void Taxon::name( std::string const& value )
{
    name_ = value;
}

// -----------------------------------------------------
//     Rank
// -----------------------------------------------------

/**
 * @brief Return the rank of this taxon.
 */
std::string const& Taxon::rank() const
{
    return rank_;
}

/**
 * @brief Set the rank of this taxon.
 */
void Taxon::rank( std::string const& value )
{
    rank_ = value;
}

// -----------------------------------------------------
//     Parent
// -----------------------------------------------------

/**
 * @brief Return a pointer to the parent of this taxon, or a `nullptr` if this is the top level taxon.
 */
Taxon const* Taxon::parent () const
{
    return parent_;
}

/**
 * @brief Return a pointer to the parent of this taxon, or a `nullptr` if this is the top level taxon.
 */
Taxon* Taxon::parent ()
{
    return parent_;
}

// ================================================================================================
//     Data
// ================================================================================================

/**
 * @brief Return `true` if the Taxon has a data object assigned to it.
 */
bool Taxon::has_data() const
{
    return data_.get() != nullptr;
}

/**
 * @brief Return a pointer to the data.
 *
 * In most cases, using data<>() is more convenient. However, in some cases, this function
 * might be necessary.
 */
BaseTaxonData* Taxon::data_ptr()
{
    return data_.get();
}

/**
 * @brief Return a const pointer to the data.
 *
 * In most cases, using data<>() is more convenient. However, in some cases, this function
 * might be necessary.
 */
BaseTaxonData const* Taxon::data_ptr() const
{
    return data_.get();
}

/**
 * @brief Reset the data pointer of this Taxon.
 *
 * Using this function, a Taxon can be assigend new data. It is also possible to change the
 * data type completely (as long as it derives from BaseTaxonData). Be however aware that many
 * functions that work with a Taxonomy expect a certain data type. Thus, changing it might break
 * those functions and lead to exceptions and other errors.
 */
Taxon& Taxon::reset_data( std::unique_ptr< BaseTaxonData > data )
{
    data_ = std::move( data );
    return *this;
}

// ================================================================================================
//     Protected Implementation Details
// ================================================================================================

/**
 * @brief Virtual implementation function for adding a child taxon.
 *
 * See Taxonomy::add_child_() for details. In addition to the base class implementation, this
 * function also sets the parent pointer of the Taxon.
 */
Taxon& Taxon::add_child_( Taxon const& child )
{
    auto& c = Taxonomy::add_child_( child );
    // c.parent_ = this;

    // We added to the container. This might have caused relocation of the contant.
    // Need to update parent pointers!
    reset_parent_pointers_( this );
    return c;
}

} // namespace taxonomy
} // namespace genesis
