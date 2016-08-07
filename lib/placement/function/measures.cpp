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
 * @brief Implementation of PlacementMeasures class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/function/measures.hpp"

#include <assert.h>
#include <algorithm>
#include <map>
#include <stdexcept>
#ifdef PTHREADS
#    include <thread>
#endif

#include "placement/function/functions.hpp"
#include "placement/function/helper.hpp"
#include "placement/function/operators.hpp"
#include "placement/placement_tree.hpp"
#include "placement/pquery/plain.hpp"
#include "placement/sample_set.hpp"
#include "placement/sample.hpp"

#include "tree/default/distances.hpp"
#include "tree/function/distances.hpp"
#include "tree/function/emd_tree.hpp"
#include "tree/function/emd.hpp"
#include "tree/function/operators.hpp"
#include "tree/function/tree_set.hpp"
#include "tree/iterator/node_links.hpp"
#include "tree/iterator/postorder.hpp"
#include "tree/tree_set.hpp"
#include "tree/tree.hpp"

#include "utils/core/logging.hpp"
#include "utils/core/options.hpp"
#include "utils/math/histogram.hpp"
#include "utils/math/histogram/accumulator.hpp"
#include "utils/math/histogram/distances.hpp"
#include "utils/math/histogram/operators.hpp"
#include "utils/math/matrix.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Helper Method pquery_distance
// =================================================================================================

/**
 * @brief Calculates the normalized distance between two plain pqueries. It is mainly a helper
 * method for distance calculations (e.g., pairwise distance, variance).
 *
 * For each placement in the two pqueries, a distance is calculated, and their normalized sum is
 * returned. Normalization is done using the mass of placements in both pqueries.
 *
 * The distance between two placements is calculated as the shortest path between them. This
 * includes the their position on the branches, and - if specified - the pendant_length of both.
 * There are three cases that might occur:
 *
 *   1. **Both placements are on the same branch.**
 *      In this case, their distance is caluclated as their difference in proximal_lengths (plus
 *      if specified the sum of their pendant_lengths).
 *
 *   2. **The path between the placements includes the root.**
 *      The distance of a placement from its neighbouring nodes is usually given in form of the
 *      proximal_length, which is the distance of the placement to the node (at the end of its
 *      branch) that lies in direction of the root. Thus, there is an implicit notion of a root,
 *      that we need to consider. If the path between two placements contains the root, we can
 *      directly calculate their distance as the distance between the two promixal nodes plus
 *      proximal_lengths (and possibly pendant_lengths) of both placements. We call this the
 *      promixal-promixal case.
 *
 *   3. **The root is not part of the path between the placements.**
 *      This case means that one of the two placements lies on the path between the other placement
 *      and the root -- thus, the path between the placements does not contain the root.
 *      The distance between the placements cannot be calculated using the proximal_lengths
 *      directly, but we need to get the distal_length (away from the root) of the inner placement
 *      first. This is simply the difference between branch_length and proximal_length of that
 *      placement. Of course, this case comes in two flavours, because both placements can be the
 *      inner or outer one. They are called proximal-distal case and distal-proximal case,
 *      respectively.
 *
 * The first case is easy to detect by comparing the edge nums. However, distinguishing between the
 * latter two cases is expensive, as it involves finding the path to the root for both placements.
 * To speed this up, we instead use a distance matrix that is calculated in the beginning of any
 * algorithm using this method and contains the pairwise distances between all nodes of the tree.
 * Using this, we do not need to find paths between placements, but simply go to the nodes at the
 * end of the branches of the placements and do a lookup for those nodes.
 *
 * With this technique, we can calculate the distances between the placements for all
 * three cases (promixal-promixal, proximal-distal and distal-proximal) cheaply. The wanted distance
 * is then simply the minimum of those three distances. This is correct, because the two wrong cases
 * will always produce an overestimation of the distance.
 *
 * This distance is normalized using the `like_weight_ratio` of both placements, before
 * summing it up to calculate the total distance between the pqueries.
 */
double pquery_distance (
    const PqueryPlain&            pqry_a,
    const PqueryPlain&            pqry_b,
    const utils::Matrix<double>&  node_distances,
    bool                          with_pendant_length
) {
    double sum = 0.0;
    double pp, pd, dp, dist;

    // TODO once the proper dist measures (outside of this class) are in place, get rid of this function!

    for (const PqueryPlacementPlain& place_a : pqry_a.placements) {
        for (const PqueryPlacementPlain& place_b : pqry_b.placements) {
            if (place_a.edge_index == place_b.edge_index) {
                // same branch case
                dist = std::abs(place_a.proximal_length - place_b.proximal_length);

            } else {
                // proximal-proximal case
                pp = place_a.proximal_length
                   + node_distances(place_a.primary_node_index, place_b.primary_node_index)
                   + place_b.proximal_length;

                // proximal-distal case
                pd = place_a.proximal_length
                   + node_distances(place_a.primary_node_index, place_b.secondary_node_index)
                   + place_b.branch_length - place_b.proximal_length;

                // distal-proximal case
                dp = place_a.branch_length - place_a.proximal_length
                   + node_distances(place_a.secondary_node_index, place_b.primary_node_index)
                   + place_b.proximal_length;

                // find min of the three cases and
                dist = std::min(pp, std::min(pd, dp));
            }

            //  If needed, use pendant length; normalize it to the weight ratios.
            if (with_pendant_length) {
                dist += place_a.pendant_length + place_b.pendant_length;
            }
            dist *= place_a.like_weight_ratio * place_b.like_weight_ratio;
            sum  += dist;
        }
    }

    return sum;
}

// =================================================================================================
//     Earth Movers Distance
// =================================================================================================

