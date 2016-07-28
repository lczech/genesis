/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/function/functions.hpp"

#include "placement/function/helper.hpp"
#include "placement/function/operators.hpp"
#include "tree/default/distances.hpp"
#include "tree/function/distances.hpp"
#include "tree/function/functions.hpp"
#include "tree/function/operators.hpp"
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
        if( has_name( pqry, name ) ) {
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
        if( has_name( pqry, name )) {
            return &pqry;
        }
    }
    return nullptr;
}

/**
 * @brief Return the first Pquery that has a particular name, or nullptr of none has.
 */
Pquery* find_pquery( Sample& smp, std::string const& name )
{
    // TODO instead of pointer, return an iterator!
    // then use find if directly!
    for( auto& pqry : smp.pqueries() ) {
        if( has_name( pqry, name )) {
            return &pqry;
        }
    }
    return nullptr;
}

/**
 * @brief Return a set of all unique PqueryName%s of the @link Pquery Pqueries @endlink
 * of the given sample.
 *
 * If a Pquery contains multiple names, all of them are added to the set.
 */
std::unordered_set<std::string> all_pquery_names( Sample const& sample )
{
    std::unordered_set<std::string> result;
    for( auto const& pquery : sample ) {
        for( auto const& pname : pquery.names() ) {
            result.insert( pname.name );
        }
    }
    return result;
}

// =================================================================================================
//     Normalization and Sorting
// =================================================================================================

/**
 * @brief Recalculate the `like_weight_ratio` of the PqueryPlacement&s of a Pquery,
 * so that their sum is 1.0, while maintaining their ratio to each other.
 */
void normalize_weight_ratios( Pquery& pquery )
{
    double sum = 0.0;
    for( auto const& place : pquery.placements() ) {
        sum += place.like_weight_ratio;
    }
    if( sum == 0.0 ) {
        throw std::overflow_error( "Cannot normalize weight ratios if all of them are zero." );
    }
    for( auto& place : pquery.placements() ) {
        place.like_weight_ratio /= sum;
    }
}

/**
 * @brief Recalculate the `like_weight_ratio` of the PqueryPlacement&s of each Pquery in the Sample,
 * so that their sum is 1.0, while maintaining their ratio to each other.
 *
 * This function simply calls normalize_weight_ratios( Pquery& pquery ) for all Pqueries of
 * the Sample.
 */
void normalize_weight_ratios( Sample& smp )
{
    for( auto pqry_it = smp.begin(); pqry_it != smp.end(); ++pqry_it ) {
        normalize_weight_ratios( *pqry_it );
    }
}

// void sort_placements_by_proximal_length( PlacementTreeEdge& edge );
// void sort_placements_by_proximal_length( Sample& smp );

/**
 * @brief Sort the PqueryPlacement%s of a Pquery by their `like_weight_ratio`, in descending order
 * (most likely first).
 */
void sort_placements_by_weight( Pquery& pquery )
{
    std::sort(
        pquery.begin_placements(),
        pquery.end_placements(),
        [] (
            PqueryPlacement const& lhs,
            PqueryPlacement const& rhs
        ) {
            return lhs.like_weight_ratio > rhs.like_weight_ratio;
        }
    );
}

/**
 * @brief Sort the PqueryPlacement%s of all @link Pquery Pqueries @endlink by their
 * `like_weight_ratio`, in descending order (most likely first).
 */
void sort_placements_by_weight( Sample& smp )
{
    for( auto pqry_it = smp.begin(); pqry_it != smp.end(); ++pqry_it ) {
        sort_placements_by_weight( *pqry_it );
    }
}

// =================================================================================================
//     Filtering
// =================================================================================================

/**
 * @brief Remove the PqueryPlacement%s with the lowest `like_weight_ratio`, while keeping the
 * accumulated weight (sum of all remaining `like_weight_ratio`s) above a given threshold.
 *
 * This is a cleaning function to get rid of unlikely placement positions, withouth sacrificing
 * too much detail of the overall distribution of weights. The EPA support a similar option, which
 * only writes enough of the most likely placement positions to the output to fullfil a threshold.
 */
