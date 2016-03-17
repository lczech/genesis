/**
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/function/functions.hpp"

#include "placement/function/helper.hpp"
#include "tree/default/distances.hpp"
#include "tree/distances.hpp"
#include "utils/core/std.hpp"

#include <algorithm>
#include <assert.h>
#include <exception>
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
bool has_name( Sample const& smp, std::string const& name )
{
    for( auto const& pqry : smp.pqueries() ) {
        if( has_name( *pqry, name ) ) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Return the first Pquery that has a particular name, or nullptr of none has.
 */
Pquery const* find_pquery( Sample const& smp, std::string const& name )
{
    // TODO instead of pointer, return an iterator!
    // then use find if directly!
    for( auto const& pqry : smp.pqueries() ) {
        if( has_name(*pqry, name) ) {
            return pqry.get();
        }
    }
    return nullptr;
}

// =================================================================================================
//     Normalization and Sorting
// =================================================================================================

/**
 * @brief Recalculate the `like_weight_ratio` of the PqueryPlacement&s of each Pquery in the Sample,
 * so that their sum is 1.0, while maintaining their ratio to each other.
 */
void normalize_weight_ratios( Sample& smp )
{
    for( auto& pqry : smp.pqueries() ) {
        double sum = 0.0;
        for (auto& place : pqry->placements) {
            sum += place->like_weight_ratio;
        }
        for (auto& place : pqry->placements) {
            place->like_weight_ratio /= sum;
        }
    }
}

/**
 * @brief Remove all PqueryPlacement%s but the most likely one from all
 * @link Pquery Pqueries @endlink in the Sample.
 *
 * Pqueries can contain multiple placements on different branches. For example, the EPA algorithm
 * of RAxML outputs up to the 7 most likely positions for placements to the output Jplace file by
 * default. The property `like_weight_ratio` weights those placement positions so that the sum over
 * all positions per pquery is 1.0.
 *
 * This function removes all but the most likely placement (the one which has the maximal
 * `like_weight_ratio`) from each Pquery. It additionally sets the `like_weight_ratio` of the
 * remaining placement to 1.0, as this one now is the only one left, thus it's "sum" has to be 1.0.
 */
void restrain_to_max_weight_placements( Sample& smp )
{
    for( auto& pqry : smp.pqueries() ) {
        // Initialization.
        double             max_w = -1.0;
        PqueryPlacement*   max_p = nullptr;
        // PlacementTreeEdge* max_e = nullptr;

        for (auto& place : pqry->placements) {
            // Find the maximum of the weight ratios in the placements of this pquery.
            if (place->like_weight_ratio > max_w) {
                max_w = place->like_weight_ratio;
                max_p = place.get();
                // max_e = place->edge;
            }

            // place->edge = nullptr;

            // Delete the reference from the edge to the current placement. We will later add the
            // one that points to the remaining (max weight) placement back to its edge.
            // std::vector<PqueryPlacement*>::iterator it = place->edge->data.placements.begin();
            // for (; it != place->edge->data.placements.end(); ++it) {
            //     if (*it == place.get()) {
            //         break;
            //     }
            // }

            // Assert that the edge actually contains a reference to this pquery. If not,
            // this means that we messed up somewhere else while adding/removing placements...
            // assert(it != place->edge->data.placements.end());
            // place->edge->data.placements.erase(it);
        }

        // Remove all but the max element from placements vector.
        auto neq = [max_p] (const std::unique_ptr<PqueryPlacement>& other)
        {
            return other.get() != max_p;
        };
        auto pend = std::remove_if (pqry->placements.begin(), pqry->placements.end(), neq);
        pqry->placements.erase(pend, pqry->placements.end());

        // Now add back the reference from the edge to the pquery.
        // Assert that we now have a single placement in the pquery (the most likely one).
        assert(pqry->placements.size() == 1 && pqry->placements[0].get() == max_p);
        // max_p->edge = max_e;
        // max_p->edge->data.placements.push_back(max_p);

        // Also, set the like_weight_ratio to 1.0, because we do not have any other placements left.
        max_p->like_weight_ratio = 1.0;
    }
}