/**
 * @brief Calculate the earth mover's distance between two Sample%s.
 *
 * This function interprets the @link PqueryPlacement::like_weight_ratio like_weight_ratios@endlink
 * of the PqueryPlacement%s as masses distributed along the branches of a tree. It then calculates
 * the earth mover's distance between those masses for the distrubitons induced by the two given
 * Sample%s.
 *
 * In order to do so, first, a tree with the average branch lengths of the two PlacementTree%s is
 * calculated. This is because of numerical issues that might yield different branch lengths.
 * This necessiates that the trees have the same topology. If not, an std::runtime_error is thrown.
 * The masses are then distributed on this tree, using the same relative position on their branches
 * that they had in their original trees.
 *
 * The calculation furthermore takes the @link PqueryName::multiplicity multiplicities@endlink of
 * the @link Pquery Pqueries@endlink into account. That means, pqueries with higher (total)
 * multiplicity have a higher influence on the calculated distance.
 *
 * As the two Sample%s might have a different total number of @link Pquery Pqueries@endlink, the
 * masses of the Samples are first normalized to 1.0, using all the
 * @link PqueryPlacement::like_weight_ratio like_weight_ratios@endlink and
 * @link PqueryName::multiplicity multiplicities@endlink of the Pqueries.
 * As a consequence, the resulting distance will not reflect the total number of Pqueries, but only
 * their relative (normalized) distrubution on the tree.
 *
 * See @link tree::earth_movers_distance( EmdTree const& ) earth_movers_distance( EmdTree const& )
 * @endlink for more information on the actual distance calculation.
 */
double earth_movers_distance (
    const Sample& lhs,
    const Sample& rhs,
    bool          with_pendant_length
) {
    // Get a tree with the average branch lengths of both provided trees.
    // This function also throws in case the trees have different topologies.
    tree::TreeSet tset;
    tset.add( "lhs", lhs.tree() );
    tset.add( "rhs", rhs.tree() );
    auto avg_length_tree = tree::average_branch_length_tree( tset );

    // Create an EMD tree from the average branch length tree.
    auto emd_tree = tree::convert(
        avg_length_tree,
        [] ( tree::BaseNodeData const& node_data ) {
            (void) node_data;
            return std::unique_ptr< tree::EmdNodeData >();
        },
        [] ( tree::BaseEdgeData const& edge_data ) {
            auto emd_edge = tree::EmdEdgeData::create();
            emd_edge->branch_length
                = dynamic_cast< PlacementEdgeData const& >( edge_data ).branch_length;

            return emd_edge;
        }
    );

    // Helper function that copies the masses from a sample to the emd tree.
    // It furthermore returns the amount of work needed to move the masses from their pendant
    // position to the branch (this result is only used if with_pendant_length is true).
    // Yep, this function does quite a lot of different things. But it's faster this way and it is
    // only a local function. Don't judge me.
    auto move_masses = [ &emd_tree ] ( Sample const& smp, double sign, double sum ) {
        double pendant_work = 0.0;

        for( auto const& pqry : smp.pqueries() ) {
            double multiplicity = total_multiplicity( pqry );

            for( auto const& place : pqry.placements() ) {
                auto& edge = emd_tree.edge_at( place.edge().index() );

                // Use the relative position of the mass on its original branch to put it to the
                // same position relative to its new branch.
                double position
                    = place.proximal_length
                    / place.edge().data<PlacementEdgeData>().branch_length
                    * edge.data<tree::EmdEdgeData>().branch_length;

                // Add the mass at that position, normalized and using the sign.
                edge.data<tree::EmdEdgeData>().masses[ position ]
                    += sign * place.like_weight_ratio * multiplicity / sum;

                // Accumulate the work we need to do to move the masses from their pendant
                // positions to the branches.
                pendant_work += place.like_weight_ratio * multiplicity * place.pendant_length / sum;
            }
        }

        return pendant_work;
    };

    // Use the sum of masses as normalization factor for the masses.
    double totalmass_l = total_placement_mass_with_multiplicities( lhs );
    double totalmass_r = total_placement_mass_with_multiplicities( rhs );

    // Copy masses of both samples to the EMD tree, with different signs.
    double pendant_work_l = move_masses( lhs, +1.0, totalmass_l );
    double pendant_work_r = move_masses( rhs, -1.0, totalmass_r );

    // Calculate EMD.
    double work = tree::earth_movers_distance( emd_tree );

    // If we also want the amount of work that was needed to move the placement masses from their
    // pendant position to the branch, we need to add those values.
    if( with_pendant_length ) {
        work += pendant_work_l + pendant_work_r;
    }

    return work;
}

/**
 * @brief Calculate the pairwise Earth Movers Distance for all Sample%s in a SampleSet.
 *
 * The result is a pairwise distance @link utils::Matrix Matrix@endlink using the indices of the
 * Sample%s in the SampleSet.
 * See @link earth_movers_distance( const Sample& lhs, const Sample& rhs, bool with_pendant_length )
 * the Sample version@endlink of this function for details on this distance measure.
 */
utils::Matrix<double> earth_movers_distance(
    SampleSet const& sample_set,
    bool             with_pendant_length
) {
    auto result = utils::Matrix<double>( sample_set.size(), sample_set.size(), 0.0 );

    for( size_t i = 0; i < sample_set.size(); ++i ) {

        // The result is symmetric - we only calculate the upper triangle.
        for( size_t j = i + 1; j < sample_set.size(); ++j ) {
            result(i, j) = earth_movers_distance(
                sample_set[i].sample,
                sample_set[j].sample,
                with_pendant_length
            );
            result(j, i) = result(i, j);
        }
    }
    return result;
}

// =================================================================================================
//     Center of Gravity
// =================================================================================================

/**
 * @brief Calculate the Center of Gravity of the placements on a tree.
 *
 * The center of gravity is the point on the tree where all masses of the placements on the one
 * side of it times their distance from the point are equal to this sum on the other side of the
 * point. In the following example, the hat `^` marks this point on a line with two placements:
 * One has mass 1 and distance 3 from the central point, and one as mass 3 and distance 1,
 * so that the product of their mass and distance to the point is the same:
 *
 *                   3
 *                   |
 *     1             |
 *     |_____________|
 *               ^
 *
 * It is thus like calculating masses and torques on a lever in order to find their physical
 * center of mass/gravity.
 *
 * This calculation is done for the whole tree, with the masses calculated from the
 * `like_weight_ratio` and distances in terms of the `branch_length` of the edges and the
 * `proximal_length` and (if specificed in the method parameter) the `pendant_length` of the
 * placements.
 */