void filter_min_accumulated_weight( Pquery& pquery, double threshold )
{
    // Sort, so that the most likely placements are in the beginning.
    sort_placements_by_weight( pquery );

    // Find the position where enough weight is accumulated.
    size_t i          = 0 ;
    double weight_sum = 0.0;
    do {
        weight_sum += pquery.placement_at(i).like_weight_ratio;
        ++i;
    } while( weight_sum < threshold && i < pquery.placement_size() );

    // Remove the rest.
    while( pquery.placement_size() > i ) {
        pquery.remove_placement_at( pquery.placement_size() - 1 );
    }
}

/**
 * @brief Remove the PqueryPlacement%s with the lowest `like_weight_ratio`, while keeping the
 * accumulated weight (sum of all remaining `like_weight_ratio`s) above a given threshold.
 *
 * This function calls filter_min_accumulated_weight( Pquery& pquery, double threshold ) for all
 * Pqueries of the Sample. See this version of the function for more information.
 */
void filter_min_accumulated_weight( Sample& smp, double threshold )
{
    for( auto& pquery : smp ) {
        filter_min_accumulated_weight( pquery, threshold );
    }
}

/**
 * @brief Remove all PqueryPlacement%s but the `n` most likely ones from the Pquery.
 *
 * Pqueries can contain multiple placements on different branches. For example, the EPA algorithm
 * of RAxML outputs up to the 7 most likely positions for placements to the output Jplace file by
 * default. The property `like_weight_ratio` weights those placement positions so that the sum over
 * all positions (all branches of the tree) per pquery is 1.0.
 *
 * This function removes all but the `n` most likely placements (the ones which have the highest
 * `like_weight_ratio`) from the Pquery. The `like_weight_ratio` of the remaining placements is not
 * changed.
 */
void filter_n_max_weight_placements( Pquery& pquery, size_t n )
{
    // Check if there is anything to do at all.
    if( pquery.placement_size() == 0 || pquery.placement_size() <= n ) {
        return;
    }

    // Sort and remove elements from the back until n are left
    // (this is not the cleanest solution, but works for now).
    sort_placements_by_weight( pquery );
    while( pquery.placement_size() > n ) {
        pquery.remove_placement_at( pquery.placement_size() - 1 );
    }
}

/**
 * @brief Remove all PqueryPlacement%s but the `n` most likely ones from all
 * @link Pquery Pqueries @endlink in the Sample.
 *
 * This function calls filter_n_max_weight_placements( Pquery& pquery, size_t n ) for all Pqueries
 * of the Sample. See this version of the function for more information.
 */
void filter_n_max_weight_placements( Sample& smp, size_t n )
{
    for( auto& pquery : smp ) {
        filter_n_max_weight_placements( pquery, n );
    }
}

/**
 * @brief Remove all PqueryPlacement%s that have a `like_weight_ratio` below the given threshold.
 */
void filter_min_weight_threshold( Pquery& pquery, double threshold )
{
    // This is certainly not the cleanest solution, as it might cause quite some relocations.
    // However, we'd need full write access to the placement vector, which is currently not
    // supported for reasons of encapsulation...
    size_t i = 0;
    while( i < pquery.placement_size() ) {
        if( pquery.placement_at(i).like_weight_ratio < threshold ) {
            pquery.remove_placement_at(i);
        } else {
            ++i;
        }
    }
}

/**
 * @brief Remove all PqueryPlacement%s that have a `like_weight_ratio` below the given threshold
 * from all @link Pquery Pqueries @endlink of the Sample.
 */
void filter_min_weight_threshold( Sample& smp,    double threshold )
{
    for( auto& pquery : smp ) {
        filter_min_weight_threshold( pquery, threshold );
    }
}

/**
 * @brief Remove all @link Pquery Pqueries@endlink which do not have at least one name that is in
 * the given keep list.
 *
 * If the Pquery has a PqueryName whose PqueryName::name value is in the `keep_list`, the Pquery is
 * kept. If none of its names is in the `keep_list`, the Pquery is removed.
 *
 * This is similar to filter_pqueries_keeping_names(), but not quite the opposite, as Pqueries can
 * have multiple names.
 */
