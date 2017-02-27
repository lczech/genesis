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
 * @brief Implementation of Sample class.
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/sample.hpp"

#include "genesis/placement/function/helper.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/utils/core/std.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <unordered_map>
#include <utility>

namespace genesis {
namespace placement {

// =================================================================================================
//     Constructor and Rule of Five
// =================================================================================================

Sample::Sample( PlacementTree const& tree )
    : tree_( tree )
{
    if( ! tree::tree_data_is< PlacementNodeData, PlacementEdgeData >( tree_ ) ) {
        throw std::runtime_error( "Tree for constructing the Sample is no PlacementTree." );
    }
}

Sample::Sample( Sample const& other )
{
    // We need this custom ctor because the placements contain pointers to their edges.
    // As the whole tree is copied, the pointers need to be adjusted to that new tree.

    // First, use normal copy assignment for the data structures.
    pqueries_ = other.pqueries_;
    tree_     = other.tree_;
    metadata  = other.metadata;

    // Now adjust all placement to edge pointers.
    for( auto& pqry : pqueries_ ) {
        for( auto& place : pqry.placements() ) {
            // Get the index using the pointer to the (still valid) old edge.
            // (Remember: the placement is still pointing to the old edge at that point.)
            auto index = place.edge().index();

            // Now set the pointer of the placement to the edge of the new tree.
            place.reset_edge( tree_.edge_at( index ) );
        }
    }
}

Sample& Sample::operator= ( Sample const& other )
{
    // As we need a custom copy constructor, it is proably safer to also use a custom copy
    // assignment, just to make sure that it actually does the copying part correctly.
    // Use copy swap idiom.
    auto tmp = Sample( other );
    this->swap( tmp );
    return *this;
}

void Sample::swap( Sample& other )
{
    using std::swap;
    swap( pqueries_, other.pqueries_ );
    swap( tree_,     other.tree_ );
    swap( metadata,  other.metadata );
}

// =================================================================================================
//     Modifiers
// =================================================================================================

void Sample::clear()
{
    pqueries_.clear();
    tree_ = PlacementTree();
    metadata.clear();
}

// =================================================================================================
//     Tree Accessors and Modifiers
// =================================================================================================

PlacementTree& Sample::tree()
{
    return tree_;
}

PlacementTree const& Sample::tree() const
{
    return tree_;
}

// =================================================================================================
//     Pquery Accessors and Modifiers
// =================================================================================================

size_t Sample::size() const
{
    return pqueries_.size();
}

bool Sample::empty() const
{
    return tree_.empty() || pqueries_.empty();
}

// -------------------------------------------------------------------------
//     Add
// -------------------------------------------------------------------------

Pquery& Sample::add()
{
    pqueries_.push_back( Pquery() );
    return pqueries_.back();
}

Pquery& Sample::add( Pquery const& other )
{
    pqueries_.push_back( other );

    // Adjust the edge pointers of the placements.
    for( auto& place : pqueries_.back().placements() ) {
        // Get the edge index of the old edge, then set the edge to the edge of the
        // correct sample that is at that index.
        auto const& old_edge_data  = place.edge().data<PlacementEdgeData>();
        auto const edge_index      = place.edge().index();
        auto const old_edge_num    = old_edge_data.edge_num();
        auto const rel_pos         = place.proximal_length / old_edge_data.branch_length;
        place.reset_edge( tree().edge_at( edge_index ));

        // Now the placement points to the new edge. We can thus check if this one still has the
        // same edge_num as the old edge.
        auto const& edge_data = place.edge().data<PlacementEdgeData>();
        if( old_edge_num != edge_data.edge_num() ) {
            throw std::runtime_error(
                "Trees are incompatible for copying Pqueries between Samples."
            );
        }
        place.proximal_length = rel_pos * edge_data.branch_length;
    }

    return pqueries_.back();
}

// -------------------------------------------------------------------------
//     At
// -------------------------------------------------------------------------

Pquery& Sample::at( const size_t index )
{
    return pqueries_.at( index );
}

Pquery const& Sample::at( const size_t index ) const
{
    return pqueries_.at( index );
}

// -------------------------------------------------------------------------
//     Remove
// -------------------------------------------------------------------------

void Sample::remove( size_t index )
{
    pqueries_.erase( pqueries_.begin() + index );
}

void Sample::remove( size_t first_index, size_t last_index )
{
    if( first_index    >= pqueries_.size()
        || last_index  >= pqueries_.size()
        || first_index >= last_index
    ) {
        throw std::out_of_range( "Invalid indices for removing from SequenceSet." );
    }

    pqueries_.erase( pqueries_.begin() + first_index, pqueries_.begin() + last_index );
}

void Sample::remove( iterator_pqueries position )
{
    pqueries_.erase( position );
}

void Sample::remove( iterator_pqueries first, iterator_pqueries last )
{
    pqueries_.erase( first, last );
}

// -------------------------------------------------------------------------
//     Clear
// -------------------------------------------------------------------------

void Sample::clear_pqueries()
{
    pqueries_.clear();
}

// =================================================================================================
//     Pquery Iterator
// =================================================================================================

Sample::iterator_pqueries Sample::begin()
{
    return pqueries_.begin();
}

Sample::const_iterator_pqueries Sample::begin() const
{
    return pqueries_.begin();
}

Sample::iterator_pqueries Sample::end()
{
    return pqueries_.end();
}

Sample::const_iterator_pqueries Sample::end() const
{
    return pqueries_.end();
}

utils::Range<Sample::iterator_pqueries> Sample::pqueries()
{
    return { pqueries_ };
}

utils::Range<Sample::const_iterator_pqueries> Sample::pqueries() const
{
    return { pqueries_ };
}

} // namespace placement
} // namespace genesis