std::pair<PlacementTreeEdge const*, double> center_of_gravity (
    const Sample& smp,
    bool          with_pendant_length
) {
    // This struct stores the torque that acts on a certain point (called the fulcrum) from a
    // specific direction. It also stores the mass that created that torque, in order to be able to
    // calculate the new torque when moving around the tree.
    // In physics, torque is distance times force. However, we consider the force to be constant in
    // the case of finding the center of gravity, so we neglect it and calculate torque as distance
    // times mass.
    struct Fulcrum {
        Fulcrum() : mass(0.0), torque(0.0) {}

        double mass;
        double torque;
    };

    // Disable debug messages while code is not in review.
    LOG_SCOPE_LEVEL(utils::Logging::kInfo)

    // Store a balance value per link, so that each element contains the mass and its torque that
    // lies downwards the tree in the direction of this link.
    std::unordered_map<PlacementTreeLink const*, Fulcrum> balance;

    // Prepare a map from edges to placements on those edges.
    auto place_map = placements_per_edge( smp );

    // -------------------------------------------------------------------------
    //     Collect All Masses, Calculate the Torque
    // -------------------------------------------------------------------------

    // Do a postorder traversal. Collect all placement masses and push them towards the root in
    // order to calculate the torque that acts on each node.
    for( auto it : postorder( smp.tree() ) ) {
        // Skip the last iteration, as we would assign an unneeded value to the first child
        // of the root.
        if( it.is_last_iteration() ) {
            continue;
        }

        // Collect the torque and mass that lies further down in the tree and act on the current
        // iterators link.
        Fulcrum curr_fulcrum;

        // Add up the masses from the current node's children.
        PlacementTreeLink const* link = &it.link().next();
        while( link != &it.link() ) {
            // We do postorder traversal, so we have seen the child links of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link));

            auto const& edge_data = it.edge().data<PlacementEdgeData>();
            curr_fulcrum.mass   += balance[link].mass;
            curr_fulcrum.torque += balance[link].mass * edge_data.branch_length;
            curr_fulcrum.torque += balance[link].torque;
            link    = &link->next();
        }

        // Add up the masses of placements on the current edge.
        for (PqueryPlacement const* place : place_map[ it.edge().index() ]) {
            double place_dist = place->proximal_length;
            if (with_pendant_length) {
                place_dist += place->pendant_length;
            }
            curr_fulcrum.mass   += place->like_weight_ratio;
            curr_fulcrum.torque += place->like_weight_ratio * place_dist;
        }

        assert( balance.count( &it.link().outer() ) == 0 );
        balance[ &it.link().outer() ] = curr_fulcrum;
    }

    // LOG_DBG << "current balance:";
    // for (auto& v : balance) {
    //     LOG_DBG1 << "node " << v.first->node()->name << ", mass " << v.second.mass
    //              << ", torque " << v.second.torque;
    // }

    // Now we have calculated all massed that lie down the tree as seen from the root and the torque
    // they create. We can now start finding the edge where the center of gravity lies. This is done
    // by going down the tree in the direction where the most torque comes from and at the same time
    // pulling with us all the masses that come from the other nodes. Once we have more torque from
    // behind us (speak: up in the tree) that lies ahead of us (speak: down the tree), we have found
    // the center edge.

    // -------------------------------------------------------------------------
    //     Find Central Edge
    // -------------------------------------------------------------------------

    // Keep track of the link whose edge we are currently examining, as well as the one that we
    // examined previously (on iteration of the loop earlier). We start at the root.
    PlacementTreeLink const* curr_link = &smp.tree().root_link();
    PlacementTreeLink const* prev_link = nullptr;

    // For asserting purposes, we keep track of the number of loop iterations we do.
    // This can never be more than the tree height (in number of nodes from root to deepest leaf)
    // plus one last iteration for going back towards the root.
    size_t num_iterations = 0;
    auto   depth_vector = node_path_length_vector(smp.tree());
    size_t max_iterations = 1 + static_cast<size_t>(
        *std::max_element(depth_vector.begin(), depth_vector.end())
    );
    // TODO turn this thing into a function similar to tree.height(), and name it aptly (find some
    // better convention to distinguish between depth [number of nodes on a path] and distance [sum
    // of branch lengths]. maybe distance and lengths instead?!).
    // TODO once the center of gravity method is established, this assertion might be removed.

    LOG_DBG << "max it " << max_iterations;

    // Loop until the balancing edge is found.
    while (true) {
        assert (num_iterations <= max_iterations);
        ++num_iterations;

        LOG_DBG << "iteration " << num_iterations;
        LOG_DBG1 << "find max at " << curr_link->node().data<PlacementNodeData>().name;

        // Find the direction away from the current node that has the highest torque.
        // At the same time, collect the sum of masses and torques at the node, in order to push
        // them towards the node with highest torque later (so that the next iteration will have
        // values to work on).
        PlacementTreeLink const* max_link   = nullptr;
        double                         max_torque = -1.0;
        Fulcrum                        sum;

        for( auto it_l : node_links( curr_link->node() )) {
            // Make sure that we actually have a useable value.
            assert(balance.count( &it_l.link() ) > 0);

            LOG_DBG2 << "at " <<  it_l.link().outer().node().data<PlacementNodeData>().name
                     << " with mass "  << balance[ &it_l.link() ].mass
                     << " and torque "  << balance[ &it_l.link() ].torque;
            if (balance[ &it_l.link() ].torque > max_torque) {
                max_link   = &it_l.link();
                max_torque = balance[ &it_l.link() ].torque;
            }
            sum.mass   += balance[ &it_l.link() ].mass;
            sum.torque += balance[ &it_l.link() ].torque;
        }
        assert(max_link);

        // Check if we found the edge where the center of gravity lies. This is the case when the
        // the highest torque is coming from the direction where we came just from in the last
        // iteration.
        LOG_DBG1 << "moving to " << max_link->outer().node().data<PlacementNodeData>().name;
        if( &max_link->outer() == prev_link ) {
            LOG_DBG << "found between "
                    << curr_link->node().data<PlacementNodeData>().name
                    << " and " << prev_link->node().data<PlacementNodeData>().name;
            break;
        }

        // If we are not done yet, move down the edge.
        prev_link = max_link;
        curr_link = &max_link->outer();

        LOG_DBG1 << "mass sum " << sum.mass << ", torque sum " << sum.torque;

        // Now we are at a node where we have calculated only the masses and torques coming from
        // further down in the tree so far, but not the values coming from the direction of the root
        // (from where we just came). So we need to calculate these now:

        // Subtract the mass and torque of the direction where we found the most torque again,
        // so that all that is left are the sums of all the other (not maximum) nodes. Then push it
        // towards to the end of the edge.
        auto const& max_edge_data = max_link->edge().data<PlacementEdgeData>();
        sum.mass   -= balance[max_link].mass;
        sum.torque -= balance[max_link].torque;
        sum.torque += sum.mass * max_edge_data.branch_length;

        // Add masses of the placements on this edge.
        for( PqueryPlacement const* place : place_map[ max_link->edge().index() ]) {
            double p_dist = max_edge_data.branch_length - place->proximal_length;
            if (with_pendant_length) {
                p_dist += place->pendant_length;
            }
            sum.mass   += place->like_weight_ratio;
            sum.torque += place->like_weight_ratio * p_dist;
        }

        // Store the values at the corresponding link.
        balance[curr_link] = sum;
        LOG_DBG1 << "stored mass " << sum.mass << " and torque " << sum.torque << " at "
                 << max_link->outer().node().data<PlacementNodeData>().name;

        LOG_DBG << "end of iteration " << num_iterations << "\n";
    }

    // Assert that the two links are actually the two ends of the same edge and that their nodes
    // are the correct ones in terms of direction to the root.
    assert( &curr_link->edge() == &prev_link->edge() );
    assert( &prev_link->node() == &prev_link->edge().primary_node() );
    assert( &curr_link->node() == &curr_link->edge().secondary_node() );

    // LOG_DBG << "current balance:";
    // for (auto& v : balance) {
    //     LOG_DBG1 << "node " << v.first->node()->name << ", mass " << v.second.mass
    //              << ", torque " << v.second.torque;
    // }

    LOG_DBG << "cur  " << curr_link->node().data<PlacementNodeData>().name
            << " with mass " << balance[curr_link].mass << " and torque " << balance[curr_link].torque;
    LOG_DBG << "prev " << prev_link->node().data<PlacementNodeData>().name
            << " with mass " << balance[prev_link].mass << " and torque " << balance[prev_link].torque;

    // At this point, we have found the central edge that balances the placement masses on the tree.
    // curr_link is at the downwards (away from the root) end of this edge, while prev_link at its
    // top (towards the root).
    // All that is left now is to find the correct position on this edge. For this, we need to
    // consider the masses and torques that come from both ends of the edge, as well as the
    // placements on the edge itself.

    PlacementTreeEdge const* const central_edge = &curr_link->edge();

    // -------------------------------------------------------------------------
    //     Calculate Torques at Central Edge
    // -------------------------------------------------------------------------

    // Define the masses and torques at both ends of the edge: proximal and distal mass/torque.
    // Calculate them as the sums of the values from the subtree that lies behind the edge.
    Fulcrum prox_fulcrum;
    Fulcrum dist_fulcrum;

    assert(balance.count(curr_link) > 0);
    assert(balance.count(prev_link) > 0);

    PlacementTreeLink const* link;
    link = &prev_link->next();
    while (link != prev_link) {
        assert(balance.count(link));
        prox_fulcrum.mass   += balance[link].mass;
        prox_fulcrum.torque += balance[link].torque;
        link = &link->next();
    }

    link = &curr_link->next();
    while (link != curr_link) {
        assert(balance.count(link));
        dist_fulcrum.mass   += balance[link].mass;
        dist_fulcrum.torque += balance[link].torque;
        link = &link->next();
    }

    LOG_DBG << "prox_mass " << prox_fulcrum.mass << ", prox_torque " << prox_fulcrum.torque;
    LOG_DBG << "dist_mass " << dist_fulcrum.mass << ", dist_torque " << dist_fulcrum.torque;

    // A simple approximation of the solution is to calculate the balancing point on the edge
    // without considering the influence of the placements on the edge:
    // Let x the solution, measured as length from the proximal node.
    // Then we are looking for an x where the weights on both sides of it are in equilibrium:
    //         prox_torque + (prox_mass * x) = dist_torque + (dist_mass * (branch_length - x))
    //     <=> x = (dist_torque - prox_torque + (dist_mass * branch_length)) / (dist_mass + prox_mass)
    // This however does not work when the mass that produced the most torque lies on the edge
    // itself. The approximation breaks then and gives a length outside of the edge as result. Thus,
    // we cannot ignore it in such a case. For example, this happens when all mass is on one edge.
    // So, we need to check for those cases and if so, use the center of the edge as the
    // approximation.
    // In code:
    double approx_proximal_length;
    auto const& central_edge_data = central_edge->data<PlacementEdgeData>();
    if (prox_fulcrum.mass == 0.0 || dist_fulcrum.mass == 0.0) {
        approx_proximal_length = central_edge_data.branch_length / 2.0;
    } else {
        approx_proximal_length = (dist_fulcrum.torque - prox_fulcrum.torque
                               + (dist_fulcrum.mass * central_edge_data.branch_length))
                               / (dist_fulcrum.mass + prox_fulcrum.mass);
    }

    LOG_DBG << "approx_proximal_length " << approx_proximal_length;
    // return std::make_pair(central_edge, approx_proximal_length);

    // We will do an iteration that moves along the edge, balancing the torques on both sides until
    // equilibrium is found. For this, we need to keep track of the masses and torques on the two
    // sides: the variables hold those values as seen from the point that we are trying to find.
    // At first, the prox_sum contains just prox_fulcrum, while dist_sum contains all values on the
    // other side. During the iteration later, those two variables will change while moving along
    // the edge, until equilibrium.
    Fulcrum prox_sum = prox_fulcrum;
    Fulcrum dist_sum = balance[prev_link];

    LOG_DBG << "prox_sum mass " << prox_sum.mass << ", prox_sum torque " << prox_sum.torque;
    LOG_DBG << "dist_sum mass " << dist_sum.mass << ", dist_sum torque " << dist_sum.torque;

    // At this point, the torque on the proximal end of the edge cannot exceed the one on the other
    // side, as this would mean that we have chosen the wrong edge as central edge.

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // TODO the assertion is wrong. there are valid cases where it does not hold.
    LOG_DBG1 << "assert " << dist_sum.torque <<  ">=" << prox_sum.torque;
    // assert(dist_sum.torque >= prox_sum.torque);

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // -------------------------------------------------------------------------
    //     Find Center of the Edge
    // -------------------------------------------------------------------------

    // We store the influence that each placement on the edge has.
    struct BalancePoint
    {
        BalancePoint()                : proximal_length(0.0),      mass(0.0), pendant_torque(0.0) {};
        BalancePoint(double prox_len) : proximal_length(prox_len), mass(0.0), pendant_torque(0.0) {};

        double proximal_length;
        double mass;
        double pendant_torque;
    };

    // Make a list of all placements on the edge, sorted by their position on it.
    // Also, as first and last element of the array, we store dummy elements for the proximal_length.
    // This makes it obsolete to check them as boundary cases.
    // We use a hand-sorted vector here (as opposed to a map, which does the ordering for us),
    // because it provides element access "[]", which comes in handy later.
    std::vector<BalancePoint> edge_balance;
    edge_balance.reserve( place_map[ central_edge->index() ].size() + 2);
    edge_balance.push_back(BalancePoint(0.0));

    double tqs = 0.0;
    double mss = 0.0;

    // Sorts the placements on the central edge by their distance from the root, ascending.
    auto sort_by_pos = [] ( PqueryPlacement const* lhs, PqueryPlacement const* rhs ) {
        return lhs->proximal_length < rhs->proximal_length;
    };
    std::sort(
        place_map[ central_edge->index() ].begin(),
        place_map[ central_edge->index() ].end(),
        sort_by_pos
    );

    // Now add all placements on the edge to the balance variable, sorted by their proximal length.
    for( PqueryPlacement const* place : place_map[ central_edge->index() ]) {
        double place_prox = place->proximal_length;

        // Some sanity checks for wrong data. We do it here because otherwise the algorithm might
        // produce weird results. However, usually this task is up to the validate() method.
        if (place_prox > central_edge_data.branch_length) {
            LOG_WARN << "Placement found that has proximal_length > branch_length.";
            place_prox = central_edge_data.branch_length;
        }
        if (place_prox < 0.0) {
            LOG_WARN << "Placement found that has proximal_length < 0.0.";
            place_prox = 0.0;
        }

        double place_pendant_torque = 0.0;
        if (with_pendant_length) {
            place_pendant_torque = place->like_weight_ratio * place->pendant_length;
        }

        tqs += place_prox * place->like_weight_ratio;
        mss += place->like_weight_ratio;

        BalancePoint place_balance;
        place_balance.proximal_length = place_prox;
        place_balance.mass            = place->like_weight_ratio;
        place_balance.pendant_torque  = place_pendant_torque;

        edge_balance.push_back(place_balance);
    }

    tqs += dist_fulcrum.torque - prox_fulcrum.torque
        + (dist_fulcrum.mass * central_edge_data.branch_length);
    mss += dist_fulcrum.mass + prox_fulcrum.mass;
    double solution_without_pendant_length = tqs / mss;
    LOG_DBG << "tqs " << tqs << ", mss " << mss;
    LOG_DBG << "solution_without_pendant_length " << solution_without_pendant_length;

    return std::make_pair(central_edge, solution_without_pendant_length);

    // The rest code in this function tries to find a solution when pendant length shall be
    // considered. However, it might happen that there is no exact solution in this case,
    // so we decided to ignore pendant lengths on the central branch and return the result
    // above instead.

    // -------------------------------------------------------------------------
    //     Experimental Extension
    // -------------------------------------------------------------------------

    // Finally, story the dummy for the end of the edge.
    edge_balance.push_back(BalancePoint( central_edge_data.branch_length ));

    LOG_DBG << "edge_balance:";
    for (auto& e : edge_balance) {
        LOG_DBG1 << "at " << e.proximal_length << " with mass " << e.mass
                 << " and pen torque " << e.pendant_torque;
    }

    LOG_DBG << "prox_sum mass " << prox_sum.mass << ", prox_sum torque " << prox_sum.torque;
    LOG_DBG << "dist_sum mass " << dist_sum.mass << ", dist_sum torque " << dist_sum.torque;

    // This is the loop where we find the center of the edge.
    size_t pos       = 1;
    double dist_diff = 0.0;
    for (; pos < edge_balance.size(); ++pos) {
        auto& curr_point = edge_balance[pos];

        // Get the distance that we travelled from the last point on the edge. This is important to
        // know how much to change the torques.
        dist_diff = curr_point.proximal_length - edge_balance[pos-1].proximal_length;

        LOG_DBG1 << "iteration " << pos;

        LOG_DBG1 << "at " << curr_point.proximal_length << " with mass " << curr_point.mass
                 << " and pen torque " << curr_point.pendant_torque;

        LOG_DBG2 << "dist diff " << dist_diff;

        LOG_DBG2 << "prox_sum mass " << prox_sum.mass << ", prox_sum torque " << prox_sum.torque;
        LOG_DBG2 << "dist_sum mass " << dist_sum.mass << ", dist_sum torque " << dist_sum.torque;

        // double pendant_torque_sum = 0.0;
        // for (size_t i = 1; i < edge_balance.size(); ++i) {
        //     if (edge_balance[i].proximal_length == curr_point.proximal_length) {
        //         pendant_torque_sum += edge_balance[i].pendant_torque;
        //     } else if (i > pos) {
        //         break;
        //     }
        // }
        // LOG_DBG2 << "pendant_torque_sum " << pendant_torque_sum;

        if (
            prox_sum.torque + prox_sum.mass * dist_diff >=
            dist_sum.torque - dist_sum.mass * dist_diff
        ) {
            break;
        }

        // Adjust the torques to the new point.
        prox_sum.torque += prox_sum.mass * dist_diff + curr_point.pendant_torque;
        dist_sum.torque -= dist_sum.mass * dist_diff + curr_point.pendant_torque;

        // Also the masses: the mass of the current point moves from the distal fulcrum to the
        // proximal one.
        prox_sum.mass   += curr_point.mass;
        dist_sum.mass   -= curr_point.mass;

        LOG_DBG2 << "new prox_sum mass " << prox_sum.mass << ", prox_sum torque " << prox_sum.torque;
        LOG_DBG2 << "new dist_sum mass " << dist_sum.mass << ", dist_sum torque " << dist_sum.torque;
    }

    LOG_DBG << "final prox_sum mass " << prox_sum.mass << ", prox_sum torque " << prox_sum.torque;
    LOG_DBG << "final dist_sum mass " << dist_sum.mass << ", dist_sum torque " << dist_sum.torque;

    LOG_DBG << "pos " << pos << " size " << edge_balance.size();

    // If the algorithm is correct, we will never finish the last iteration of the loop above,
    // because that would imply that we still did not find our central part of the edge. We might
    // leave the loop (via break) not until the last iteration (which means, that this is where the
    // center lies), but we will never finish the loop via its default exit condition.
    // So let's assert that we actually didn't.
    assert(pos < edge_balance.size());

    // if (pos == edge_balance.size() - 1) {
    //     /* code */
    // }

    dist_sum.torque -= dist_sum.mass * dist_diff;
    double result_proximal_length = (dist_sum.torque - prox_sum.torque
                                  + (dist_sum.mass * dist_diff))
                                  / (dist_sum.mass + prox_sum.mass);
    LOG_DBG << "result_proximal_length " << result_proximal_length;
    result_proximal_length += edge_balance[pos-1].proximal_length;
    LOG_DBG << "result_proximal_length " << result_proximal_length;

    return std::make_pair(central_edge, result_proximal_length);
}

