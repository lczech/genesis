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

/**
 * @brief Clear all data stored in this Pquery, i.e., clear all PqueryName%s and all
 * PqueryPlacement%s.
 */
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

/**
 * @brief Return a Range iterator to the PqueryPlacement%s.
 *
 * This makes iterating placements via a range based for loop easy.
 */
utils::Range<Pquery::iterator_placements> Pquery::placements()
{
    return { placements_ };
}

/**
* @brief Return a const Range iterator to the PqueryPlacement%s.
*
* This makes iterating placements via a range based for loop easy.
*/
utils::Range<Pquery::const_iterator_placements> Pquery::placements() const
{
    return { placements_ };
}

// =============================================================================
//     Placement Accessors and Modifiers
// =============================================================================

/**
 * @brief Create a new PqueryPlacement at a given PlacementTreeEdge, add it to the Pquery and
 * return it.
 *
 * The values of the placement can then be adjusted using the returned object reference.
 *
 * It is important that the provided edge belongs to the same PlacementTree as the Pquery and
 * its containing Sample do. This is up to the user and not checked.
 *
 * As this function might reallocate the memory where placements are stored, all iterators
 * and pointer to PqueryPlacement%s are invalidated.
 */
PqueryPlacement& Pquery::add_placement( PlacementTreeEdge& edge )
{
    placements_.emplace_back( edge );
    return placements_.back();
}

/**
 * @brief Create a new PqueryPlacement as a copy of the provided one, add it to the Pquery and
 * return it.
 *
 * As this function might reallocate the memory where placements are stored, all iterators
 * and pointer to PqueryPlacement%s are considered to be invalidated.
 */
PqueryPlacement& Pquery::add_placement( PqueryPlacement const& val )
{
    placements_.emplace_back( val );
    return placements_.back();
}

/**
 * @brief Create a new PqueryPlacement at a given PlacementTreeEdge, add it to the Pquery and
 * return it. The property values of the provided PqueryPlacement are copied.
 *
 * It is important that the provided edge belongs to the same PlacementTree as the Pquery and
 * its containing Sample do. This is up to the user and not checked.
 *
 * As this function might reallocate the memory where placements are stored, all iterators
 * and pointer to PqueryPlacement%s are considered to be invalidated.
 */
PqueryPlacement& Pquery::add_placement(
    PlacementTreeEdge    & edge,
    PqueryPlacement const& val
) {
    placements_.emplace_back( val );
    placements_.back().reset_edge( edge );
    return placements_.back();
}

/**
 * @brief Return the number of PqueryPlacement%s stored in this Pquery.
 */
size_t Pquery::placement_size() const
{
    return placements_.size();
}

/**
 * @brief Return the PqueryPlacement at a certain index.
 *
 * The index must be smaller than placement_size(), otherwise this functions throws an exception.
 */
PqueryPlacement& Pquery::placement_at( size_t index )
{
    return placements_[index];
}

/**
 * @brief Return the PqueryPlacement at a certain index.
 *
 * The index must be smaller than placement_size(), otherwise this functions throws an exception.
 */
PqueryPlacement const& Pquery::placement_at( size_t index ) const
{
    return placements_[index];
}

/**
 * @brief Remove the PqueryPlacement at a certain index position within this Pquery.
 *
 * As this function moves placements in the container, all iterators and pointer to
 * PqueryPlacement%s are considered to be invalidated.
 */
void Pquery::remove_placement_at( size_t index )
{
    placements_.erase( placements_.begin() + index );
}

/**
 * @brief Delete all PqueryPlacement%s of this Pquery.
 */
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

/**
* @brief Return a Range iterator to the PqueryName%s.
*
* This makes iterating placements via a range based for loop easy.
*/
utils::Range<Pquery::iterator_names> Pquery::names()
{
    return { names_ };
}

/**
* @brief Return a const Range iterator to the PqueryName%s.
*
* This makes iterating placements via a range based for loop easy.
*/
utils::Range<Pquery::const_iterator_names> Pquery::names() const
{
    return { names_ };
}

// =============================================================================
//     Name Accessors and Modifiers
// =============================================================================

/**
 * @brief Create a new PqueryName using the provided parameters, add it to the Pquery and return it.
 *
 * As this function might reallocate the memory where names are stored, all iterators
 * and pointer to PqueryNames%s are considered to be invalidated.
 */
PqueryName& Pquery::add_name( std::string name, double multiplicity )
{
    names_.emplace_back( name, multiplicity );
    return names_.back();
}

/**
 * @brief Create a new PqueryName as a copy of the provided one, add it to the Pquery and return it.
 *
 * As this function might reallocate the memory where names are stored, all iterators
 * and pointer to PqueryNames%s are considered to be invalidated.
 */
PqueryName& Pquery::add_name( PqueryName const& other )
{
    names_.push_back( PqueryName(other) );
    return names_.back();
}

/**
 * @brief Return the number of PqueryName%s stored in this Pquery.
 */
size_t Pquery::name_size() const
{
    return names_.size();
}

/**
 * @brief Return the PqueryName at a certain index.
 *
 * The index must be smaller than name_size(), otherwise this functions throws an exception.
 */
PqueryName& Pquery::name_at( size_t index )
{
    return names_.at(index);
}

/**
 * @brief Return the PqueryName at a certain index.
 *
 * The index must be smaller than name_size(), otherwise this functions throws an exception.
 */
PqueryName const& Pquery::name_at( size_t index ) const
{
    return names_.at(index);
}

/**
 * @brief Remove the PqueryName at a certain index position within this Pquery.
 *
 * As this function moves names in the container, all iterators and pointer to PqueryNames%s
 * are considered to be invalidated.
 */
void Pquery::remove_name_at( size_t index )
{
    names_.erase( names_.begin() + index );
}

/**
 * @brief Delete all PqueryName%s of this Pquery.
 */
void Pquery::clear_names()
{
    names_.clear();
}

} // namespace placement
} // namespace genesis
