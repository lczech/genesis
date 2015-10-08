/**
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include <algorithm>
#include <assert.h>
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
    // Iterate as long as needed to merge all transitive connections between Pqueries that share
    // names. We need as many iterations as the longest chain of connected Pqueries.
    bool need_iteration = true;
    while (need_iteration) {
        need_iteration = false;

        // A hash map that contains the already processed names and links them to their pquery.
        // Whenever the same name is found again, the associated placements will be copied to the
        // stored Pquery and the then surplus Pquery will be deleted.
        std::unordered_map<std::string, Pquery*> hash;

        // This is a list of the Pqueries that we want to delete, because their placements have been
        // moved to other Pqueries (those, which contain the same name). We need this list as deleting
        // from a list while iterating it is not a good idea.
        std::unordered_set<Pquery*> del;

        for (auto& it : map.pqueries()) {
            // Collect the Pqueries that can be merged with the current one, because they share
            // a common name.
            std::unordered_set<Pquery*> merges;
            for (size_t name_idx = 0; name_idx < it->name_size() ; ++name_idx) {
                auto name = it->get_name(name_idx).name;
                if (hash.count(name) > 0) {
                    merges.insert(hash[name]);
                }
            }

            if (merges.size() == 0) {
                // All names are new, so store them in the hash map for later.
                // We don't need to do any merging in this case.
                for (size_t name_idx = 0; name_idx < it->name_size() ; ++name_idx) {
                    auto name = it->get_name(name_idx).name;
                    assert(hash.count(name) == 0);
                    hash[name] = &*it;
                }
            } else {
                // We need to merge with exaclty on Pquery. No transitive connections.
                // Do the merging and mark the Pquery for deletion.

                // Get the Pquery that we will merge into.
                Pquery* merge_into = *merges.begin();

                for (auto& place : it->placements) {
                    merge_into->insert_placement(*place);
                }

                for (auto& name : it->names) {
                    auto name_it = std::find_if(
                        merge_into->names.begin(),
                        merge_into->names.end(),
                        [&name] (std::unique_ptr<PqueryName>& merge_name) {
                            return merge_name->name == name->name;
                        }
                    );

                    if (name_it == merge_into->names.end()) {
                        merge_into->add_name(name->name)->multiplicity = name->multiplicity;
                    } else {
                        (*name_it)->multiplicity += name->multiplicity;
                    }
                }

                del.insert(&*it);

                if (merges.size() > 1) {
                    need_iteration = true;
                }
            }
        }

        // Delete all Pqueries that were merged to others during this iteration.
        erase_if(map.pqueries(), [del] (std::unique_ptr<Pquery>& pqry) {
            return del.count(pqry.get()) > 0;
        });
    }
}

} // namespace genesis