/**
 * @brief
 *
 * \f$ Var(X) = E[ (x - \mu)^2 ] = \frac{\sum (x - \mu)^2 \cdot \omega} {\sum \omega} \f$, where
 * the weights \f$ \omega \f$ are the `like_weight_ratio`s of the placements.
 */
double center_of_gravity_variance (
    const Sample& smp,
    bool          with_pendant_length
) {
    double variance = 0.0;
    double mass     = 0.0;

    auto const   cog               = center_of_gravity(smp, with_pendant_length);
    auto const   central_edge      = cog.first;
    auto const&  central_edge_data = central_edge->data<PlacementEdgeData>();
    double const proximal_length   = cog.second;

    LOG_DBG << "edge " << central_edge->primary_node().data<PlacementNodeData>().name
            << " "     << central_edge->secondary_node().data<PlacementNodeData>().name;
    LOG_DBG << "prox " << proximal_length;

    auto node_dist_prox = node_branch_length_distance_vector(
        smp.tree(), &central_edge->primary_node()
    );
    auto node_dist_dist = node_branch_length_distance_vector(
        smp.tree(), &central_edge->secondary_node()
    );

    for (const auto& pqry : smp.pqueries()) {
        for( auto const& place : pqry.placements() ) {
            auto const& place_edge_data = place.edge().data<PlacementEdgeData>();
            double distance;

            if (place.edge().index() == central_edge->index()) {
                distance = std::abs(place.proximal_length - proximal_length);
            } else{
                double pp, pd, dp;

                // proximal-proximal case
                pp = proximal_length
                   + node_dist_prox[ place.edge().primary_node().index() ]
                   + place.proximal_length;

                // proximal-distal case
                pd = proximal_length
                   + node_dist_prox[ place.edge().secondary_node().index() ]
                   + place_edge_data.branch_length - place.proximal_length;

                // distal-proximal case
                dp = central_edge_data.branch_length - proximal_length
                   + node_dist_dist[ place.edge().primary_node().index() ]
                   + place.proximal_length;

                // find min of the three cases
                distance = std::min(pp, std::min(pd, dp));
            }

            if (with_pendant_length) {
                distance += place.pendant_length;
            }
            variance += distance * distance * place.like_weight_ratio;
            mass     += place.like_weight_ratio;
        }
    }

    return variance / mass;
}

