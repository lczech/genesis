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
//     Modifiers
// =============================================================================

/**
 * @brief Creates a new PqueryPlacement, adds it to the Pquery and returns a pointer to it.
 *
 * The values of the placement can than be adjusted using the returned pointer.
 */
PqueryPlacement* Pquery::emplace_placement(PlacementTreeEdge* edge)
{
    // This is not totally efficient, as we create an empty Placement and then copy-construct it
    // again, but for now this should be sufficient...
    return insert_placement(PqueryPlacement(), edge);
}

/**
 * @brief Creates a PqueryPlacement by copying from another one.
 *
 * If edge is given, the new Placement is attached to it. If not, the edge of the given Placement
 * is used instead. For this it is important that the given Placement belongs to the same Tree!
 */
PqueryPlacement* Pquery::insert_placement(const PqueryPlacement& val, PlacementTreeEdge* edge)
{
    auto place = make_unique<PqueryPlacement>(val);
    PqueryPlacement* place_ptr = place.get();

    if (!edge) {
        edge = val.edge_;
    }

    // Add connection to the edge.
    place->edge_ = edge;

    // if( edge ) {
    //     place->edge_num = edge->data.edge_num;
    //     place->edge = edge;
    //     place->edge->data.placements.push_back(place_ptr);
    // } else {
    //     place->edge_num = val.edge_num;
    //     place->edge = nullptr;
    // }

    // Add the placement to the query and vice versa.
    placements.push_back(std::move(place));

    return place_ptr;
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
//     Name Accessros and Modifiers
// =============================================================================

/**
 * @brief Create a new PqueryName using the provided parameters, add it to the Pquery and return it.
 */
PqueryName& Pquery::add_name( std::string name, double multiplicity )
{
    names_.emplace_back( name, multiplicity );
    return names_.back();
}

/**
 * @brief Create a new PqueryName as a copy of the provided one, add it to the Pquery and return it.
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
