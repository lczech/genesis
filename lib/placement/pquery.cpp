/**
 * @brief Implementation of Pquery class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/pquery.hpp"

namespace genesis {

// =============================================================================
//     Modifiers
// =============================================================================

/**
 * @brief Creates a new PqueryPlacement, adds it to the Pquery and returns a pointer to it.
 *
 * The values of the placement can than be adjusted using the returned pointer.
 */
PqueryPlacement* Pquery::add_placement(PlacementTree::EdgeType* edge)
{
    auto place = make_unique<PqueryPlacement>();
    PqueryPlacement* place_ptr = place.get();

    // Add connection to the edge and vice versa.
    place->edge_num = edge->data.edge_num;
    place->edge = edge;
    place->edge->data.placements.push_back(place_ptr);

    // Add the placement to the query and vice versa.
    place->pquery = this;
    placements.push_back(std::move(place));

    return place_ptr;
}

/**
 * @brief Creates a new PqueryName, adds it to the Pquery and returns a pointer to it.
 */
PqueryName* Pquery::add_name(std::string name)
{
    auto pname = make_unique<PqueryName>(name);
    PqueryName* name_ptr = pname.get();

    // Add the name to the query and vice versa.
    pname->pquery = this;
    names.push_back(std::move(pname));

    return name_ptr;
}

} // namespace genesis