/**
 * @brief
 */
double center_of_gravity_distance (
    const Sample& smp_a,
    const Sample& smp_b,
    bool          with_pendant_length
) {
    if (!compatible_trees(smp_a, smp_b)) {
        throw std::invalid_argument("__FUNCTION__: Incompatible trees.");
    }

    // Disable debug messages while code is not in review.
    LOG_SCOPE_LEVEL(utils::Logging::kInfo)

    auto cog_a = center_of_gravity(smp_a, with_pendant_length);
    auto cog_b = center_of_gravity(smp_b, with_pendant_length);

    auto edge_a = cog_a.first;
    auto edge_b = cog_b.first;

    double prox_a = cog_a.second;
    double prox_b = cog_b.second;

    // TODO this is for testing purposes only
    if (prox_a < 0.0) {
        LOG_INFO << "map a COG proximal_length < 0: " << prox_a;
    }

    LOG_DBG << "cog a edge " << edge_a->index() << " prox " << prox_a;
    LOG_DBG << "cog b edge " << edge_b->index() << " prox " << prox_b;

    // TODO turn the result of cog method into a class TreePoint or so, and write functions for this
    // like distance to another point, which uses the code below. then replace the code by calling
    // this function.

    double dist = -1.0;
    if (edge_a->index() == edge_b->index()) {
        // same branch case
        dist = std::abs(prox_a - prox_b);
    } else {
        // TODO instead of the whole vector, we need a distnace between nodes function in the future,
        // which probably uses the path iterator.
        auto node_dist_a_prox = node_branch_length_distance_vector(
            smp_a.tree(), &edge_a->primary_node()
        );
        auto node_dist_a_dist = node_branch_length_distance_vector(
            smp_a.tree(), &edge_a->secondary_node()
        );

        double pp, pd, dp;

        // proximal-proximal case
        pp = prox_a
           + node_dist_a_prox[edge_b->primary_node().index()]
           + prox_b;

        // proximal-distal case
        pd = prox_a
           + node_dist_a_prox[edge_b->secondary_node().index()]
           + edge_b->data<PlacementEdgeData>().branch_length - prox_b;

        // distal-proximal case
        dp = edge_a->data<PlacementEdgeData>().branch_length - prox_a
           + node_dist_a_dist[edge_b->primary_node().index()]
           + prox_b;

        // find min of the three cases and
        dist = std::min(pp, std::min(pd, dp));
    }

    return dist;
}

