/**
 * @brief Implementation of Pquery class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/pquery.hpp"

#include "placement/placement_tree.hpp"
#include "utils/core/std.hpp"

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
 * and pointer to PqueryPlacement%s are invalidated.
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
 * and pointer to PqueryPlacement%s are invalidated.
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
PqueryPlacement const& Pquery::placement_at( size_t index ) const
{
    return placements_[index];
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

// =============================================================================
//     Name Accessors and Modifiers
// =============================================================================

/**
 * @brief Create a new PqueryName using the provided parameters, add it to the Pquery and return it.
 *
 * As this function might reallocate the memory where names are stored, all iterators
 * and pointer to PqueryNames%s are invalidated.
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
 * and pointer to PqueryNames%s are invalidated.
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
PqueryName const& Pquery::name_at( size_t index ) const
{
    return names_.at(index);
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
