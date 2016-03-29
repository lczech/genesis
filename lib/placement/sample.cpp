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

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdio.h>
#include <unordered_map>
#include <utility>

#include "placement/function/helper.hpp"
#include "tree/iterator/preorder.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Constructor and Rule of Five
// =================================================================================================

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
 * and pointer to the Pqueries of this Sample are invalidated.
 */
Pquery& Sample::add_pquery()
{
    pqueries_.push_back( Pquery() );
    return pqueries_.back();
}

/**
 * @brief Create a Pquery as a copy of the provided one, add it to the sample and return it.
 *
 * As this function might reallocate the memory where Pqueries are stored, all iterators
 * and pointer to the Pqueries of this Sample are invalidated.
 */
Pquery& Sample::add_pquery( Pquery const& other )
{
    pqueries_.push_back( other );
    return pqueries_.back();
}

/**
 * @brief Return a const ref to the Pquery container.
 *
 * This makes iterating Pqueries via a range based for loop easy.
 */
std::vector<Pquery> const& Sample::pqueries() const
{
    return pqueries_;
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

} // namespace placement
} // namespace genesis
