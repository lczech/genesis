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
        double current_pos  = tree_it.edge().data.branch_length;
        double current_mass = node_masses[ sec_node_index ];

        // ... and move the mass along the branch, balancing it with the masses found on the branch.
        // We use a reverse iterator in order to traverse the branch from end to start.
        auto const& edge_masses = tree_it.edge().data.masses;
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
 * @brief Set all branch lengths of the EmdTree to 1.0, while keeping the relative position of all
 * masses on the branches.
 */
void transform_to_unit_branch_lengths( EmdTree& tree )
{
    for( auto& edge : tree.edges() ) {
        std::map<double, double> relative;
        for( auto mass : edge->data.masses ) {
            relative[ mass.first / edge->data.branch_length ] += mass.second;
        }
        edge->data.masses = relative;

        edge->data.branch_length = 1.0;
    }
}

/**
 * @brief Accumulate all masses of the EmdTree on the centers of their edges. Return the work
 * (mass times distance) that was needed to move the masses to the centers.
 */
double center_masses_on_branches( EmdTree& tree )
{
    double work = 0.0;
    for( auto& edge : tree.edges() ) {
        double branch_center = edge->data.branch_length / 2;
        double central_mass  = 0.0;

        for( auto mass : edge->data.masses ) {
            work         += mass.second * std::abs( branch_center - mass.first );
            central_mass += mass.second;
        }

        edge->data.masses.clear();
        edge->data.masses[ branch_center ] = central_mass;
    }
    return work;
}

/**
 * @brief Return the total sum of all masses on the EmdTree.
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
        for( auto mass : edge->data.masses ) {
            total_mass += mass.second;
        }
    }
    return total_mass;
}

/**
 * @brief Validate the data on an EmdTree.
 *
 * This function returns true iff the data on the EmdTree is valid:
 *
 *  *  The positions of the masses are in [0.0, branch_length] on their respective branches.
 *  *  The sum of all masses is close to 0.0, using the optional arument
 *     `valid_total_mass_difference` as a measure of closeness.
 *
 * The function stops at the first encountered invalid condition and outputs a message to
 * LOG_INFO.
 */
bool validate( EmdTree const& tree, double valid_total_mass_difference )
{
    double mass_sum = 0.0;

    for( auto const& edge : tree.edges() ) {
        for( auto const& mass : edge->data.masses ) {
            if( mass.first < 0.0 ) {
                LOG_INFO << "Mass with branch position < 0.0";
                return false;
            }
            if( mass.first > edge->data.branch_length ) {
                LOG_INFO << "Mass with branch position > branch_length";
                return false;
            }

            mass_sum += mass.second;
        }
    }

    if( mass_sum > valid_total_mass_difference ) {
        LOG_INFO << "Total mass difference " << mass_sum
                 << " is higher than " << valid_total_mass_difference;
        return false;
    }
    return true;
}

} // namespace tree
} // namespace genesis
