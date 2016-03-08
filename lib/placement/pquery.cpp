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
        edge = val.edge;
    }

    // Add connection to the edge and vice versa.
    place->edge_num = edge->data.edge_num;
    place->edge = edge;
    place->edge->data.placements.push_back(place_ptr);

    // if( edge ) {
    //     place->edge_num = edge->data.edge_num;
    //     place->edge = edge;
    //     place->edge->data.placements.push_back(place_ptr);
    // } else {
    //     place->edge_num = val.edge_num;
    //     place->edge = nullptr;
    // }

    // Add the placement to the query and vice versa.
    place->pquery = this;
    placements.push_back(std::move(place));

    return place_ptr;
}

/**
 * @brief Creates a new PqueryName, adds it to the Pquery and returns a pointer to it.
 */
PqueryName* Pquery::emplace_name(std::string name, double multiplicity)
{
    return insert_name(PqueryName(name, multiplicity));
}

/**
 * @brief
 */
PqueryName* Pquery::insert_name(const PqueryName& other)
{
    // Create new name via copy constructor, get pointer to it.
    auto pname = make_unique<PqueryName>(other);
    PqueryName* name_ptr = pname.get();

    // Add the name to the query and vice versa.
    pname->pquery = this;
    names.push_back(std::move(pname));

    return name_ptr;
}

} // namespace placement
} // namespace genesis