// =================================================================================================
//     Pairwise Distance
// =================================================================================================

/**
 * @brief Calculate the normalized pairwise distance between all placements of the two Samples.
 *
 * This method calculates the distance between two Sample%s as the normalized sum of the distances
 * between all pairs of @link Pquery Pqueries @endlink in the Sample. It is similar to the
 * variance() calculation, which calculates this sum for the squared distances between all Pqueries
 * of one Sample.
 *
 * @param  smp_a               First Sample to which the distances shall be calculated to.
 * @param  smp_b               Second Sample to which the distances shall be calculated to.
 * @param  with_pendant_length Whether or not to include all pendant lengths in the calculation.
 *
 * @return                         Distance value.
 */
double pairwise_distance (
    const Sample& smp_a,
    const Sample& smp_b,
    bool          with_pendant_length
) {
    if (!compatible_trees(smp_a, smp_b)) {
        throw std::invalid_argument("__FUNCTION__: Incompatible trees.");
    }

    // Init.
    double sum = 0.0;

    // Create PqueryPlain objects for every placement and copy all interesting data into it.
    // This way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. this gives a tremendous speedup!
    std::vector<PqueryPlain> pqueries_a = plain_queries( smp_a );
    std::vector<PqueryPlain> pqueries_b = plain_queries( smp_b );

    // Calculate a matrix containing the pairwise distance between all nodes. This way, we
    // do not need to search a path between placements every time. We use the tree of the first smp
    // here, ignoring branch lengths on tree b.
    // FIXME this might be made better by using average or so in the future.
    auto node_distances = node_branch_length_distance_matrix(smp_a.tree());

    for (const PqueryPlain& pqry_a : pqueries_a) {
        for (const PqueryPlain& pqry_b : pqueries_b) {
            sum += pquery_distance(pqry_a, pqry_b, node_distances, with_pendant_length);
        }
    }

    // Return normalized value.
    return sum / total_placement_mass( smp_a ) / total_placement_mass( smp_b );
}

