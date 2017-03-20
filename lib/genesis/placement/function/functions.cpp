/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/placement/function/functions.hpp"

#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/utils/core/std.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Pquery Names
// =================================================================================================

bool has_name( Pquery const& pquery, std::string const& name )
{
    for( size_t i = 0; i < pquery.name_size(); ++i ) {
        if( pquery.name_at(i).name == name ) {
            return true;
        }
    }
    return false;
}

bool has_name( Sample const& smp, std::string const& name )
{
    for( auto const& pqry : smp.pqueries() ) {
        if( has_name( pqry, name ) ) {
            return true;
        }
    }
    return false;
}

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

void normalize_weight_ratios( Sample& smp )
{
    for( auto pqry_it = smp.begin(); pqry_it != smp.end(); ++pqry_it ) {
        normalize_weight_ratios( *pqry_it );
    }
}

// void sort_placements_by_proximal_length( PlacementTreeEdge& edge );
// void sort_placements_by_proximal_length( Sample& smp );

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

void sort_placements_by_weight( Sample& smp )
{
    for( auto pqry_it = smp.begin(); pqry_it != smp.end(); ++pqry_it ) {
        sort_placements_by_weight( *pqry_it );
    }
}

void scale_all_branch_lengths( Sample& smp, double factor )
{
    tree::scale_all_branch_lengths( smp.tree(), factor );
    for( auto& pqry : smp.pqueries() ) {
        for( auto& place : pqry.placements() ) {
            place.proximal_length *= factor;
        }
    }
}

// =================================================================================================
//     Filtering
// =================================================================================================

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

void filter_min_accumulated_weight( Sample& smp, double threshold )
{
    for( auto& pquery : smp ) {
        filter_min_accumulated_weight( pquery, threshold );
    }
}

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

void filter_n_max_weight_placements( Sample& smp, size_t n )
{
    for( auto& pquery : smp ) {
        filter_n_max_weight_placements( pquery, n );
    }
}

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

void filter_min_weight_threshold( Sample& smp,    double threshold )
{
    for( auto& pquery : smp ) {
        filter_min_weight_threshold( pquery, threshold );
    }
}

void filter_pqueries_keeping_names( Sample& smp, std::string const& regex )
{
    std::regex pattern( regex );
    auto new_past_the_end = std::remove_if(
        smp.begin(),
        smp.end(),
        [&] ( Pquery const& pqry ) {
            for( auto const& nm : pqry.names() ) {
                if( regex_match( nm.name, pattern ) ) {
                    return false;
                }
            }
            return true;
        }
    );
    smp.remove( new_past_the_end, smp.end() );
}

void filter_pqueries_keeping_names( Sample& smp, std::unordered_set<std::string> keep_list )
{
    // Again, not the cleanest solution, see above functions for discussion.
    size_t i = 0;
    while( i < smp.size() ) {

        // See if the Pquery has a name in the list.
        bool keep = false;
        for( auto const& name : smp.at(i).names() ) {
            if( keep_list.count( name.name ) > 0 ) {
                keep = true;
            }
        }

        // If not, remove it. If so, move to the next Pquery.
        if( ! keep ) {
            smp.remove(i);
        } else {
            ++i;
        }
    }
}

void filter_pqueries_removing_names( Sample& smp, std::string const& regex )
{
    std::regex pattern( regex );
    auto new_past_the_end = std::remove_if(
        smp.begin(),
        smp.end(),
        [&] ( Pquery const& pqry ) {
            for( auto const& nm : pqry.names() ) {
                if( regex_match( nm.name, pattern ) ) {
                    return true;
                }
            }
            return false;
        }
    );
    smp.remove( new_past_the_end, smp.end() );
}

void filter_pqueries_removing_names( Sample& smp, std::unordered_set<std::string> remove_list )
{
    // Again, not the cleanest solution, see above functions for discussion.
    size_t i = 0;
    while( i < smp.size() ) {

        // See if the Pquery has a name in the list.
        bool remove = false;
        for( auto const& name : smp.at(i).names() ) {
            if( remove_list.count( name.name ) > 0 ) {
                remove = true;
            }
        }

        // If so, remove it. If not, move to the next Pquery.
        if( remove ) {
            smp.remove(i);
        } else {
            ++i;
        }
    }
}

void filter_pqueries_intersecting_names( Sample& sample_1, Sample& sample_2 )
{
    // Remove those pqueries from sample_2 which do not occur in sample_1.
    auto names = all_pquery_names( sample_1 );
    filter_pqueries_keeping_names( sample_2, names );

    // And vice versa (using the names of the already smaller sample_2).
    names = all_pquery_names( sample_2 );
    filter_pqueries_keeping_names( sample_1, names );
}

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
        target.add( npqry );
    }
}

void merge_duplicates (Sample& smp)
{
    collect_duplicate_pqueries (smp);
    merge_duplicate_names      (smp);
    merge_duplicate_placements (smp);
}

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

        for( size_t i = 0; i < smp.size(); ++i ) {
            auto& pqry = smp.at(i);

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
            assert( smp.at( *it ).placement_size() == 0 );
            assert( smp.at( *it ).name_size()      == 0 );

            smp.remove( *it );
        }
    }
}

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

void merge_duplicate_placements( Sample& smp )
{
    for( auto pquery_it = smp.begin(); pquery_it != smp.end(); ++pquery_it ) {
        merge_duplicate_placements( *pquery_it );
    }
}

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

void merge_duplicate_names( Sample& smp )
{
    for( auto pquery_it = smp.begin(); pquery_it != smp.end(); ++pquery_it ) {
        merge_duplicate_names( *pquery_it );
    }
}

// =================================================================================================
//     Placement Mass
// =================================================================================================

double total_multiplicity( Pquery const& pqry )
{
    double mult = 0.0;
    for( auto const& name : pqry.names() ) {
        mult += name.multiplicity;
    }
    return mult;
}

double total_multiplicity( Sample const& sample )
{
    double mult = 0.0;
    for( auto const& pqry : sample.pqueries() ) {
        mult += total_multiplicity( pqry );
    }
    return mult;
}

size_t total_placement_count( Sample const& smp )
{
    size_t count = 0;
    for( auto const& pqry : smp.pqueries() ) {
        count += pqry.placement_size();
    }
    return count;
}

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

std::pair<PlacementTreeEdge const*, size_t> placement_count_max_edge( Sample const& smp )
{
    PlacementTreeEdge const* edge = nullptr;
    size_t                   max  = 0;

    auto place_map = placements_per_edge( smp );

    for( size_t edge_i = 0; edge_i < place_map.size(); ++edge_i ) {
        if( place_map[ edge_i ].size() > max ) {
            edge = &smp.tree().edge_at( edge_i );
            max  = place_map[ edge_i ].size();
        }
    }

    return std::make_pair(edge, max);
}

std::pair<PlacementTreeEdge const*, double> placement_mass_max_edge( Sample const& smp )
{
    PlacementTreeEdge const* edge = nullptr;
    double                   max  = 0.0;

    auto place_map = placements_per_edge( smp );

    for( size_t edge_i = 0; edge_i < place_map.size(); ++edge_i ) {
        double sum = 0.0;
        for( auto const& place : place_map[ edge_i ]) {
            sum += place->like_weight_ratio;
        }
        if (sum > max) {
            edge = &smp.tree().edge_at( edge_i );
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
