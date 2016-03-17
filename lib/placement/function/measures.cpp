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

#include "placement/function/helper.hpp"
#include "placement/function/functions.hpp"
#include "placement/function/operators.hpp"
#include "placement/placement_tree.hpp"
#include "placement/sample.hpp"
#include "tree/default/distances.hpp"
#include "tree/distances.hpp"
#include "tree/iterator/postorder.hpp"
#include "utils/core/options.hpp"
#include "utils/math/histogram.hpp"
#include "utils/math/histogram/distances.hpp"

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
 * @brief Calculates the Earth Movers Distance between two sets of placements on a fixed reference
 * tree.
 */
double earth_movers_distance(
    const Sample& lhs,
    const Sample& rhs,
    bool                with_pendant_length
) {
    // keep track of the total resulting distance.
    double distance = 0.0;

    // store a per-node balance of mass. each element in this map contains information about how
    // much placement mass is pushing from the direction of this node towards the root.
    // caveat: the masses that are stored here are already fully pushed towards the root, but are
    // stored here using the node at the lower end of the branch as key.
    std::unordered_map<const PlacementTree::NodeType*, double> balance;

    // use the sum of masses as normalization factor for the masses.
    double totalmass_l = lhs.placement_mass();
    double totalmass_r = rhs.placement_mass();

    // Disable all debug messages for this function...
    LOG_SCOPE_LEVEL(utils::Logging::kInfo)

    LOG_DBG << "totalmass_l " << totalmass_l;
    LOG_DBG << "totalmass_r " << totalmass_r;

    auto place_map_l = placements_per_edge( lhs );
    auto place_map_r = placements_per_edge( rhs );

    // do a postorder traversal on both trees in parallel. while doing so, move placements
    // from the leaves towards the root and store their movement (mass * distance) in balance[].
    // in theory, it does not matter where we start the traversal - however, the positions of the
    // placements are given as "proximal_length" on their branch, which always points away from the
    // root. thus, if we decided to traverse from a different node than the root, we would have to
    // take this into account. so we do start at the root, to keep it simple.
    PlacementTree::ConstIteratorPostorder it_l = lhs.tree().begin_postorder();
    PlacementTree::ConstIteratorPostorder it_r = rhs.tree().begin_postorder();
    for (
        ;
        it_l != lhs.tree().end_postorder() && it_r != rhs.tree().end_postorder();
        ++it_l, ++it_r
    ) {
        LOG_DBG << "\033[1;31miteration at node " << it_l.node()->index_ << ": " << it_l.node()->data.name << "\033[0m";
        LOG_DBG << "current distance " << distance;

        // check whether both trees have identical topology. if they have, the ranks of all nodes
        // are the same. however, if not, at some point their ranks will differ.
        if (it_l.node()->rank() != it_r.node()->rank()) {
            throw std::invalid_argument("__FUNCTION__: Incompatible trees.");
        }

        // if we are at the last iteration, we reached the root, thus we have moved all masses now
        // and don't need to proceed. if we did, we would count an edge of the root again.
        if (it_l.is_last_iteration()) {
            LOG_DBG1 << "last iteration";
            // we do a check for the mass at the root here for debug purposes.
            double root_mass = 0.0;
            for (
                PlacementTree::NodeType::ConstIteratorLinks n_it = it_l.node()->begin_links();
                n_it != it_l.node()->end_links();
                ++n_it
            ) {
                assert(balance.count(n_it.link()->outer()->node()));
                root_mass += balance[n_it.link()->outer()->node()];
            }

            LOG_DBG << "Mass at root: " << root_mass;

            continue;
        }

        // check whether the data on both reference trees is the same. this has to be done after the
        // check for last iteration / root node, because we don't want to check this for the root.
        if (it_l.node()->data.name       != it_r.node()->data.name ||
            it_l.edge()->data.edge_num() != it_r.edge()->data.edge_num()
        ) {
            throw std::invalid_argument("__FUNCTION__: Inconsistent trees.");
        }

        // we now start a "normal" earth_movers_distance caluclation on the current edge. for this,
        // we store the masses of all placements sorted by their position on the branch.
        std::multimap<double, double> edge_balance;
        LOG_DBG1 << "placing on branch...";

        // add all placements of the branch from the left tree (using positive mass)...
        for( PqueryPlacement const* place :  place_map_l[ it_l.edge()->index() ] ) {
            if (with_pendant_length) {
                distance += place->like_weight_ratio() * place->pendant_length() / totalmass_l;
            }
            edge_balance.emplace(place->proximal_length(), +place->like_weight_ratio() / totalmass_l);

            // LOG_DBG2 << "placement   " << place->pquery->names[0]->name;
            LOG_DBG2 << "it_l edge   " << it_l.edge()->index_;
            LOG_DBG2 << "added dist  " << place->like_weight_ratio() * place->pendant_length() / totalmass_l;
            LOG_DBG2 << "new dist    " << distance;
            LOG_DBG2 << "emplaced at " << place->proximal_length() << ": " << +place->like_weight_ratio() / totalmass_l;
            LOG_DBG2;
        }

        // ... and the branch from the right tree (using negative mass)
        for( PqueryPlacement const* place :  place_map_r[ it_r.edge()->index() ] ) {
            if (with_pendant_length) {
                distance += place->like_weight_ratio() * place->pendant_length() / totalmass_r;
            }
            edge_balance.emplace(place->proximal_length(), -place->like_weight_ratio() / totalmass_r);

            // LOG_DBG2 << "placement   " << place->pquery->names[0]->name;
            LOG_DBG2 << "it_r edge   " << it_r.edge()->index_;
            LOG_DBG2 << "added dist  " << place->like_weight_ratio() * place->pendant_length() / totalmass_r;
            LOG_DBG2 << "new dist    " << distance;
            LOG_DBG2 << "emplaced at " << place->proximal_length() << ": " << -place->like_weight_ratio() / totalmass_r;
            LOG_DBG2;
        }

        LOG_DBG1 << "placed all.";

        // distribute placement mass between children of this node, and collect the remaining mass
        // in mass_s. mass_s then contains the rest mass of the subtree that could not be
        // distributed among the children and thus has to be moved upwards.
        double mass_s = 0.0;
        PlacementTree::LinkType* link = it_l.link()->next();
        while (link != it_l.link()) {
            // we do postorder traversal, so we have seen the child nodes of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link->outer()->node()));

            mass_s += balance[link->outer()->node()];
            link = link->next();
        }
        LOG_DBG1 << "subtrees mass_s " << mass_s;
        LOG_DBG1 << "entering standard emd part...";

        // start the earth_movers_distance with the mass that is left over from the subtrees...
        double cur_pos  = it_l.edge()->data.branch_length;
        double cur_mass = mass_s;

        LOG_DBG1 << "cur_pos  " << cur_pos;
        LOG_DBG1 << "cur_mass " << cur_mass;

        // ... and move it along the branch, balancing it with the placements found on the branches.
        // this is basically a standard earth_movers_distance calculation along the branch.
        std::multimap<double, double>::reverse_iterator rit;
        for (rit = edge_balance.rbegin(); rit != edge_balance.rend(); ++rit) {
            LOG_DBG2 << "at " << rit->first << " with " << rit->second;
            //~ LOG_INFO << "at " << rit->first << " with " << rit->second;
            //~ assert(cur_pos >= rit->first);
            LOG_DBG2 << "is " << cur_pos << " >= " << rit->first << "? with " << rit->second;
            //~ if (cur_pos < rit->first) {
                //~ LOG_INFO << "is " << cur_pos << " >= " << rit->first << "? with " << rit->second;
                //~ return -1.0;
            //~ }
            distance += std::abs(cur_mass) * (cur_pos - rit->first);
            LOG_DBG2 << "added dist " << std::abs(cur_mass) * (cur_pos - rit->first);
            LOG_DBG2 << "new dist   " << distance;

            cur_pos   = rit->first;
            cur_mass += rit->second;

            LOG_DBG2 << "cur_pos  " << cur_pos;
            LOG_DBG2 << "cur_mass " << cur_mass;
            LOG_DBG2;
        }

        // finally, move the rest to the end of the branch and store its mass in balance[],
        // so that it can be used for the nodes further up in the tree.
        distance += std::abs(cur_mass) * cur_pos;
        balance[it_l.node()] = cur_mass;

        LOG_DBG1 << "added dist " << std::abs(cur_mass) * cur_pos;
        LOG_DBG1 << "new dist   " << distance;
        LOG_DBG1 << "balance at node " << it_l.node()->index_ << ": " << it_l.node()->data.name << " = " << cur_mass;
        LOG_DBG1 << "finished standard emd part";
        LOG_DBG1;
    }

    // check whether we are done with both trees.
    if (it_l != lhs.tree().end_postorder() || it_r != rhs.tree().end_postorder()) {
        throw std::invalid_argument("__FUNCTION__: Incompatible trees.");
    }

    LOG_DBG << "final distance: " << distance;
    return distance;
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
std::pair<PlacementTreeEdge*, double> center_of_gravity (
    const Sample& map,
    bool                with_pendant_length
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
    std::unordered_map<PlacementTree::LinkType const*, Fulcrum> balance;

    // Prepare a map from edges to placements on those edges.
    auto place_map = placements_per_edge( map );

    // -------------------------------------------------------------------------
    //     Collect All Masses, Calculate the Torque
    // -------------------------------------------------------------------------

    // Do a postorder traversal. Collect all placement masses and push them towards the root in
    // order to calculate the torque that acts on each node.
    for (
        PlacementTree::ConstIteratorPostorder it = map.tree().begin_postorder();
        it != map.tree().end_postorder();
        ++it
    ) {
        // Node does not have a corresponding edge (this was e.g. the case in the old tree
        // implementation, where the root had a "virtual" up link; this is not the case any more,
        // but maybe it will be re-introduced some day, and it does no harm to leave it here).
        // Also skip the last iteration, as we would assign an unneeded value to the first child
        // of the root.
        if (!it.edge() || it.is_last_iteration()) {
            continue;
        }

        // Collect the torque and mass that lies further down in the tree and act on the current
        // iterators link.
        Fulcrum curr_fulcrum;

        // Add up the masses from the current node's children.
        PlacementTree::LinkType* link = it.link()->next();
        while (link != it.link()) {
            // We do postorder traversal, so we have seen the child links of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link));

            curr_fulcrum.mass   += balance[link].mass;
            curr_fulcrum.torque += balance[link].mass * it.edge()->data.branch_length;
            curr_fulcrum.torque += balance[link].torque;
            link    = link->next();
        }

        // Add up the masses of placements on the current edge.
        for (PqueryPlacement const* place : place_map[ it.edge()->index() ]) {
            double place_dist = place->proximal_length();
            if (with_pendant_length) {
                place_dist += place->pendant_length();
            }
            curr_fulcrum.mass   += place->like_weight_ratio();
            curr_fulcrum.torque += place->like_weight_ratio() * place_dist;
        }

        assert(balance.count(it.link()->outer()) == 0);
        balance[it.link()->outer()] = curr_fulcrum;
    }

    // LOG_DBG << "current balance:";
    // for (auto& v : balance) {
    //     LOG_DBG1 << "node " << v.first->node()->name << ", mass " << v.second.mass << ", torque " << v.second.torque;
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
    PlacementTree::LinkType const* curr_link = map.tree().root_link();
    PlacementTree::LinkType const* prev_link = nullptr;

    // For asserting purposes, we keep track of the number of loop iterations we do.
    // This can never be more than the tree height (in number of nodes from root to deepest leaf)
    // plus one last iteration for going back towards the root.
    size_t num_iterations = 0;
    auto   depth_vector = node_depth_vector(map.tree());
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
        LOG_DBG1 << "find max at " << curr_link->node()->data.name;

        // Find the direction away from the current node that has the highest torque.
        // At the same time, collect the sum of masses and torques at the node, in order to push
        // them towards the node with highest torque later (so that the next iteration will have
        // values to work on).
        PlacementTree::LinkType* max_link   = nullptr;
        double                   max_torque = -1.0;
        Fulcrum                  sum;

        for (
            auto it_l = curr_link->node()->begin_links();
            it_l != curr_link->node()->end_links();
            ++it_l
        ) {
            // Make sure that we actually have a useable value.
            assert(balance.count(it_l.link()) > 0);

            LOG_DBG2 << "at " <<  it_l.link()->outer()->node()->data.name
                     << " with mass "  << balance[it_l.link()].mass
                     << " and torque "  << balance[it_l.link()].torque;
            if (balance[it_l.link()].torque > max_torque) {
                max_link   = it_l.link();
                max_torque = balance[it_l.link()].torque;
            }
            sum.mass   += balance[it_l.link()].mass;
            sum.torque += balance[it_l.link()].torque;
        }
        assert(max_link);

        // Check if we found the edge where the center of gravity lies. This is the case when the
        // the highest torque is coming from the direction where we came just from in the last
        // iteration.
        LOG_DBG1 << "moving to " << max_link->outer()->node()->data.name;
        if (max_link->outer() == prev_link) {
            LOG_DBG << "found between " << curr_link->node()->data.name << " and " << prev_link->node()->data.name;
            break;
        }

        // If we are not done yet, move down the edge.
        prev_link = max_link;
        curr_link = max_link->outer();

        LOG_DBG1 << "mass sum " << sum.mass << ", torque sum " << sum.torque;

        // Now we are at a node where we have calculated only the masses and torques coming from
        // further down in the tree so far, but not the values coming from the direction of the root
        // (from where we just came). So we need to calculate these now:

        // Subtract the mass and torque of the direction where we found the most torque again,
        // so that all that is left are the sums of all the other (not maximum) nodes. Then push it
        // towards to the end of the edge.
        sum.mass   -= balance[max_link].mass;
        sum.torque -= balance[max_link].torque;
        sum.torque += sum.mass * max_link->edge()->data.branch_length;

        // Add masses of the placements on this edge.
        for( PqueryPlacement const* place : place_map[ max_link->edge()->index() ]) {
            double p_dist = max_link->edge()->data.branch_length - place->proximal_length();
            if (with_pendant_length) {
                p_dist += place->pendant_length();
            }
            sum.mass   += place->like_weight_ratio();
            sum.torque += place->like_weight_ratio() * p_dist;
        }

        // Store the values at the corresponding link.
        balance[curr_link] = sum;
        LOG_DBG1 << "stored mass " << sum.mass << " and torque " << sum.torque << " at " << max_link->outer()->node()->data.name;

        LOG_DBG << "end of iteration " << num_iterations << "\n";
    }

    // Assert that the two links are actually the two ends of the same edge and that their nodes
    // are the correct ones in terms of direction to the root.
    assert(curr_link->edge() == prev_link->edge());
    assert(prev_link->node() == prev_link->edge()->primary_node());
    assert(curr_link->node() == curr_link->edge()->secondary_node());

    // LOG_DBG << "current balance:";
    // for (auto& v : balance) {
    //     LOG_DBG1 << "node " << v.first->node()->name << ", mass " << v.second.mass << ", torque " << v.second.torque;
    // }

    LOG_DBG << "cur  " << curr_link->node()->data.name << " with mass " << balance[curr_link].mass << " and torque " << balance[curr_link].torque;
    LOG_DBG << "prev " << prev_link->node()->data.name << " with mass " << balance[prev_link].mass << " and torque " << balance[prev_link].torque;

    // At this point, we have found the central edge that balances the placement masses on the tree.
    // curr_link is at the downwards (away from the root) end of this edge, while prev_link at its
    // top (towards the root).
    // All that is left now is to find the correct position on this edge. For this, we need to
    // consider the masses and torques that come from both ends of the edge, as well as the
    // placements on the edge itself.

    PlacementTree::EdgeType* central_edge = curr_link->edge();

    // -------------------------------------------------------------------------
    //     Calculate Torques at Central Edge
    // -------------------------------------------------------------------------

    // Define the masses and torques at both ends of the edge: proximal and distal mass/torque.
    // Calculate them as the sums of the values from the subtree that lies behind the edge.
    Fulcrum prox_fulcrum;
    Fulcrum dist_fulcrum;

    assert(balance.count(curr_link) > 0);
    assert(balance.count(prev_link) > 0);

    PlacementTree::LinkType* link;
    link = prev_link->next();
    while (link != prev_link) {
        assert(balance.count(link));
        prox_fulcrum.mass   += balance[link].mass;
        prox_fulcrum.torque += balance[link].torque;
        link = link->next();
    }

    link = curr_link->next();
    while (link != curr_link) {
        assert(balance.count(link));
        dist_fulcrum.mass   += balance[link].mass;
        dist_fulcrum.torque += balance[link].torque;
        link = link->next();
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
    if (prox_fulcrum.mass == 0.0 || dist_fulcrum.mass == 0.0) {
        approx_proximal_length = central_edge->data.branch_length / 2.0;
    } else {
        approx_proximal_length = (dist_fulcrum.torque - prox_fulcrum.torque
                               + (dist_fulcrum.mass * central_edge->data.branch_length))
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
        return lhs->proximal_length() < rhs->proximal_length();
    };
    std::sort(
        place_map[ central_edge->index() ].begin(),
        place_map[ central_edge->index() ].end(),
        sort_by_pos
    );

    // Now add all placements on the edge to the balance variable, sorted by their proximal length.
    for( PqueryPlacement const* place : place_map[ central_edge->index() ]) {
        double place_prox = place->proximal_length();

        // Some sanity checks for wrong data. We do it here because otherwise the algorithm might
        // produce weird results. However, usually this task is up to the validate() method.
        if (place_prox > central_edge->data.branch_length) {
            LOG_WARN << "Placement found that has proximal_length > branch_length.";
            place_prox = central_edge->data.branch_length;
        }
        if (place_prox < 0.0) {
            LOG_WARN << "Placement found that has proximal_length < 0.0.";
            place_prox = 0.0;
        }

        double place_pendant_torque = 0.0;
        if (with_pendant_length) {
            place_pendant_torque = place->like_weight_ratio() * place->pendant_length();
        }

        tqs += place_prox * place->like_weight_ratio();
        mss += place->like_weight_ratio();

        BalancePoint place_balance;
        place_balance.proximal_length = place_prox;
        place_balance.mass            = place->like_weight_ratio();
        place_balance.pendant_torque  = place_pendant_torque;

        edge_balance.push_back(place_balance);
    }

    tqs += dist_fulcrum.torque - prox_fulcrum.torque + (dist_fulcrum.mass * central_edge->data.branch_length);
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
    edge_balance.push_back(BalancePoint(central_edge->data.branch_length));

    LOG_DBG << "edge_balance:";
    for (auto& e : edge_balance) {
        LOG_DBG1 << "at " << e.proximal_length << " with mass " << e.mass << " and pen torque " << e.pendant_torque;
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

        LOG_DBG1 << "at " << curr_point.proximal_length << " with mass " << curr_point.mass << " and pen torque " << curr_point.pendant_torque;

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
    const Sample& map,
    bool                with_pendant_length
) {
    double variance = 0.0;
    double mass     = 0.0;

    auto   cog             = center_of_gravity(map, with_pendant_length);
    auto   central_edge    = cog.first;
    double proximal_length = cog.second;

    LOG_DBG << "edge " << central_edge->primary_node()->data.name << " " << central_edge->secondary_node()->data.name;
    LOG_DBG << "prox " << proximal_length;

    auto   node_dist_prox  = node_distance_vector(map.tree(), central_edge->primary_node());
    auto   node_dist_dist  = node_distance_vector(map.tree(), central_edge->secondary_node());

    for (const auto& pqry : map.pqueries()) {
        for (const auto& place : pqry->placements) {
            double distance;

            if (place->edge().index() == central_edge->index()) {
                distance = std::abs(place->proximal_length() - proximal_length);
            } else{
                double pp, pd, dp;

                // proximal-proximal case
                pp = proximal_length
                   + node_dist_prox[place->edge().primary_node()->index()]
                   + place->proximal_length();

                // proximal-distal case
                pd = proximal_length
                   + node_dist_prox[place->edge().secondary_node()->index()]
                   + place->edge().data.branch_length - place->proximal_length();

                // distal-proximal case
                dp = central_edge->data.branch_length - proximal_length
                   + node_dist_dist[place->edge().primary_node()->index()]
                   + place->proximal_length();

                // find min of the three cases
                distance = std::min(pp, std::min(pd, dp));
            }

            if (with_pendant_length) {
                distance += place->pendant_length();
            }
            variance += distance * distance * place->like_weight_ratio();
            mass     += place->like_weight_ratio();
        }
    }

    return variance / mass;
}

/**
 * @brief
 */
double center_of_gravity_distance (
    const Sample& map_a,
    const Sample& map_b,
    bool                with_pendant_length
) {
    if (!compatible_trees(map_a, map_b)) {
        throw std::invalid_argument("__FUNCTION__: Incompatible trees.");
    }

    // Disable debug messages while code is not in review.
    LOG_SCOPE_LEVEL(utils::Logging::kInfo)

    auto cog_a = center_of_gravity(map_a, with_pendant_length);
    auto cog_b = center_of_gravity(map_b, with_pendant_length);

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
        auto node_dist_a_prox = node_distance_vector(map_a.tree(), edge_a->primary_node());
        auto node_dist_a_dist = node_distance_vector(map_a.tree(), edge_a->secondary_node());

        double pp, pd, dp;

        // proximal-proximal case
        pp = prox_a
           + node_dist_a_prox[edge_b->primary_node()->index()]
           + prox_b;

        // proximal-distal case
        pd = prox_a
           + node_dist_a_prox[edge_b->secondary_node()->index()]
           + edge_b->data.branch_length - prox_b;

        // distal-proximal case
        dp = edge_a->data.branch_length - prox_a
           + node_dist_a_dist[edge_b->primary_node()->index()]
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
 * This method calculates the distance between two maps as the normalized sum of the distances
 * between all pairs of pqueries in the map. It is similar to the variance() calculation, which
 * calculates this sum for the squared distances between all pqueries of one map.
 *
 * @param  map_a               The first Sample to which the distances shall be calculated to.
 * @param  map_b               The second Sample to which the distances shall be calculated to.
 * @param  with_pendant_length Whether or not to include all pendant lengths in the calculation.
 *
 * @return                         Distance value.
 */
double pairwise_distance (
    const Sample& map_a,
    const Sample& map_b,
    bool                with_pendant_length
) {
    if (!compatible_trees(map_a, map_b)) {
        throw std::invalid_argument("__FUNCTION__: Incompatible trees.");
    }

    // Init.
    double sum = 0.0;

    // Create PqueryPlain objects for every placement and copy all interesting data into it.
    // This way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. this gives a tremendous speedup!
    std::vector<PqueryPlain> pqueries_a = plain_queries( map_a );
    std::vector<PqueryPlain> pqueries_b = plain_queries( map_b );

    // Calculate a matrix containing the pairwise distance between all nodes. This way, we
    // do not need to search a path between placements every time. We use the tree of the first map
    // here, ignoring branch lengths on tree b.
    // FIXME this might be made better by using average or so in the future.
    auto node_distances = node_distance_matrix(map_a.tree());

    for (const PqueryPlain& pqry_a : pqueries_a) {
        for (const PqueryPlain& pqry_b : pqueries_b) {
            sum += pquery_distance(pqry_a, pqry_b, node_distances, with_pendant_length);
        }
    }

    // Return normalized value.
    return sum / map_a.placement_mass() / map_b.placement_mass();
}

// =================================================================================================
//     Node-Based Distances
// =================================================================================================

utils::Histogram node_distance_histogram (
    // const Sample& map,
    const PlacementTreeNode& node,
    bool                with_pendant_length
) {
    (void) node;
    (void) with_pendant_length;
    throw std::domain_error("Not yet implemented.");
    return utils::Histogram(1);
}

std::vector< utils::Histogram > node_distance_histograms (
    const Sample& map,
    bool                with_pendant_length
) {
    auto vec = std::vector< utils::Histogram >();
    vec.reserve(map.tree().node_count());

    // TODO ensure that the node iterator gives the nodes in order of their index!

    for (auto it = map.tree().begin_nodes(); it != map.tree().end_nodes(); ++it) {
        vec.push_back(node_distance_histogram(**it, with_pendant_length));
    }

    return vec;
}

double node_histogram_distance (
    const Sample& map_a,
    const Sample& map_b,
    bool                with_pendant_length
) {
    if( !compatible_trees( map_a, map_b ) ) {
        throw std::invalid_argument( "Incompatible trees." );
    }

    // Get the histograms describing the distances from placements to all nodes.
    auto vec_a = node_distance_histograms( map_a, with_pendant_length );
    auto vec_b = node_distance_histograms( map_b, with_pendant_length );

    // If the trees are compatible (as ensured in the beginning of this function), they need to have
    // the same number of nodes. Thus, also there should be this number of histograms in the vectors.
    assert( vec_a.size() == map_a.tree().node_count() );
    assert( vec_b.size() == map_b.tree().node_count() );
    assert( vec_a.size() == vec_b.size() );

    double dist = 0.0;
    for( size_t i = 0; i < vec_a.size(); ++i ) {
        dist += earth_movers_distance( vec_a[i], vec_b[i], true );
    }

    assert( dist >= 0.0 );
    return dist;
}

/*
Matrix<double> node_histogram_distance_matrix (
    const SampleSet& maps,
    bool                   with_pendant_length
) {
    auto const maps_count = maps.size();
    auto mat = Matrix<double> (maps_count, maps_count);

    // TODO do not write this function. instead, create a generic function that takes a placement
    // map set and a functor to some distance function. also, put it into some map set file
    // instead of the general map file.

    (void) with_pendant_length;
    throw std::domain_error("Not yet implemented.");

    return mat;
}
*/

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
    const Sample& map,
    bool                with_pendant_length
) {
    // Init.
    double variance = 0.0;

    // Create PqueryPlain objects for every placement and copy all interesting data into it.
    // This way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. This gives a tremendous speedup!
    std::vector<PqueryPlain> vd_pqueries = plain_queries( map );

    // Also, calculate a matrix containing the pairwise distance between all nodes. this way, we
    // do not need to search a path between placements every time.
    auto node_distances = node_distance_matrix(map.tree());

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
