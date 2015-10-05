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

#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =================================================================================================
//     Modifiers
// =================================================================================================

void merge_duplicates (PlacementMap& map)
{
    std::unordered_map<std::string, Pquery*> hash;
    std::unordered_set<Pquery*>              del;

    for (auto& it : map.pqueries()) {
        for (size_t name_idx = 0; name_idx < it->name_size() ; ++name_idx) {
            auto name = it->get_name(name_idx).name;

            if (hash.count(name) == 0) {
                hash[name] = &*it;
            } else {
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

    erase_if(map.pqueries(), [del] (std::unique_ptr<Pquery>& pqry) {
        return del.count(pqry.get()) > 0;
    });
}

} // namespace genesis
