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
 * @brief Implementation of Taxon class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "taxonomy/taxon.hpp"

#include <algorithm>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

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
//     Protected Implementation Details
// ================================================================================================

/**
 * @brief Virtual implementation function for adding a child taxon.
 *
 * See Taxonomy::add_child_() for details.
 */
Taxon& Taxon::add_child_( Taxon&& child )
{
    auto& c = Taxonomy::add_child_( std::move( child ));
    c.parent_ = this;
    return c;
}

} // namespace taxonomy
} // namespace genesis