// void sort_placements_by_proximal_length( PlacementTreeEdge& edge );
// void sort_placements_by_proximal_length( Sample& smp );

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
void sort_placements_by_like_weight_ratio( Sample& smp )
{
    for( auto& pquery_it : smp.pqueries() ) {
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
void merge_duplicates (Sample& smp)
{
    collect_duplicate_pqueries (smp);
    merge_duplicate_names      (smp);
    merge_duplicate_placements (smp);
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
void collect_duplicate_pqueries (Sample& smp)
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

        // A hash smp that contains the already processed names and links them to their pquery.
        // Whenever the same name is found again, the associated placements will be copied to the
        // stored Pquery and the then surplus Pquery will be deleted.
        std::unordered_map<std::string, Pquery*> hash;

        // This is a list of the Pqueries that we want to delete, because their placements have been
        // moved to other Pqueries (those, which contain the same name). We need this list as
        // deleting from a list while iterating it is not a good idea.
        std::unordered_set<Pquery*> del;

        for (auto& it : smp.pqueries()) {
            // Collect the Pqueries that can be merged with the current one, because they share
            // a common name.
            std::unordered_set<Pquery*> merges;
            for( auto name_it = it->begin_names(); name_it != it->end_names(); ++name_it ) {
                auto& name = *name_it;

                if (hash.count(name.name) > 0) {
                    merges.insert(hash[name.name]);
                }
            }

            if (merges.size() == 0) {
                // All names are new, so store them in the hash smp for later.
                // We don't need to do any merging in this case.
                for( auto name_it = it->begin_names(); name_it != it->end_names(); ++name_it ) {
                    auto& name = *name_it;

                    // We are here because we found no pquery to merge with. This means that the
                    // name cannot be in the hash smp already.
                    assert(hash.count(name.name) == 0);

                    // Now add it.
                    hash[name.name] = &*it;
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
                for( auto name_it = it->begin_names(); name_it != it->end_names(); ++name_it ) {
                    auto& name = *name_it;

                    // Add the name to the Pquery.
                    merge_into->add_name( name );

                    // Also add it to the hash smp. If this was name that is responsible for the
                    // merging (the shared name), we will replace the entry by an indentical copy.
                    // For all other names of the current Pquery, we need this to ensure that the
                    // next Pqueries in the loop will find it.
                    hash[name.name] = merge_into;
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
        erase_if(smp.pqueries(), [del] (std::unique_ptr<Pquery>& pqry) {
            return del.count(pqry.get()) > 0;
        });
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
        merge_units[ place->edge_num() ].push_back( place.get() );
    }

    // For each edge_num (i.e., each index in the unordered_map), merge all placements into the first
    // one and mark all other for deletion.
    for (auto place_it : merge_units) {
        auto& place_vec = place_it.second;
        size_t count = place_vec.size();

        // Add up everything into the first placement.
        // If there is only one placement on this edge, nothing happens.
        for (size_t i = 1; i < count; ++i) {
            assert( place_vec[0]->edge_num() == place_vec[i]->edge_num() );

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
void merge_duplicate_placements (Sample& smp)
{
    for (auto& pquery_it : smp.pqueries()) {
        merge_duplicate_placements (*pquery_it);
    }
}

/**
 * @brief Merge all PqueryName%s that have the same `name` property into one, while adding up their
 * `multiplicity`.
 */
void merge_duplicate_names( Pquery& pquery )
{
    // We add names to this map, indexed by their actual name string.
    std::unordered_map<std::string, PqueryName> result;
    for( auto name_it = pquery.begin_names(); name_it != pquery.end_names(); ++name_it ) {
        auto const& name = *name_it;

        if( result.count( name.name ) == 0 ) {
            result[ name.name ] = name;
        } else {
            result[ name.name ].multiplicity += name.multiplicity;
        }
    }

    // Now delete all names and re-populate using the map.
    pquery.clear_names();
    for( auto const& n : result ) {
        pquery.add_name( n.second );
    }
}

/**
 * @brief Call `merge_duplicate_names()` for each Pquery of the Sample.
 */
void merge_duplicate_names (Sample& smp)
{
    for (auto& pquery_it : smp.pqueries()) {
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
std::pair<PlacementTreeEdge const*, size_t> placement_count_max_edge( Sample const& smp )
{
    PlacementTreeEdge const* edge = nullptr;
    size_t                   max  = 0;

    auto place_map = placements_per_edge( smp );

    for( auto const& it : place_map ) {
        if( it.second.size() > max ) {
            edge = smp.tree().edge_at( it.first );
            max  = it.second.size();
        }
    }

    return std::make_pair(edge, max);
}

/**
 * @brief Get the summed mass of the placements on the heaviest edge, measured by their
 * `like_weight_ratio`, and a pointer to this edge.
 */
std::pair<PlacementTreeEdge const*, double> placement_mass_max_edge( Sample const& smp )
{
    PlacementTreeEdge const* edge = nullptr;
    double                   max  = 0.0;

    auto place_map = placements_per_edge( smp );

    for( auto const& it : place_map ) {
        double sum = 0.0;
        for( auto const& place : it.second ) {
            sum += place->like_weight_ratio;
        }
        if (sum > max) {
            edge = smp.tree().edge_at( it.first );
            max  = sum;
        }
    }

    return std::make_pair(edge, max);
}

// =================================================================================================
//     Histograms
// =================================================================================================

// TODO use the histogram class of utils here!

/**
 * @brief Return a histogram representing how many placements have which depth with respect to
 * their closest leaf node.
 *
 * The depth between two nodes on a tree is the number of edges between them. Thus, the depth of a
 * placement (which sits on an edge of the tree) to a specific node is the number of edges between
 * this node and the closer one of the two nodes at the end of the edge where the placement sits.
 *
 * The closest leaf to a placement is thus the leaf node which has the smallest depth to that
 * placement. This function then returns a histogram of how many placements (values of the vector)
 * are there that have a specific depth (indices of the vector) to their closest leaf.
 *
 * Example: A return vector of
 *
 *     histogram[0] = 2334
 *     histogram[1] = 349
 *     histogram[2] = 65
 *     histogram[3] = 17
 *
 * means that there are 2334 placements that sit on an edge which leads to a leaf node (thus, the
 * depth of one of the nodes of the edge is 0). It has 349 placements that sit on an edge where
 * one of its nodes has one neighbour that is a leaf; and so on.
 *
 * The vector is automatically resized to the needed number of elements.
 */
std::vector<int> closest_leaf_depth_histogram( Sample const& smp )
{
    std::vector<int> hist;

    // Get a vector telling us the depth from each node to its closest leaf node.
    auto depths = closest_leaf_depth_vector( smp.tree() );

    for( auto const& pqry : smp.pqueries() ) {
        for (const auto& place : pqry->placements) {
            // Try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            int dp = depths[place->edge().primary_node()->index()].second;
            int ds = depths[place->edge().secondary_node()->index()].second;
            unsigned int ld = std::min(dp, ds);

            // Put the closer one into the histogram, resize if necessary.
            if (ld + 1 > hist.size()) {
                hist.resize(ld + 1, 0);
            }
            ++hist[ld];
        }
    }

    return hist;
}

/**
 * @brief Returns a histogram counting the number of placements that have a certain distance to
 * their closest leaf node, divided into equally large intervals between a min and a max distance.
 *
 * The distance range between min and max is divided into `bins` many intervals of equal size.
 * Then, the distance from each placement to its closest leaf node is calculated and the counter for
 * this particular distance inverval in the histogram is incremented.
 *
 * The distance is measured along the `branch_length` values of the edges, taking the
 * `pendant_length` and `proximal_length` of the placements into account. If the distances is
 * outside of the interval [min,max], the counter of the first/last bin is incremented respectively.
 *
 * Example:
 *
 *     double min      =  0.0;
 *     double max      = 20.0;
 *     int    bins     = 25;
 *     double bin_size = (max - min) / bins;
 *     std::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);
 *     for (unsigned int bin = 0; bin < hist.size(); ++bin) {
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; " << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
 *     }
 * %
 */
std::vector<int> closest_leaf_distance_histogram (
    Sample const& smp, const double min, const double max, const int bins
) {
    std::vector<int> hist(bins, 0);
    double bin_size = (max - min) / bins;

    // get a vector telling us the distance from each node to its closest leaf node.
    auto dists = tree::closest_leaf_distance_vector( smp.tree() );

    for (const auto& pqry : smp.pqueries()) {
        for (const auto& place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place->pendant_length
                      + place->proximal_length
                      + dists[place->edge().primary_node()->index()].second;
            double ds = place->pendant_length
                      + place->edge().data.branch_length
                      - place->proximal_length
                      + dists[place->edge().secondary_node()->index()].second;
            double ld = std::min(dp, ds);

            // find the right bin. if the distance value is outside the boundaries of [min;max],
            // place it in the first or last bin, respectively.
            int bin = static_cast <int> (std::floor( (ld - min) / bin_size ));
            if (bin < 0) {
                bin = 0;
            }
            if (bin >= bins) {
                bin = bins - 1;
            }
            ++hist[bin];
        }
    }

    return hist;
}

/**
 * @brief Returns the same type of histogram as closest_leaf_distance_histogram(), but automatically
 * determines the needed boundaries.
 *
 * See closest_leaf_distance_histogram() for general information about what this function does. The
 * difference between both functions is that this one first procresses all distances from
 * placements to their closest leaf nodes to find out what the shortest and longest are, then sets
 * the boundaries of the histogram accordingly. The number of bins is then used to divide this
 * range into intervals of equal size.
 *
 * The boundaries are returned by passing two doubles `min` and `max` to the function by reference.
 * The value of `max` will actually contain the result of std::nextafter() called on the longest
 * distance; this makes sure that the value itself will be placed in the interval.
 *
 * Example:
 *
 *     double min, max;
 *     int    bins = 25;
 *     std::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);
 *     double bin_size = (max - min) / bins;
 *     LOG_INFO << "Histogram boundaries: [" << min << "," << max << ").";
 *     for (unsigned int bin = 0; bin < hist.size(); ++bin) {
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; " << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
 *     }
 *
 * It has a slightly higher time and memory consumption than the non-automatic version
 * closest_leaf_distance_histogram(), as it needs to process the values twice in order to find their
 * min and max.
 */
std::vector<int> closest_leaf_distance_histogram_auto (
    Sample const& smp, double& min, double& max, const int bins
) {
    std::vector<int> hist(bins, 0);

    // we do not know yet where the boundaries of the histogram lie, so we need to store all values
    // first and find their min and max.
    std::vector<double> distrib;
    double min_d = 0.0;
    double max_d = 0.0;

    // get a vector telling us the distance from each node to its closest leaf node.
    auto dists = tree::closest_leaf_distance_vector( smp.tree() );

    // calculate all distances from placements to their closest leaf and store them.
    for (const auto& pqry : smp.pqueries()) {
        for (const auto& place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place->pendant_length
                      + place->proximal_length
                      + dists[place->edge().primary_node()->index()].second;
            double ds = place->pendant_length
                      + place->edge().data.branch_length
                      - place->proximal_length
                      + dists[place->edge().secondary_node()->index()].second;
            double ld = std::min(dp, ds);
            distrib.push_back(ld);

            // update min and max as needed (and in first iteration). we use std::nextafter() for
            // the max in order to make sure that the max value is actually placed in the last bin.
            if (distrib.size() == 1 || ld < min_d) {
                min_d = ld;
            }
            if (distrib.size() == 1 || ld > max_d) {
                max_d = std::nextafter(ld, ld + 1);
            }
        }
    }

    // now we know min and max of the distances, so we can calculate the histogram.
    double bin_size = (max_d - min_d) / bins;
    for (double ld : distrib) {
        int bin = static_cast <int> (std::floor( (ld - min_d) / bin_size ));
        assert(bin >= 0 && bin < bins);
        ++hist[bin];
    }

    // report the min and max values to the calling function and return the histogram.
    min = min_d;
    max = max_d;
    return hist;
}

} // namespace placement
} // namespace genesis
