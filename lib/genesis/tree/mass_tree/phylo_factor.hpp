#ifndef GENESIS_TREE_MASS_TREE_PHYLO_FACTOR_H_
#define GENESIS_TREE_MASS_TREE_PHYLO_FACTOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @ingroup tree
 */

#include "genesis/tree/mass_tree/balances.hpp"
#include "genesis/utils/containers/matrix.hpp"

#include <functional>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Subtree;
class Tree;
using MassTree = Tree;

// =================================================================================================
//     Phylogenetic Factorization
// =================================================================================================

/**
 * @brief A single phylogenetic factor.
 *
 * @see phylogenetic_factorization()
 */
struct PhyloFactor
{
    /**
     * @brief The edge that this factor found to be maximizing for the objective function.
     */
    size_t                     edge_index;

    /**
     * @brief The set of edges on the root (primary) side of the edge that belongs to this factor.
     */
    std::unordered_set<size_t> edge_indices_primary;

    /**
     * @brief The set of edges on the non-root (secondary) side of the edge that belongs to this factor.
     */
    std::unordered_set<size_t> edge_indices_secondary;

    /**
     * @brief The balances for all Samples calculated on the two sets of edge indices of this factor.
     */
    std::vector<double>        balances;

    /**
     * @brief The objective value obtained from the objective function using the balances.
     */
    double                     objective_value;

    /**
     * @brief For reference, all other objective values for the other edges of the tree.
     *
     * This is mainly used for visualization purposes in order to validate why a particular edge
     * has been chosen as the next factor. This shows the objective value for all edges that
     * have not yet been factored out by previous iterations.
     *
     * The vector is indexed by edge indices as usual. The edges of previous factors as well as
     * leaf edges have a `NaN` value to distinguish them from proper values.
     */
    std::vector<double>        all_objective_values;
};

/**
 * @brief Helper function for phylogenetic_factorization() to find the constrained subtrees
 * that are split by an edge.
 *
 * Helper function to get the edge indices of a particular subtree,
 * excluding the edge that leads to it, and excluding all subtrees that are not connected
 * to the given subtree via the candidate edges. In other words, a subtree is excluded
 * if it is connected to the given subtree by an edge that is not in the candidate list.
 * Consequently, the returned indices are all part of the candidates.
 */
std::unordered_set<size_t> phylo_factor_subtree_indices(
    Subtree const& subtree,
    std::unordered_set<size_t> const& candidate_edges
);

/**
 * @brief Helper function for phylogenetic_factorization() that tries all candidate edges
 * to find the one that maximizes the objective function.
 */
PhyloFactor phylo_factor_find_best_edge(
    BalanceData const& data,
    std::unordered_set<size_t> const& candidate_edges,
    std::function<double( std::vector<double> const& balances )> objective
);

/**
 * @brief Calculate the Phylogenetic Factorization (PhyloFactor) of a set of MassTree%s.
 *
 * This implementation is simiar to the ideas presented in [1]. We however extend this original
 * idea by being able to place masses on inner branches as well, instead of just the tips (OTUs).
 *
 * The function expects @p data coming from mass_balance_data(), and an @p objective function
 * that needs to be maximized for finding the next best (greedy) phylo factor. The input to this
 * objective function are the balances for all input @p data points for the current edge being
 * considered as a factor during the execution of the greedy algorithm.
 *
 * Furthermore, the number of iterations can be set via @p max_iterations, that is, the number of
 * phylo factors to find. By default, all possible are found, which might take too long.
 * Currently, we do not have a stopping criterion implemented, so it is up to the user to set a
 * reasonable value here.
 *
 * Lastly, a functional for logging the progress can be set, which needs to take the current and
 * the maximal iteration counter (1-based) and can produce some logging for this:
 *
 *     []( size_t iteration, size_t max_iterations ){
 *         LOG_DBG1 << "iteration " << iteration << " of " << max_iterations;
 *     }
 *
 * More details on the method can be found in
 *
 * > [1] A. D. Washburne, J. D. Silverman, J. W. Leff, D. J. Bennett, J. L. Darcy, S. Mukherjee, N. Fierer, and L. A. David,
 * > "Phylogenetic factorization of compositional data yields lineage-level associations in microbiome datasets,"
 * > PeerJ, vol. 5, p. e2969, Feb. 2017.
 * > https://doi.org/10.7717/peerj.2969
 *
 * @see mass_balance()
 * @see phylogenetic_ilr_transform()
 */
std::vector<PhyloFactor> phylogenetic_factorization(
    BalanceData const& data,
    std::function<double( std::vector<double> const& balances )> objective,
    size_t max_iterations = 0,
    std::function<void( size_t iteration, size_t max_iterations )> log_progress = {}
);

} // namespace tree
} // namespace genesis

#endif // include guard
