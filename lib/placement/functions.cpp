/**
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/functions.hpp"

#include "utils/core/std.hpp"

#include <algorithm>
#include <assert.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace genesis {
namespace placement {

// =================================================================================================
//     Pquery Names
// =================================================================================================

/**
 * @brief Return true iff the given Pquery contains a particular name.
 */
bool has_name( Pquery const& pquery, std::string const& name )
{
    for( size_t i = 0; i < pquery.name_size(); ++i ) {
        if( pquery.name_at(i).name == name ) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Return true iff the given Sample contains a Pquery with a particular name, i.e.,
 * a PqueryName whose name member equals the given name.
 */
bool has_name( Sample const& map, std::string const& name )
{
    for( auto const& pqry : map.pqueries() ) {
        if( has_name( *pqry, name ) ) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Return the first Pquery that has a particular name, or nullptr of none has.
 */
Pquery const* find_pquery( Sample const& map, std::string const& name )
{
    // TODO instead of pointer, return an iterator!
    // then use find if directly!
    for( auto const& pqry : map.pqueries() ) {
        if( has_name(*pqry, name) ) {
            return pqry.get();
        }
    }
    return nullptr;
}

// =================================================================================================
//     Normalization and Sorting
// =================================================================================================

// void normalize_weight_ratios( Sample& map );

// void restrain_to_max_weight_placements( Sample& map );

// void sort_placements_by_proximal_length( PlacementTreeEdge& edge );
// void sort_placements_by_proximal_length( Sample& map );

/**
 * @brief Sort the PqueryPlacement%s of a Pquery by their `like_weight_ratio`, in descending order
 * (most likely first).
 */
void sort_placements_by_like_weight_ratio( Pquery& pquery )
{
    std::sort(
        pquery.begin_placements(),
        pquery.end_placements(),
        [] (
            std::unique_ptr<PqueryPlacement> const& lhs,
            std::unique_ptr<PqueryPlacement> const& rhs
        ) {
            return lhs->like_weight_ratio > rhs->like_weight_ratio;
        }
    );
}

/**
 * @brief Sort the PqueryPlacement%s of all @link Pquery Pqueries @endlink by their
 * `like_weight_ratio`, in descending order (most likely first).
 */
void sort_placements_by_like_weight_ratio( Sample& map )
{
    for( auto& pquery_it : map.pqueries() ) {
        sort_placements_by_like_weight_ratio( *pquery_it );
    }
}

// =================================================================================================
//     Merging Duplicates
// =================================================================================================

/**
 * @brief Look for @link Pquery Pqueries @endlink with the same name and merge them.
 *
 * This function is a wrapper that simply calls three other functions on the provided Sample:
 *
 *     * collect_duplicate_pqueries()
 *     * merge_duplicate_names()
 *     * merge_duplicate_placements()
 *
 * See there for more information on what they do.
 */
void merge_duplicates (Sample& map)
{
    collect_duplicate_pqueries (map);
    merge_duplicate_names      (map);
    merge_duplicate_placements (map);
}

/**
 * @brief Find all @link Pquery Pqueries @endlink that share a common name and combine them into a
 * single Pquery containing all their collective PqueryPlacement%s and PqueryName%s.
 *
 * The function collects all Pqueries that share at least one name. This is transitive, so that for
 * example three Pqueries with two names each like `(a,b) (b,c) (c,d)` will be combined into one
 * Pquery. Thus, the transitive closure of shared names is collected.
 *
 * All those Pqueries with shared names are combined by simply moving all their Placements and
 * Names into one Pquery and deleting the others. This means that at least the shared names will
 * be doubled after this function. Also, Placements on the same edge can occur.
 * Thus, usually `merge_duplicate_names()` and `merge_duplicate_placements()` are called after
 * this function. The function merge_duplicates() does exaclty this, for convenience.
 */
void collect_duplicate_pqueries (Sample& map)
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
        // We detach (and later reattach) the pqueries from the tree for speed reasons.
        // See those functions for more information on why this is necessary.
        map.detach_pqueries_from_tree();
        erase_if(map.pqueries(), [del] (std::unique_ptr<Pquery>& pqry) {
            return del.count(pqry.get()) > 0;
        });
        map.reattach_pqueries_to_tree();
    }
}

/**
 * @brief Merge all PqueryPlacement%s of a Pquery that are on the same TreeEdge into one averaged
 * PqueryPlacement.
 *
 * The merging is done via averaging all values of the PqueryPlacement: `likelihood`,
 * `like_weight_ratio`, `proximal_length`, `pendant_length` and `parsimony`.
 */
void merge_duplicate_placements (Pquery& pquery)
{
    // We want to merge all placements that are on the same edge. This object collects those sets,
    // indexed by their edge_num.
    std::unordered_map <
        int,
        std::vector<PqueryPlacement*>
    > merge_units;

    // After merging, we can delete all but one placement per edge. This set collects the ones
    // that we later want to delete.
    std::unordered_set<PqueryPlacement*> del;

    // Collect all placements sorted (indexed) by their edge.
    for (auto& place : pquery.placements) {
        merge_units[place->edge_num].push_back(place.get());
    }

    // For each edge_num (i.e., each index in the unordered_map), merge all placements into the first
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

/**
 * @brief Call `merge_duplicate_placements()` for each Pquery of the Sample.
 */
void merge_duplicate_placements (Sample& map)
{
    map.detach_pqueries_from_tree();
    for (auto& pquery_it : map.pqueries()) {
        merge_duplicate_placements (*pquery_it);
    }
    map.reattach_pqueries_to_tree();
}

/**
 * @brief Merge all PqueryName%s that are the same into one, while adding up their `multiplicity`.
 */
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

/**
 * @brief Call `merge_duplicate_names()` for each Pquery of the Sample.
 */
void merge_duplicate_names (Sample& map)
{
    for (auto& pquery_it : map.pqueries()) {
        merge_duplicate_names (*pquery_it);
    }
}

// =================================================================================================
//     Placement Mass
// =================================================================================================

/**
 * @brief Get the number of placements on the edge with the most placements, and a pointer to this
 * edge.
 */
std::pair<PlacementTreeEdge*, size_t> placement_count_max_edge(PlacementTree const& tree)
{
    PlacementTreeEdge* edge = nullptr;
    size_t             max  = 0;

    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it ) {
        if ((*it)->data.placements.size() > max) {
            edge = (*it).get();
            max  = (*it)->data.placements.size();
        }
    }

    return std::make_pair(edge, max);
}

/**
 * @brief Get the summed mass of the placements on the heaviest edge, measured by their
 * `like_weight_ratio`, and a pointer to this edge.
 */
std::pair<PlacementTreeEdge*, double> placement_mass_max_edge(PlacementTree const& tree)
{
    PlacementTreeEdge* edge = nullptr;
    double             max  = 0.0;

    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it ) {
        double sum = 0.0;
        for (const auto& place : (*it)->data.placements) {
            sum += place->like_weight_ratio;
        }
        if (sum > max) {
            edge = (*it).get();
            max  = sum;
        }
    }

    return std::make_pair(edge, max);
}

} // namespace placement
} // namespace genesis
