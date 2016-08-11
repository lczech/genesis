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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "tree/function/emd.hpp"

#include "tree/function/emd_tree.hpp"
#include "tree/function/operators.hpp"
#include "tree/iterator/postorder.hpp"
#include "tree/tree.hpp"

#include "utils/core/logging.hpp"

#include <assert.h>
#include <cmath>
#include <unordered_map>

namespace genesis {
namespace tree {

// =================================================================================================
//     EMD Functions
// =================================================================================================

/**
 * @brief Calculate the earth mover's distance of two distributions of masses on a given tree.
 *
 * The earth mover's distance is typically a distance measure between two distributions.
 * See https://en.wikipedia.org/wiki/Earth_mover's_distance for an introduction.
 *
 * In our case, we use distibutions of masses along the branches of a tree. Each branch can have
 * multiple masses at different positions within [0.0, branch_length].
 *
 * The distance is calculated as the amount of work needed to move the masses of one distribution
 * so that they end up in the positions of the masses of the other distribution.
 * Work is here defined as mass times dislocation. Thus, the work ( = total distance ) is higher
 * if either more mass has to be moved, or mass has to be moved further.
 *
 * The resulting distance is independed of the rooting of the tree and commutative with respect
 * to the two mass distributions.
 *
 * In order to keep the calculations simple, we use the following convention for the two
 * distributions: The masses of one distribution are stored using a positive sign, the masses of the
 * other distribution use a negative sign. This way, only one storage for the masses can be used
 * and the algorithm is simplyfied.
 *
 * The earth mover's distance is only meaningful if both mass distributions contain the same amount
 * of total mass. Thus, as they use opposite signs, the sum of all masses on the tree should ideally
 * be zero (apart from numerical derivations).
 * See @link sum_of_masses( EmdTree const& tree ) sum_of_masses() @endlink and
 * @link validate_emd_tree( EmdTree const& tree, double valid_total_mass_difference )
 * validate_emd_tree() @endlink for functions to verify this.
 *
 * See @link placement::earth_movers_distance( const Sample& lhs, const Sample& rhs, bool with_pendant_length )
 * earth_movers_distance( Sample const&, ... ) @endlink for an exemplary
 * usage of this function, which applies the earth mover's distance to the placement weights
 * (@link placement::PqueryPlacement::like_weight_ratio like_weight_ratio@endlink) of a
 * PlacementTree.
 */
double earth_movers_distance( EmdTree const& tree )
{
    // Keep track of the total resulting work (the distance we moved the masses).
    // This is the result returned in the end.
    double work = 0.0;

    // Store a list of masses for each processed node. It maps from node indices to the total
    // mass that comes from the subtree below that node. Thus, for the root node, it should be
    // the same value as sum_of_masses(). Both values should be close to zero (except for numerical
    // issues), in order for the result of this function to be meaningful.
    std::unordered_map<size_t, double> node_masses;

    // Init the mass to 0 for all leaf nodes. At leaves, we do not have any mass coming from further
    // down the tree (obviously, because they are leaves).
    for( auto const& node : tree.nodes() ) {
        if( node->is_leaf() ) {
            node_masses[ node->index() ] = 0.0;
        }
    }

    // Do a postorder traversal of the tree, starting at the root.
    // In theory, it does not matter where we start the traversal - however, the positions of the
    // masses are given as "proximal_length" on their branch, which always points away from the
    // root. Thus, if we decided to traverse from a different node than the root, we would have to
    // take this into account. So, we do start at the root, to keep it simple.
    for( auto tree_it : postorder(tree) ) {

        // If we are at the last iteration, we reached the root. Thus, we have moved all masses
        // and don't need to proceed. If we did, we would count an edge of the root again
        // (because the iterator traverses nodes, not edges, so the root node itself is traversed,
        // although it has no proper edge that we would need to process).
        if( tree_it.is_last_iteration() ) {
            continue;
        }

        // Some shorthands.
        const size_t pri_node_index = tree_it.edge().primary_node().index();
        const size_t sec_node_index = tree_it.edge().secondary_node().index();

        // The iterator should guarantee that its edge is always the one pointing towards the root.
        // Still, better check this!
        assert( sec_node_index == tree_it.node().index() );

        // Make sure that we actually processes the subtree of the current edge.
        assert( node_masses.count( sec_node_index ) > 0 );

        // We now start a "normal" earth movers distance caluclation along the current edge.
        // We start at the end of the branch, with the mass that comes from the subtree below it...
        double current_pos  = tree_it.edge().data<EmdEdgeData>().branch_length;
        double current_mass = node_masses[ sec_node_index ];

        // ... and move the mass along the branch, balancing it with the masses found on the branch.
        // We use a reverse iterator in order to traverse the branch from end to start.
        auto const& edge_masses = tree_it.edge().data<EmdEdgeData>().masses;
        for(
            auto mass_rit = edge_masses.crbegin();
            mass_rit != edge_masses.crend();
            ++mass_rit
        ) {
            // The work is accumulated: The mass that we are currently moving times the distances
            // that we move it.
            work += std::abs( current_mass ) * ( current_pos - mass_rit->first );

            // Update the current position and mass.
            current_pos   = mass_rit->first;
            current_mass += mass_rit->second;
        }

        // After we finished moving along the branch, we need extra work to move the remaining mass
        // to the node at the top end of the branch. Also, add the remaining mass to this node, so
        // that it is available for when we process the upper part of that node (towards the root).
        work                          += std::abs( current_mass ) * current_pos;
        node_masses[ pri_node_index ] += current_mass;
    }

    return work;
}

/**
 * @brief Set all branch lengths of the Tree to 1.0, while keeping the relative position of all
 * masses on the branches.
 */
void transform_to_unit_branch_lengths( EmdTree& tree )
{
    for( auto& edge : tree.edges() ) {
        auto& edge_data = edge->data<EmdEdgeData>();
        std::map<double, double> relative;

        for( auto mass : edge_data.masses ) {
            relative[ mass.first / edge_data.branch_length ] += mass.second;
        }

        edge_data.masses = relative;
        edge_data.branch_length = 1.0;
    }
}

/**
 * @brief Accumulate all masses of the Tree on the centers of their edges. Return the work
 * (mass times distance) that was needed to move the masses to the centers.
 */
double center_masses_on_branches( EmdTree& tree )
{
    double work = 0.0;
    for( auto& edge : tree.edges() ) {
        auto& edge_data = edge->data<EmdEdgeData>();

        double branch_center = edge_data.branch_length / 2;
        double central_mass  = 0.0;

        for( auto mass : edge_data.masses ) {
            work         += mass.second * std::abs( branch_center - mass.first );
            central_mass += mass.second;
        }

        edge_data.masses.clear();
        edge_data.masses[ branch_center ] = central_mass;
    }
    return work;
}

/**
 * @brief Return the total sum of all masses on the Tree.
 *
 * In order for the earth_movers_distance() algorithm to work properly (and give meaningful
 * results), the total mass on the tree should ideally be 0.0. This function can be used to check
 * this.
 *
 * Because of numerical issues however, be aware that the result might be slighly off zero. This
 * is okay, as it usually is in the last digits of the double.
 */
double sum_of_masses( EmdTree const& tree )
{
    double total_mass = 0.0;
    for( auto& edge : tree.edges() ) {
        for( auto mass : edge->data<EmdEdgeData>().masses ) {
            total_mass += mass.second;
        }
    }
    return total_mass;
}

/**
 * @brief Validate the data on an Tree.
 *
 * This function returns true iff the data on the Tree is valid:
 *
 *  *  The node and edge data types have to be EmdNodeData and EmdEdgeData, respectively.
 *  *  The positions of the masses are in [0.0, branch_length] on their respective branches.
 *  *  The sum of all masses is close to 0.0, using the optional arument
 *     `valid_total_mass_difference` as a measure of closeness.
 *
 * The function stops at the first encountered invalid condition and outputs a description message
 * of the invalid value to LOG_INFO.
 *
 * @param tree                        Tree to be validated.
 * @param valid_total_mass_difference (= 0.00001 by default) allowed difference from zero for the
 *                                    total sum of all masses on the tree.
 */
bool validate_emd_tree( EmdTree const& tree, double valid_total_mass_difference )
{
    // Check tree.
    if( ! validate_topology( tree ) ) {
        LOG_INFO << "Invalid EMD tree topology.";
        return false;
    }
    if( ! tree_data_is< EmdNodeData, EmdEdgeData >( tree )) {
        LOG_INFO << "Tree does not only contain EMD Node and Edge data types.";
        return false;
    }

    // Check masses.
    double mass_sum = 0.0;
    for( auto const& edge : tree.edges() ) {
        auto const edge_data = dynamic_cast< EmdEdgeData const* >( edge->data_ptr() );
        if( edge_data == nullptr ) {
            LOG_INFO << "Edge data type is not 'EmdEdgeData'.";
            return false;
        }

        for( auto const& mass : edge_data->masses ) {
            if( mass.first < 0.0 ) {
                LOG_INFO << "Mass with branch position < 0.0";
                return false;
            }
            if( mass.first > edge_data->branch_length ) {
                LOG_INFO << "Mass with branch position > branch_length";
                return false;
            }

            mass_sum += mass.second;
        }
    }

    if( std::abs(mass_sum) > valid_total_mass_difference ) {
        LOG_INFO << "Total mass difference " << mass_sum
                 << " is higher than " << valid_total_mass_difference;
        return false;
    }
    return true;
}

} // namespace tree
} // namespace genesis