void filter_pqueries_keeping_names( Sample& smp, std::unordered_set<std::string> keep_list )
{
    // Again, not the cleanest solution, see above functions for discussion.
    size_t i = 0;
    while( i < smp.pquery_size() ) {

        // See if the Pquery has a name in the list.
        bool keep = false;
        for( auto const& name : smp.pquery_at(i).names() ) {
            if( keep_list.count( name.name ) > 0 ) {
                keep = true;
            }
        }

        // If not, remove it. If so, move to the next Pquery.
        if( ! keep ) {
            smp.remove_pquery_at(i);
        } else {
            ++i;
        }
    }
}

/**
 * @brief Remove all @link Pquery Pqueries@endlink which have at least one name that is in
 * the given remove list.
 *
 * If the Pquery has a PqueryName whose PqueryName::name value is in the `remove_list`,
 * the Pquery is remove. If none of its names is in the `remove_list`, the Pquery is kept.
 *
 * This is similar to filter_pqueries_keeping_names(), but not quite the opposite, as Pqueries can
 * have multiple names.
 */
void filter_pqueries_removing_names( Sample& smp, std::unordered_set<std::string> remove_list )
{
    // Again, not the cleanest solution, see above functions for discussion.
    size_t i = 0;
    while( i < smp.pquery_size() ) {

        // See if the Pquery has a name in the list.
        bool remove = false;
        for( auto const& name : smp.pquery_at(i).names() ) {
            if( remove_list.count( name.name ) > 0 ) {
                remove = true;
            }
        }

        // If so, remove it. If not, move to the next Pquery.
        if( remove ) {
            smp.remove_pquery_at(i);
        } else {
            ++i;
        }
    }
}

/**
 * @brief Remove all @link Pquery Pqueries@endlink from the two Sample%s except the ones that
 * have names in common.
 *
 * This function builds the intersection of the set of names of both Samples and only keeps those
 * Pqueries that have a PqueryName with one of those names.
 */
void filter_pqueries_intersecting_names( Sample& sample_1, Sample& sample_2 )
{
    // Remove those pqueries from sample_2 which do not occur in sample_1.
    auto names = all_pquery_names( sample_1 );
    filter_pqueries_keeping_names( sample_2, names );

    // And vice versa (using the names of the already smaller sample_2).
    names = all_pquery_names( sample_2 );
    filter_pqueries_keeping_names( sample_1, names );
}

/**
 * @brief Remove all @link Pquery Pqueries@endlink from the two Sample%s that have a name in common.
 *
 * This function builds the intersection of the set of names of both Samples and removes all those
 * Pqueries that have a PqueryName with one of those names.
 *
 * This is not quite the same as building the symmetric difference and keeping those elements, and,
 * although similar, it not the opposite of filter_pqueries_intersecting_names(), because Pqueries
 * can have multiple names.
 */
void filter_pqueries_differing_names( Sample& sample_1, Sample& sample_2 )
{
    // Yep, yep, this is wasteful. But it works for now.

    // Get all sorted names in sample 1 ...
    auto names_1_u = all_pquery_names( sample_1 );
    auto names_1   = std::vector< std::string >( names_1_u.begin(), names_1_u.end() );
    std::sort( names_1.begin(), names_1.end() );
    names_1_u.clear();

    // ... and in sample 2.
    auto names_2_u = all_pquery_names( sample_2 );
    auto names_2   = std::vector< std::string >( names_2_u.begin(), names_2_u.end() );
    std::sort( names_2.begin(), names_2.end() );
    names_2_u.clear();

    // Get their intersection.
    std::unordered_set< std::string > symdiff;
    std::set_intersection(
        names_1.begin(), names_1.end(),
        names_2.begin(), names_2.end(),
        std::inserter( symdiff, symdiff.end() )
    );

    // Remove all intersecting elements from the sampels.
    filter_pqueries_removing_names( sample_1, symdiff );
    filter_pqueries_removing_names( sample_2, symdiff );
}