// =================================================================================================
//     Node-Based Distances
// =================================================================================================

/**
 * @brief Local helper function to calculate a histogram of distances from each PqueryPlacement
 * in a Sample to a given @link tree::TreeNode Node@endlink of the @link PlacementTree Tree@endlink.
 */
utils::Histogram node_distance_histogram (
    Sample const&                sample,
    size_t                       node_index,
    utils::Matrix<double> const& node_dists,
    double                       diameter,
    size_t                       histogram_bins
) {
    // Prepare proximal and distal distances of each placement to the node.
    double p_dist, d_dist, dist;

    // Fill an accumulator with the distances from all placments to the node.
    auto hist_acc = utils::HistogramAccumulator();
    for( auto const& pquery : sample ) {
        double mult = total_multiplicity( pquery );

        // Get the distance from the placement to the given node, and accumulate it.
        for( auto const& placement : pquery.placements() ) {

            p_dist = placement.proximal_length
                   + node_dists( node_index, placement.edge().primary_node().index() );

            d_dist = placement.edge().data<PlacementEdgeData>().branch_length
                   - placement.proximal_length
                   + node_dists( node_index, placement.edge().secondary_node().index() );

            dist = std::min( p_dist, d_dist );
            assert( dist >= 0.0 && dist <= diameter );

            // Accumulate at the distance, using the lwr and multiplicity as weight, so that the
            // total weight of a pquery sums up to the multiplicity.
            hist_acc.accumulate( dist, placement.like_weight_ratio * mult );
        }
    }

    // Use the accumulator to build the histogram.
    return hist_acc.build_uniform_ranges_histogram( histogram_bins, 0.0, diameter );
}

/**
 * @brief Local helper function to calculate the histograms of distances from all
 * @link tree::TreeNode Nodes@endlink of the @link PlacementTree Tree@endlink of a Sample to all
 * its PqueryPlacement%s.
 */
std::vector< utils::Histogram > node_distance_histograms (
    Sample const& sample,
    size_t        histogram_bins
) {
    // Prepare a vector of histograms for each node of the tree.
    auto hist_vec = std::vector< utils::Histogram >();
    hist_vec.reserve( sample.tree().node_count() );

    // Calculate the pairwise distance between all pairs of nodes.
    auto node_dists = node_branch_length_distance_matrix( sample.tree() );

    // Calculate the diameter of the tree, i.e., the longest distance between any two nodes.
    // This is used as upper bound for the histogram. Its calculation is the same as in
    // tree::diameter().
    double const diameter = *std::max_element( node_dists.begin(), node_dists.end() );

    // Prepare a counter for asserting that the node order follows the indices.
    size_t cnt = 0;
    (void) cnt;

    // Calculate the histogram for all nodes of the tree, and store it in the vector.
    for( auto it = sample.tree().begin_nodes(); it != sample.tree().end_nodes(); ++it ) {
        assert( it->get()->index() == cnt++ );

        hist_vec.push_back( node_distance_histogram(
            sample, it->get()->index(), node_dists, diameter, histogram_bins
        ));
    }
    assert( cnt == sample.tree().node_count() );

    return hist_vec;
}

/**
 * @brief Calculate the Node Histogram Distance of two Sample%s.
 */
double node_histogram_distance (
    Sample const& sample_a,
    Sample const& sample_b,
    size_t        histogram_bins
) {
    if( ! compatible_trees( sample_a, sample_b ) ) {
        throw std::invalid_argument( "Incompatible trees." );
    }

    // Get the histograms describing the distances from placements to all nodes.
    auto const hist_vec_a = node_distance_histograms( sample_a, histogram_bins );
    auto const hist_vec_b = node_distance_histograms( sample_b, histogram_bins );

    // If the trees are compatible (as ensured in the beginning of this function), they need to have
    // the same number of nodes. Thus, also there should be this number of histograms in the vectors.
    assert( hist_vec_a.size() == sample_a.tree().node_count() );
    assert( hist_vec_b.size() == sample_b.tree().node_count() );
    assert( hist_vec_a.size() == hist_vec_b.size() );

    double dist = 0.0;
    for( size_t i = 0; i < hist_vec_a.size(); ++i ) {
        dist += earth_movers_distance( hist_vec_a[i], hist_vec_b[i], true );
    }
    assert( dist >= 0.0 );

    // Return normalized distance.
    return dist / static_cast< double >( sample_a.tree().node_count() );
}

/**
 * @brief Calculate the
 * @link node_histogram_distance( Sample const&, Sample const&, size_t ) node_histogram_distance()@endlink
 * for every pair of Sample%s in the SampleSet.
 */
utils::Matrix<double> node_histogram_distance (
    SampleSet const& sample_set,
    size_t           histogram_bins
) {
    auto const set_size = sample_set.size();

    // Get the histograms for all samples.
    auto hist_vecs = std::vector< std::vector< utils::Histogram >>( set_size );
    for( size_t i = 0; i < set_size; ++i ) {

        // Check compatibility.
        // It suffices to check adjacent pairs of samples, as compatibility is transitive.
        if( i > 0 ) {
            if( ! compatible_trees( sample_set[ i - 1 ].sample, sample_set[ i ].sample )) {
                throw std::invalid_argument(
                    "Trees in SampleSet not compatible for calculating Node Histogram Distance."
                );
            }
        }

        // Calculate the histograms for every node of the sample.
        hist_vecs[ i ] = node_distance_histograms( sample_set[ i ].sample, histogram_bins );
        assert( hist_vecs[ i ].size() == sample_set[ i ].sample.tree().node_count() );
    }

    // Calculate distance matrix for every pair of samples.
    auto result = utils::Matrix<double>( set_size, set_size, 0.0 );
    for( size_t i = 0; i < set_size; ++i ) {

        // The result is symmetric - we only calculate the upper triangle.
        for( size_t j = i + 1; j < set_size; ++j ) {

            // Sum up the emd distances of the histograms for each node of the tree in the
            // two samples.
            double dist = 0.0;
            assert( hist_vecs[ i ].size() == hist_vecs[ j ].size() );
            for( size_t k = 0; k < hist_vecs[ i ].size(); ++k ) {
                dist += earth_movers_distance( hist_vecs[ i ][ k ], hist_vecs[ j ][ k ], true );
            }
            assert( dist >= 0.0 );

            // Store normalized distance.
            dist /= static_cast< double >( sample_set[ i ].sample.tree().node_count() );
            result(i, j) = dist;
            result(j, i) = dist;
        }
    }

    return result;
}

