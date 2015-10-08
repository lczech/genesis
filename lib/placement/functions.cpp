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
            for (auto& name : it->names) {
                if (hash.count(name->name) > 0) {
                    merges.insert(hash[name->name]);
                }
            }

            if (merges.size() == 0) {
                // All names are new, so store them in the hash map for later.
                // We don't need to do any merging in this case.
                for (auto& name : it->names) {
                    // We are here because we found no pquery to merge with. This means that the
                    // name cannot be in the hash map already.
                    assert(hash.count(name->name) == 0);

                    // Now add it.
                    hash[name->name] = &*it;
                }
            } else {
                // We need merging. We will merge with only one Pquery in this iteration. If there
                // are more than one Pqueries that we need to merge with (i.e., merges.size() > 1),
                // we will mark that we need another iteration and do the other mergings then.

                // Get the Pquery that we will merge into.
                Pquery* merge_into = *merges.begin();

                // Add all placements to it.
                for (auto& place : it->placements) {
                    merge_into->insert_placement(*place);
                }

                // Add all names. This will cause doubled names, but they are filtered out later.
                // We could do the check here, but this would increase complexity and gain just a
                // bit of speed (probably).
                for (auto& name : it->names) {
                    // Add the name to the Pquery.
                    merge_into->insert_name(*name);

                    // Also add it to the hash map. If this was name that is responsible for the
                    // merging (the shared name), we will replace the entry by an indentical copy.
                    // For all other names of the current Pquery, we need this to ensure that the
                    // next Pqueries will find it.
                    hash[name->name] = merge_into;
                }

                // Mark the Pquery for deletion.
                del.insert(&*it);

                // We need to merge with more than one Pquery, meaning we have transitive
                // connections. This means, we need another iteration to resolve this.
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

    merge_duplicate_names(map);
    merge_duplicate_placements(map);
}

void merge_duplicate_placements (Pquery& pquery)
{
    (void) pquery;
}

void merge_duplicate_placements (PlacementMap& map)
{
    for (auto& pquery_it : map.pqueries()) {
        merge_duplicate_placements (*pquery_it);
    }
}

void merge_duplicate_names (Pquery& pquery)
{
    std::unordered_map<std::string, PqueryName*> hash;
    std::unordered_set<PqueryName*> del;

    for (auto& name_it : pquery.names) {
        if (hash.count(name_it->name) == 0) {
            hash[name_it->name] = &*name_it;
        } else {
            hash[name_it->name]->multiplicity += name_it->multiplicity;
            del.insert(&*name_it);
        }
    }

    erase_if(pquery.names, [del] (std::unique_ptr<PqueryName>& name) {
        return del.count(name.get()) > 0;
    });
}

void merge_duplicate_names (PlacementMap& map)
{
    for (auto& pquery_it : map.pqueries()) {
        merge_duplicate_names (*pquery_it);
    }
}

} // namespace genesis