// =================================================================================================
//     Joining and Merging
// =================================================================================================

/**
 * @brief Copy all @link Pquery Pqueries @endlink from the source Sample (left parameter) to the
 * target Sample (right parameter).
 *
 * For this method to succeed, the PlacementTree%s of the Sample%s need to have the same topology,
 * including identical edge_nums and node names.
 * Otherwise, this function throws an `std::runtime_error`.
 *
 * The PlacementTree of the target Sample is not modified. If the average branch length tree is
 * needed instead, see SampleSet::merge_all().
 */
void copy_pqueries( Sample const& source, Sample& target )
{
    // Check for identical topology, taxa names and edge_nums.
    // We do not check here for branch_length, because usually those differ slightly.
    if( ! compatible_trees( source.tree(), target.tree() )) {
        throw std::runtime_error("Cannot join Samples, because their PlacementTrees differ.");
    }

    // We need to assign edge pointers to the correct edge objects, so we need a mapping.
    auto en_map = edge_num_to_edge_map( target.tree() );

    // Copy all (o)ther pqueries to (n)ew pqueries.
    for( const auto& opqry : source.pqueries() ) {
        auto npqry = Pquery();
        for( auto opit = opqry.begin_placements(); opit != opqry.end_placements(); ++opit ) {

            // Assuming that the trees have identical topology (checked at the beginning of this
            // function), there will be an edge for every placement. if this assertion fails,
            // something broke the integrity of our in memory representation of the data.
            assert( en_map.count( opit->edge_num() ) > 0 );

            npqry.add_placement( *en_map[ opit->edge_num() ], *opit );
        }
        for( auto name_it = opqry.begin_names(); name_it != opqry.end_names(); ++name_it ) {
            npqry.add_name( *name_it );
        }
        target.add_pquery( npqry );
    }
}

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

        // This is a list of the Pquery indices that we want to delete, because their placements
        // have been moved to other Pqueries (those, which contain the same name). We need this
        // list as deleting from a list while iterating it is not a good idea.
        std::vector<size_t> del;

        for( size_t i = 0; i < smp.pquery_size(); ++i ) {
            auto& pqry = smp.pquery_at(i);

            // Collect the Pqueries that can be merged with the current one, because they share
            // a common name.
            std::unordered_set<Pquery*> merges;
            for( auto name_it = pqry.begin_names(); name_it != pqry.end_names(); ++name_it ) {
                auto& name = *name_it;

                if (hash.count(name.name) > 0) {
                    merges.insert(hash[name.name]);
                }
            }

            if (merges.size() == 0) {
                // All names are new, so store them in the hash smp for later.
                // We don't need to do any merging in this case.
                for( auto name_it = pqry.begin_names(); name_it != pqry.end_names(); ++name_it ) {
                    auto& name = *name_it;

                    // We are here because we found no pquery to merge with. This means that the
                    // name cannot be in the hash smp already.
                    assert(hash.count(name.name) == 0);

                    // Now add it.
                    hash[name.name] = &pqry;
                }
            } else {
                // We need merging. We will merge with only one Pquery in this iteration. If there
                // are more than one Pqueries that we need to merge with (i.e., merges.size() > 1),
                // we will mark that we need another iteration and do the other mergings then.

                // Get the Pquery that we will merge into.
                Pquery* merge_into = *merges.begin();

                // TODO we could use move here instead of copy creating.

                // Add all placements to it.
                for( auto const& place : pqry.placements() ) {
                    merge_into->add_placement( place );
                }

                // Add all names. This will cause doubled names, but they can be reduced later
                // via merge_duplicate_names().
                // We could do the check here, but this would increase complexity and gain just a
                // bit of speed (probably).
                for( auto const& name : pqry.names() ) {
                    // Add the name to the Pquery.
                    merge_into->add_name( name );

                    // Also add it to the hash smp. If this was name that is responsible for the
                    // merging (the shared name), we will replace the entry by an indentical copy.
                    // For all other names of the current Pquery, we need this to ensure that the
                    // next Pqueries in the loop will find it.
                    hash[name.name] = merge_into;
                }

                // Mark the Pquery for deletion and delete its content
                // (this is both to save memory, but also for some assertions later).
                del.push_back(i);
                pqry.clear();

                // Check whether we need to merge with more than one Pquery, meaning that we have
                // transitive connections. This means, we need another iteration to resolve this.
                if (merges.size() > 1) {
                    need_iteration = true;
                }
            }
        }

        // Delete all Pqueries that were merged to others during this iteration.
        // We need to do this in reverse order so that the indeces are not messed up while deleting.
        for( auto it = del.rbegin(); it != del.rend(); ++it ) {
            // Assert that this is an empty pquery. We cleared the ones that are marked for
            // deletion, so in case that it is not empty, we are about to delete the wrong one!
            assert( smp.pquery_at( *it ).placement_size() == 0 );
            assert( smp.pquery_at( *it ).name_size()      == 0 );

            smp.remove_pquery_at( *it );
        }
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
    // Merge the placements into this map, indexed by the edge index they belong to.
    // Also, store how many placements have been merged into this one.
    std::unordered_map< size_t, std::pair< PqueryPlacement, size_t >> merge_units;

    for( auto pit = pquery.begin_placements(); pit != pquery.end_placements(); ++pit ) {
        auto const& place = *pit;
        auto edge_idx = place.edge().index();

        // For the first placement on each edge, make a copy.
        if( merge_units.count( edge_idx ) == 0 ) {
            merge_units[ edge_idx ] = { place, 0 };

        // For all others, add their values to the stored one.
        } else {
            auto& merge_into = merge_units[ edge_idx ].first;
            ++merge_units[ edge_idx ].second;

            merge_into.likelihood        += place.likelihood;
            merge_into.like_weight_ratio += place.like_weight_ratio;
            merge_into.proximal_length   += place.proximal_length;
            merge_into.pendant_length    += place.pendant_length;
            merge_into.parsimony         += place.parsimony;
        }
    }

    // Clear all previous placements and add back the averaged merged ones.
    pquery.clear_placements();
    for( auto& merge_unit : merge_units ) {
        auto& place = merge_unit.second.first;

        if( merge_unit.second.second > 1 ) {
            double denom = static_cast<double>( merge_unit.second.second );

            place.likelihood        /= denom;
            place.like_weight_ratio /= denom;
            place.proximal_length   /= denom;
            place.pendant_length    /= denom;
            place.parsimony         /= denom;
        }

        pquery.add_placement(place);
    }
}

