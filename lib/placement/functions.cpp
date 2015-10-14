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
//     Merging Duplicates
// =================================================================================================

void merge_duplicates (PlacementMap& map)
{
    collect_duplicate_pqueries (map);
    merge_duplicate_names      (map);
    merge_duplicate_placements (map);
}

void collect_duplicate_pqueries (PlacementMap& map)
{
    // We are looking for the transitive closure of all Pqueries that pairwise share a common name.
    // In a graph theory setting, this could be depicted as follows:
    // Each Pquery is a node, and it has an edge to other nodes iff they share a common name.
    // For this, there exist algorithms like Floyd–Warshall. However, they are an overkill for the
    // problem at hand, in terms of the complexity of both code and runtime requirements.
    // From the biological data and the way Evolutionary Placement works, we do not expect to have
    // many edges in this graph. Thus, this function uses a simpler solution: repeated search.
    // Worst case, this needs as many iterations over all Pqueries as the longest chain of shared
    // names. Pqueries with shared names like (a,b) (b,c) (c,d) (d,e) might need four iterations,
    // depending on the order of those Pqueries. This is acceptable, as this case should be rare.

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
        // moved to other Pqueries (those, which contain the same name). We need this list as
        // deleting from a list while iterating it is not a good idea.
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

                // TODO we could use move here instead of copy creating.

                // Add all placements to it.
                for (auto& place : it->placements) {
                    merge_into->insert_placement(*place);
                }

                // Add all names. This will cause doubled names, but they can be reduced later
                // via merge_duplicate_names().
                // We could do the check here, but this would increase complexity and gain just a
                // bit of speed (probably).
                for (auto& name : it->names) {
                    // Add the name to the Pquery.
                    merge_into->insert_name(*name);

                    // Also add it to the hash map. If this was name that is responsible for the
                    // merging (the shared name), we will replace the entry by an indentical copy.
                    // For all other names of the current Pquery, we need this to ensure that the
                    // next Pqueries in the loop will find it.
                    hash[name->name] = merge_into;
                }

                // Mark the Pquery for deletion.
                del.insert(&*it);

                // Check whether we need to merge with more than one Pquery, meaning that we have
                // transitive connections. This means, we need another iteration to resolve this.
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

void merge_duplicate_placements (Pquery& pquery)
{
    // We want to merge all placements that are on the same edge. This object collects those sets,
    // indexed by their edge.
    std::unordered_map <
        PlacementTree::EdgeType*,
        std::vector<PqueryPlacement*>
    > merge_units;

    // After merging, we can delete all but one placement per edge. This set collects the ones
    // that we later want to delete.
    std::unordered_set<PqueryPlacement*> del;

    // Collect all placements sorted (indexed) by their edge.
    for (auto& place : pquery.placements) {
        merge_units[place->edge].push_back(place.get());
    }

    // For each edge (i.e., each index in the unordered_map), merge all placements into the first
    // one and mark all other for deletion.
    for (auto place_it : merge_units) {
        auto& place_vec = place_it.second;
        size_t count = place_vec.size();

        // Add up everything into the first placement.
        // If there is only one placement on this edge, nothing happens.
        for (size_t i = 1; i < count; ++i) {
            assert(place_vec[0]->edge_num == place_vec[i]->edge_num);

            place_vec[0]->likelihood        += place_vec[i]->likelihood;
            place_vec[0]->like_weight_ratio += place_vec[i]->like_weight_ratio;
            place_vec[0]->proximal_length   += place_vec[i]->proximal_length;
            place_vec[0]->pendant_length    += place_vec[i]->pendant_length;
            place_vec[0]->parsimony         += place_vec[i]->parsimony;

            del.insert(place_vec[i]);
        }

        // Average everything.
        if (count > 1) {
            double denom = static_cast<double> (count);

            place_vec[0]->likelihood        /= denom;
            place_vec[0]->like_weight_ratio /= denom;
            place_vec[0]->proximal_length   /= denom;
            place_vec[0]->pendant_length    /= denom;
            place_vec[0]->parsimony         /= denom;
        }
    }

    // Finally, we can delete all surplus placements.
    erase_if(pquery.placements, [del] (std::unique_ptr<PqueryPlacement>& place) {
        return del.count(place.get()) > 0;
    });
}

void merge_duplicate_placements (PlacementMap& map)
{
    for (auto& pquery_it : map.pqueries()) {
        merge_duplicate_placements (*pquery_it);
    }
}

void merge_duplicate_names (Pquery& pquery)
{
    // We traverse all names of this Pquery and use a hash map to find duplictes. As deleting during
    // iteration is not a good idea, we also keep track of those names that are duplicates and
    // delete them later.
    std::unordered_map<std::string, PqueryName*> hash;
    std::unordered_set<PqueryName*> del;

    // Merge names. If two PlacementName objects have a commong name, add their multiplicity into
    // the first one and mark the other one for deletion.
    for (auto& name_it : pquery.names) {
        if (hash.count(name_it->name) == 0) {
            hash[name_it->name] = &*name_it;
        } else {
            hash[name_it->name]->multiplicity += name_it->multiplicity;
            del.insert(&*name_it);
        }
    }

    // Now delete all Name objects whose values were merged into objects of the same name.
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
