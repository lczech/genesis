/**
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "placement/functions.hpp"

#include "utils/utils.hpp"

namespace genesis {

// =================================================================================================
//     Modifiers
// =================================================================================================

void merge_duplicates (PlacementMap& map)
{
    // A hash map that contains the already processed names and links them to their pquery.
    // Whenever the same name is found again, the associated placements will be copied to the stored
    // Pquery and the then surplus Pquery will be deleted.
    std::unordered_map<std::string, Pquery*> hash;

    // This is a list of the Pqueries that we want to delete, because their placements have been
    // moved to other Pqueries (those, which contain the same name). We need this list as deleting
    // from a list while iterating it is not a good idea.
    std::unordered_set<Pquery*>              del;

    // Check for all pqueries...
    for (auto& it : map.pqueries()) {
        // ... if one of their names...
        for (size_t name_idx = 0; name_idx < it->name_size() ; ++name_idx) {
            auto name = it->get_name(name_idx).name;

            // ... is also the name of another pquery.
            if (hash.count(name) == 0) {
                // If not (first time that we see a name), store it in the hash map for later.
                hash[name] = &*it;
            } else {
                // If so, move the placements and mark the pquery for deletion.

                // Skip if the element in the hash map is this pquery. This can only happen (as far
                // as I am aware) if a pquery contains the same name more than once. Still, safty
                // first!
                if (hash[name] == &*it) {
                    continue;
                }

                for (auto& place : it->placements) {
                    hash[name]->insert_placement(*place);
                }

                del.insert(&*it);
            }
        }
    }

    // Delete all Pqueries that were merged to others.
    erase_if(map.pqueries(), [del] (std::unique_ptr<Pquery>& pqry) {
        return del.count(pqry.get()) > 0;
    });
}

} // namespace genesis