/**
 * @brief Call merge_duplicate_placements( Pquery& ) for each Pquery of a Sample.
 */
void merge_duplicate_placements( Sample& smp )
{
    for( auto pquery_it = smp.begin(); pquery_it != smp.end(); ++pquery_it ) {
        merge_duplicate_placements( *pquery_it );
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
void merge_duplicate_names( Sample& smp )
{
    for( auto pquery_it = smp.begin(); pquery_it != smp.end(); ++pquery_it ) {
        merge_duplicate_names( *pquery_it );
    }
}

// =================================================================================================
//     Placement Mass
// =================================================================================================

/**
 * @brief Return the sum of all @link PqueryName::multiplicity multiplicities @endlink of the
 * Pquery.
 */
double total_multiplicity( Pquery const& pqry )
{
    double mult = 0.0;
    for( auto const& name : pqry.names() ) {
        mult += name.multiplicity;
    }
    return mult;
}

/**
 * @brief Get the total number of PqueryPlacement%s in all @link Pquery Pqueries @endlink of the
 * given Sample.
 */
size_t total_placement_count( Sample const& smp )
{
    size_t count = 0;
    for( auto const& pqry : smp.pqueries() ) {
        count += pqry.placement_size();
    }
    return count;
}

/**
 * @brief Get the summed mass of all PqueryPlacement%s in all @link Pquery Pqueries @endlink of the
 * given Sample, where mass is measured by the
 * @link PqueryPlacement::like_weight_ratio like_weight_ratios@endlink of the PqueryPlacement%s.
 *
 * Be aware that this function only gives the pure sum of the `like_weight_ratio`s. See
 * total_placement_mass_with_multiplicities() for a version of this function, which also takes
 * the @link PqueryName::multiplicity multiplicities @endlink of the Pqueries into account.
 */
double total_placement_mass(  Sample const& smp )
{
    double sum = 0.0;
    for( const auto& pqry : smp.pqueries() ) {
        for( auto& place : pqry.placements() ) {
            sum += place.like_weight_ratio;
        }
    }
    return sum;
}

/**
 * @brief Get the mass of all PqueryPlacement%s of the Sample, using the
 * @link PqueryName::multiplicity multiplicities @endlink as factors.
 *
 * This function returns the summed mass of all PqueryPlacement%s in all
 * @link Pquery Pqueries @endlink of the
 * given Sample, where mass is measured by `like_weight_ratio`, and the mass of each Pquery is
 * multiplied by the sum of the @link PqueryName::multiplicity multiplicities @endlink of this
 * Pquery.
 *
 * This method returns the same value as total_placement_mass() in case that the `multiplicity` is
 * left at its default value of 1.0.
 */
double total_placement_mass_with_multiplicities(  Sample const& smp )
{
    double sum = 0.0;
    for( const auto& pqry : smp.pqueries() ) {
        double mult = 0.0;
        for( auto const& name : pqry.names() ) {
            mult += name.multiplicity;
        }

        double lwr_sum = 0.0;
        for( auto const& place : pqry.placements() ) {
            lwr_sum += place.like_weight_ratio;
        }
        sum += lwr_sum * mult;
    }
    return sum;
}

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
            edge = &smp.tree().edge_at( it.first );
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
            edge = &smp.tree().edge_at( it.first );
            max  = sum;
        }
    }

    return std::make_pair(edge, max);
}

