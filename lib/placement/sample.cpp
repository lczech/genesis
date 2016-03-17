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
 * @brief Copy constructor.
 */
Sample::Sample( Sample const& other )
{
    // TODO once the edge and node data are copyable and tree supports full data copies, these are not needed any longer.

    clear();

    // use assignment operator to create copy of the tree and metadata.
    tree_    = other.tree_;
    metadata = other.metadata;

    // copy all data of the tree: do a preorder traversal on both trees in parallel
    auto it_n = tree_.begin_preorder();
    auto it_o = other.tree_.begin_preorder();
    for (
        ;
        it_n != tree_.end_preorder() && it_o != other.tree_.end_preorder();
        ++it_n, ++it_o
    ) {
        // the trees are copies of each other, they need to have the same rank. otherwise,
        // the copy constructor is not working!
        assert(it_n.node()->rank() == it_o.node()->rank());

        it_n.edge()->data.branch_length = it_o.edge()->data.branch_length;
        it_n.edge()->data.reset_edge_num( it_o.edge()->data.edge_num() );

        it_n.node()->data.name = it_o.node()->data.name;
    }

    // the trees are copies. they should take equal iterations to finish a traversal.
    assert( it_n == tree_.end_preorder() && it_o == other.tree_.end_preorder() );

    // copy all (o)ther pqueries to (n)ew pqueries
    // TODO this is very similar to merge(). make this code shared somehow.
    auto en_map = edge_num_to_edge_map( tree_ );
    for (const auto& opqry : other.pqueries_) {
        auto npqry = Pquery();

        for( auto opit = opqry.begin_placements(); opit != opqry.end_placements(); ++opit ) {
            auto const& op = *opit;
            npqry.add_placement( *en_map[ op.edge_num() ], op );
        }
        for( auto name_it = opqry.begin_names(); name_it != opqry.end_names(); ++name_it ) {
            npqry.add_name( *name_it );
        }

        pqueries_.push_back(npqry);
    }
}

/**
 * @brief Move constructor.
 */
// Sample::Sample( Sample&& other ) noexcept
// {
//     swap(other);
// }

Sample::Sample( Sample&& other )
    : pqueries_( std::move(other.pqueries_))
    , tree_(     std::move(other.tree_))
    , metadata(  std::move(other.metadata))
{
    other.pqueries_.clear();
    other.tree_.clear();
    other.metadata.clear();
}

/**
 * @brief Assignment operator. See Copy constructor for details.
 */
Sample& Sample::operator = (const Sample& other)
{
    // check for self-assignment.
    if (&other == this) {
        return *this;
    }

    // the Sample tmp is a copy of the right hand side object (automatically created using the
    // copy constructor). we can thus simply swap the arrays, and upon leaving the function,
    // tmp is automatically destroyed, so that its arrays are cleared and the data freed.
    Sample tmp(other);
    std::swap(pqueries_, tmp.pqueries_);
    tree_.swap(tmp.tree_);
    std::swap(metadata, tmp.metadata);
    return *this;
}

Sample& Sample::operator= (Sample&& other)
{
    swap(other);
    return *this;
}

void Sample::swap (Sample& other)
{
    using std::swap;
    swap(pqueries_, other.pqueries_);
    swap(tree_,     other.tree_);
    swap(metadata,  other.metadata);
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

} // namespace placement
} // namespace genesis