// =================================================================================================
//     Variance
// =================================================================================================

/**
 * @brief Internal function that calculates the sum of distances contributed by one pquery for
 * the variance. See variance() for more information.
 *
 * This function is intended to be called by variance() or variance_thread() -- it is not a
 * stand-alone function.
 */
double variance_partial (
    const PqueryPlain&              pqry_a,
    const std::vector<PqueryPlain>& pqrys_b,
    const utils::Matrix<double>&    node_distances,
    bool                            with_pendant_length
) {
    double partial = 0.0;

    for (const PqueryPlain& pqry_b : pqrys_b) {
        // Check whether it is same pquery (a=b, nothing to do, as their distance is zero),
        // or a pair of pqueries that was already calculated (a>b, skip it also).
        if (pqry_a.index >= pqry_b.index) {
            continue;
        }
        double dist = pquery_distance(pqry_a, pqry_b, node_distances, with_pendant_length);
        partial += dist * dist;
    }

    return partial;
}

/**
 * @brief Internal function that calculates the sum of distances for the variance that is
 * contributed by a subset of the pqueries. See variance() for more information.
 *
 * This function is intended to be called by variance() -- it is not a stand-alone function.
 * It takes an offset and an incrementation value and does an interleaved loop over the pqueries,
 * similar to the sequential version for calculating the variance.
 */
void variance_thread (
    const int                       offset,
    const int                       incr,
    const std::vector<PqueryPlain>* pqrys,
    const utils::Matrix<double>*    node_distances,
    double*                         partial,
    bool                            with_pendant_length
) {
    // Use internal variables to avoid false sharing.
    assert(*partial == 0.0);
    double tmp_partial = 0.0;

    // Iterate over the pqueries, starting at offset and interleaved with incr for each thread.
    for (size_t i = offset; i < pqrys->size(); i += incr) {
        // LOG_PROG(i, pqrys->size()) << "of Variance() finished (Thread " << offset << ").";

        const PqueryPlain& pqry_a = (*pqrys)[i];
        tmp_partial += variance_partial(pqry_a, *pqrys, *node_distances, with_pendant_length);
    }

    // Return the results.
    *partial = tmp_partial;
}

/**
 * @brief Calculate the variance of the placements on a tree.
 *
 * The variance is a measure of how far a set of items is spread out in its space
 * (http://en.wikipedia.org/wiki/variance). In many cases, it can be measured using the mean of the
 * items. However, when considering placements on a tree, this does not truly measure how far they
 * are from each other. Thus, this algorithm applies a different method of calculating the variance
 * in terms of squared deviations of all items from each other:
 * \f$ Var(X) = \frac{1}{n^2} \sum_{i=1}^n \sum_{j=1}^n \frac{1}{2}(x_i - x_j)^2 \f$,
 * where \f$ (x_i - x_j) \f$ denotes the distance between two placements.
 *
 * According to the formula above, each pair of placements is evaluated twice, and subsequently
 * their distance need to be halfed when being added to the sum of distanaces. Instead of that,
 * we calculate the distance for each pair only once, thus are able skip half the calculations, and
 * of course skip the division by two.
 *
 * Furthermore, the normalizing factor \f$ \frac{1}{n^2} \f$ of the variance usually contains the
 * number of elements being processed. However, as the placements are weighted by their
 * `like_weight_ratio`, we instead calculate `n` as the sum of the `like_weight_ratio` of all
 * placements. In case that for each pquery the ratios of all its placements sum up to 1.0, this
 * number will be equal to the number of pqueries (and thus be equal to the usual case of using the
 * number of elements). However, as this is not required (placements with small ratio can be
 * dropped, so that their sum per pquery is less than 1.0), we cannout simply use the count.
 */
double variance(
    const Sample& smp,
    bool          with_pendant_length
) {
    // Init.
    double variance = 0.0;

    // Create PqueryPlain objects for every placement and copy all interesting data into it.
    // This way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. This gives a tremendous speedup!
    std::vector<PqueryPlain> vd_pqueries = plain_queries( smp );

    // Also, calculate a matrix containing the pairwise distance between all nodes. this way, we
    // do not need to search a path between placements every time.
    auto node_distances = node_branch_length_distance_matrix(smp.tree());

#ifdef PTHREADS

    // Prepare storage for thread data.
    int num_threads = utils::Options::get().number_of_threads();
    std::vector<double>      partials(num_threads, 0.0);
    std::vector<std::thread> threads;

    // Start all threads.
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(
            &variance_thread,
            i, num_threads, &vd_pqueries, &node_distances,
            &partials[i],
            with_pendant_length
        );
        // threads[i] = new std::thread ();
    }

    // Wait for all threads to finish, collect their results.
    for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
        variance += partials[i];
    }

#else

    // Do a pairwise calculation on all placements.
    // int progress = 0;
    for (const PqueryPlain& pqry_a : vd_pqueries) {
        // LOG_PROG(++progress, vd_pqueries.size()) << "of Variance() finished.";
        variance += variance_partial(pqry_a, vd_pqueries, node_distances, with_pendant_length);
    }

#endif

    // Calculate normalizing factor. Should be the same value as given by placement_mass(),
    // however this calculation is probably faster, as we already have the plain values at hand.
    double mass = 0.0;
    for (const auto& pqry : vd_pqueries) {
        for (const auto& place : pqry.placements) {
            mass += place.like_weight_ratio;
        }
    }

    // Return the normalized value.
    return ((variance / mass) / mass);
}

} // namespace placement
} // namespace genesis
