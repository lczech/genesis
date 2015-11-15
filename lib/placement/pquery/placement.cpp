/**
 * @brief Implementation of PqueryPlacement class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/pquery/placement.hpp"

#include "placement/placement_tree.hpp"

namespace genesis {

// =================================================================================================
//     Constructor and Rule of Five
// =================================================================================================

/**
 * @brief Destructor. Removes the Placement also from its Edge.
 */
PqueryPlacement::~PqueryPlacement()
{
    if (!edge) {
        return;
    }

    // Find the pointer on the edge that belongs to this placement.
    auto it = edge->data.placements.begin();
    for (; it != edge->data.placements.end(); ++it) {
        if (*it == this) {
            break;
        }
    }

    // Assert that the edge actually contains a reference to this pquery. If not,
    // this means that we messed up somewhere else while adding/removing placements...
    assert(it != edge->data.placements.end());

    // Delete the reference from the edge to the current placement.
    edge->data.placements.erase(it);
}

} // namespace genesis
