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
 * @brief Implementation of Sample class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample.hpp"

#include "placement/function/helper.hpp"

#include "tree/iterator/preorder.hpp"
#include "tree/function/operators.hpp"

#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

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

/**
 * @brief Constructor taking a reference tree.
 *
 * The tree has to have the data types PlacementNodeData and PlacementEdgeData at its nodes and
 * edges, respectively. If not, an exception is thrown.
 */
Sample::Sample( PlacementTree const& tree )
    : tree_( tree )
{
    if( ! tree::tree_data_is< PlacementNodeData, PlacementEdgeData >( tree_ ) ) {
        throw std::runtime_error( "Tree for constructing the Sample is no PlacementTree." );
    }
}

/**
 * @brief Copy constructor.
 */
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

/**
 * @brief Copy assignment.
 */
Sample& Sample::operator= ( Sample const& other )
{
    // As we need a custom copy constructor, it is proably safer to also use a custom copy
    // assignment, just to make sure that it actually does the copying part correctly.
    // Use copy swap idiom.
    auto tmp = Sample( other );
    this->swap( tmp );
    return *this;
}

/**
 * @brief Swap the contents of this Sample with the contents of another Sample.
 */
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

/**
 * @brief Clears all data of this object.
 *
 * The @link Pquery Pqueries @endlink, the PlacementTree and the #metadata are deleted.
 */
void Sample::clear()
{
    pqueries_.clear();
    tree_ = PlacementTree();
    metadata.clear();
}

// =================================================================================================
//     Tree Accessors and Modifiers
// =================================================================================================

/**
 * @brief Get the PlacementTree of this Sample.
 */
PlacementTree& Sample::tree()
{
    return tree_;
}

/**
 * @brief Get the PlacementTree of this Sample.
 */
PlacementTree const& Sample::tree() const
{
    return tree_;
}

// =================================================================================================
//     Pquery Accessors and Modifiers
// =================================================================================================

/**
 * @brief Create an empty Pquery, add it to the Sample and return it.
 *
 * The returned reference can then be used to add PqueryPlacement%s and PqueryName%s to the Pquery.
 *
 * As this function might reallocate the memory where Pqueries are stored, all iterators
 * and pointers to the Pqueries of this Sample are invalidated.
 */
Pquery& Sample::add_pquery()
{
    pqueries_.push_back( Pquery() );
    return pqueries_.back();
}

/**
 * @brief Create a Pquery as a copy of the provided one, add it to the sample and return it.
 *
 * It is possible to provide a Pquery from a different Sample, as long as the tree topology of both
 * Sample%s is identical, including identical @link PlacementTreeEdge::index() edge indices @endlink
 * and @link PqueryPlacement::edge_num edge_nums@endlink. For that purpose, this function
 * automatically adjusts the internal pointers of the Pquery and its PqueryPlacement%s accordingly.
 *
 * However, if the trees are incompatible (i.e., have a different topology, indices or edge nums),
 * the PqueryPlacement%s will either point to different edges or the function might throw an
 * exception, in cases where the tree does not have a corresponding edge at all. To further ensure
 * correct behaviour, the function also checks whether the @link PqueryPlacement::edge_num
 * edge_num @endlink is the same for the edge of the original PqueryPlacement and the new one,
 * and throws an std::runtime_error if not.
 *
 * As this function might reallocate the memory where Pqueries are stored, all iterators
 * and pointers to the Pqueries of this Sample are invalidated.
 */
Pquery& Sample::add_pquery( Pquery const& other )
{
    pqueries_.push_back( other );

    // Adjust the edge pointers of the placements.
    for( auto& placement : pqueries_.back().placements() ) {
        // Get the edge index of the old edge, then set the edge to the edge of the
        // correct sample that is at that index.
        auto edge_index   = placement.edge().index();
        auto old_edge_num = placement.edge().data<PlacementEdgeData>().edge_num();
        placement.reset_edge( tree().edge_at( edge_index ));

        // Now the placement points to the new edge. We can thus check if this one still has the
        // same edge_num as the old edge.
        auto const& edge_data = placement.edge().data<PlacementEdgeData>();
        if( old_edge_num != edge_data.edge_num() ) {
            throw std::runtime_error(
                "Trees are incompatible for copying Pqueries between Samples."
            );
        }
    }

    return pqueries_.back();
}

/**
 * @brief Return the number of @link Pquery Pqueries @endlink that are stored in this Sample.
 */
size_t Sample::pquery_size() const
{
    return pqueries_.size();
}

/**
 * @brief Return the Pquery at a certain index.
 */
Pquery& Sample::pquery_at( const size_t index )
{
    return pqueries_.at( index );
}

/**
 * @brief Return the Pquery at a certain index.
 */
Pquery const& Sample::pquery_at( const size_t index ) const
{
    return pqueries_.at( index );
}

/**
 * @brief Remove the Pquery at a certain index position within this Sample.
 *
 * As this function might reallocate the memory where Pqueries are stored, all iterators
 * and pointer to the Pqueries of this Sample are considered to be invalidated.
 */
void Sample::remove_pquery_at( size_t index )
{
    pqueries_.erase( pqueries_.begin() + index );
}

/**
 * @brief Clear all @link Pquery Pqueries @endlink of this Sample.
 *
 * All Pqueries are deleted. However, the PlacementTree and the #metadata are left as they are.
 * Thus this is a useful method for e.g., simulating placements: Take a copy of a given sample,
 * clear its Pqueries, then generate new ones using the Simulator.
 */
void Sample::clear_pqueries()
{
    pqueries_.clear();
}

// =================================================================================================
//     Pquery Iterator
// =================================================================================================

/**
 * @brief Return an iterator to the beginning of the @link Pquery Pqueries @endlink of this Sample.
 */
Sample::iterator_pqueries Sample::begin()
{
    return pqueries_.begin();
}

/**
 * @brief Return a constiterator to the beginning of the @link Pquery Pqueries @endlink of this
 * Sample.
 */
Sample::const_iterator_pqueries Sample::begin() const
{
    return pqueries_.begin();
}

/**
 * @brief Return an iterator to the end of the @link Pquery Pqueries @endlink of this Sample.
 */
Sample::iterator_pqueries Sample::end()
{
    return pqueries_.end();
}

/**
 * @brief Return a const iterator to the end of the @link Pquery Pqueries @endlink of this Sample.
 */
Sample::const_iterator_pqueries Sample::end() const
{
    return pqueries_.end();
}

/**
 * @brief Return a Range iterator to the @link Pquery Pqueries @endlink.
 *
 * This makes iterating Pqueries via a range based for loop easy.
 */
utils::Range<Sample::iterator_pqueries> Sample::pqueries()
{
    return { pqueries_ };
}

/**
 * @brief Return a const Range iterator to the @link Pquery Pqueries @endlink.
 *
 * This makes iterating Pqueries via a range based for loop easy.
 */
utils::Range<Sample::const_iterator_pqueries> Sample::pqueries() const
{
    return { pqueries_ };
}

} // namespace placement
} // namespace genesis