// =================================================================================================
//     Histograms
// =================================================================================================

std::vector<double> closest_leaf_weight_distribution( Sample const& sample )
{
    std::vector<double> distrib;

    // Get a vector telling us the depth from each node to its closest leaf node.
    auto depths = tree::closest_leaf_depth_vector( sample.tree() );

    for( auto const& pquery : sample.pqueries() ) {
        for( auto& place : pquery.placements() ) {
            // Try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            int dp = depths[ place.edge().primary_node().index()   ].second;
            int ds = depths[ place.edge().secondary_node().index() ].second;
            unsigned int ld = std::min(dp, ds);

            // Put the closer one into the histogram, resize if necessary.
            if( distrib.size() < ld + 1 ) {
                distrib.resize( ld + 1, 0.0 );
            }
            distrib[ld] += place.like_weight_ratio;
        }
    }

    return distrib;
}

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
    auto depths = tree::closest_leaf_depth_vector( smp.tree() );

    for( auto const& pquery : smp.pqueries() ) {
        for( auto& place : pquery.placements() ) {
            // Try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            int dp = depths[ place.edge().primary_node().index()   ].second;
            int ds = depths[ place.edge().secondary_node().index() ].second;
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
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; "
 *                  << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
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

    for (const auto& pquery : smp.pqueries()) {
        for( auto& place : pquery.placements() ) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place.pendant_length
                      + place.proximal_length
                      + dists[ place.edge().primary_node().index() ].second;
            double ds = place.pendant_length
                      + place.edge().data<PlacementEdgeData>().branch_length
                      - place.proximal_length
                      + dists[ place.edge().secondary_node().index() ].second;
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
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; "
 *                  << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
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
    for (const auto& pquery : smp.pqueries()) {
        for( auto& place : pquery.placements() ) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place.pendant_length
                      + place.proximal_length
                      + dists[ place.edge().primary_node().index() ].second;
            double ds = place.pendant_length
                      + place.edge().data<PlacementEdgeData>().branch_length
                      - place.proximal_length
                      + dists[ place.edge().secondary_node().index() ].second;
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
