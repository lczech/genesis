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
 * @brief Implementation of Pquery class.
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/pquery.hpp"

#include "genesis/placement/placement_tree.hpp"
#include "genesis/utils/core/std.hpp"

namespace genesis {
namespace placement {

// =============================================================================
//     General Modifiers
// =============================================================================

void Pquery::clear()
{
    clear_placements();
    clear_names();
}

// =============================================================================
//     Placement Iterators
// =============================================================================

Pquery::iterator_placements Pquery::begin_placements()
{
    return placements_.begin();
}

Pquery::iterator_placements Pquery::end_placements()
{
    return placements_.end();
}

Pquery::const_iterator_placements Pquery::begin_placements() const
{
    return placements_.cbegin();
}

Pquery::const_iterator_placements Pquery::end_placements() const
{
    return placements_.cend();
}

utils::Range<Pquery::iterator_placements> Pquery::placements()
{
    return { placements_ };
}

utils::Range<Pquery::const_iterator_placements> Pquery::placements() const
{
    return { placements_ };
}

std::vector<PqueryPlacement>& Pquery::expose_placements()
{
    return placements_;
}

// =============================================================================
//     Placement Accessors and Modifiers
// =============================================================================

PqueryPlacement& Pquery::add_placement( PlacementTreeEdge& edge )
{
    placements_.emplace_back( edge );
    return placements_.back();
}

PqueryPlacement& Pquery::add_placement( PqueryPlacement const& val )
{
    placements_.emplace_back( val );
    return placements_.back();
}

PqueryPlacement& Pquery::add_placement(
    PlacementTreeEdge    & edge,
    PqueryPlacement const& val
) {
    placements_.emplace_back( val );
    placements_.back().reset_edge( edge );
    return placements_.back();
}

size_t Pquery::placement_size() const
{
    return placements_.size();
}

PqueryPlacement& Pquery::placement_at( size_t index )
{
    return placements_[index];
}

PqueryPlacement const& Pquery::placement_at( size_t index ) const
{
    return placements_[index];
}

void Pquery::remove_placement_at( size_t index )
{
    placements_.erase( placements_.begin() + index );
}

void Pquery::clear_placements()
{
    placements_.clear();
}

// =============================================================================
//     Name Iterators
// =============================================================================

Pquery::iterator_names Pquery::begin_names()
{
    return names_.begin();
}

Pquery::iterator_names Pquery::end_names()
{
    return names_.end();
}

Pquery::const_iterator_names Pquery::begin_names() const
{
    return names_.cbegin();
}

Pquery::const_iterator_names Pquery::end_names() const
{
    return names_.cend();
}

utils::Range<Pquery::iterator_names> Pquery::names()
{
    return { names_ };
}

utils::Range<Pquery::const_iterator_names> Pquery::names() const
{
    return { names_ };
}

std::vector<PqueryName>& Pquery::expose_names()
{
    return names_;
}

// =============================================================================
//     Name Accessors and Modifiers
// =============================================================================

PqueryName& Pquery::add_name( std::string name, double multiplicity )
{
    names_.emplace_back( name, multiplicity );
    return names_.back();
}

PqueryName& Pquery::add_name( PqueryName const& other )
{
    names_.push_back( PqueryName(other) );
    return names_.back();
}

size_t Pquery::name_size() const
{
    return names_.size();
}

PqueryName& Pquery::name_at( size_t index )
{
    return names_.at(index);
}

PqueryName const& Pquery::name_at( size_t index ) const
{
    return names_.at(index);
}

void Pquery::remove_name_at( size_t index )
{
    names_.erase( names_.begin() + index );
}

void Pquery::clear_names()
{
    names_.clear();
}

} // namespace placement
} // namespace genesis
