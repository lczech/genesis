/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief Implementation of Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/function/distances.hpp"

#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/tree/iterator/levelorder.hpp"

#include "genesis/utils/core/logging.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Distance Measures
// =================================================================================================

utils::Matrix<size_t> node_path_length_matrix(
    Tree const& tree
) {
    auto max_val = std::numeric_limits<size_t>::max();
    utils::Matrix<size_t> mat( tree.node_count(), tree.node_count(), max_val );

    // Fill every row of the matrix.
    #pragma omp parallel for
    for( size_t ni = 0; ni < tree.node_count(); ++ni ) {
        auto const& row_node = tree.node_at( ni );
        assert( row_node.index() == ni );

        // Set the diagonal element of the matrix.
        mat( row_node.index(), row_node.index() ) = 0;

        // The columns are filled using a levelorder traversal. This makes sure that for every node
        // we know how to calculate the distance to the current row node.
        // Unfortunately, this prevents us from simply calculating the upper triangle of the matrix
        // and copying it (distance is symmetric), because we do not really know which nodes are in
        // which half during a levelorder traversal...
        for( auto it : levelorder( row_node.link() )) {
            // Skip the diagonal of the matrix.
            if (it.is_first_iteration()) {
                assert( it.node().index() == row_node.index() );
                continue;
            }

            // Make sure we don't have touched the current position, but have calculated
            // the needed dependency already.
            assert( mat(row_node.index(), it.node().index()) == max_val );
            assert( mat(row_node.index(), it.link().outer().node().index()) != max_val );

            // The distance to the current row node is one more than the distance from the other
            // end of that branch to the row node.
            mat( row_node.index(), it.node().index() )
                = 1 + mat(row_node.index(), it.link().outer().node().index());
        }
    }

    return mat;
}

std::vector<size_t> node_path_length_vector(
    Tree const& tree,
    TreeNode const& node
) {
    if( ! belongs_to( tree, node )) {
        throw std::runtime_error(
            "Cannot caluclate node_path_length_vector, as the given Node does not belong to the Tree."
        );
    }
    auto max_val = std::numeric_limits<size_t>::max();

    // store the distance from each node to the given node.
    std::vector<size_t> vec;
    vec.resize(tree.node_count(), max_val);
    vec[ node.index() ] = 0;

    // calculate the distance vector via levelorder iteration.
    for( auto it : levelorder( node ) ) {
        // skip the starting node (it is already set to 0).
        if (it.is_first_iteration()) {
            continue;
        }

        // we do not have the distance of the current node, but the one of its "parent" (the one in
        // direction of the starting node)!
        assert( vec[it.node().index()] == max_val );
        assert( vec[it.link().outer().node().index()] != max_val );

        // the distance is the distance from the "parent" node (the next one in direction towards
        // the given node) plus 1.
        vec[it.node().index()] = 1 + vec[it.link().outer().node().index()];
    }

    return vec;
}

std::vector<size_t> node_path_length_vector(
    Tree const& tree
) {
    return node_path_length_vector( tree, tree.root_node() );
}

// =================================================================================================
//     Edge Distance Measures
// =================================================================================================

utils::Matrix<size_t> edge_path_length_matrix(
    Tree const& tree
) {
    // Result matrix that will be returned.
    utils::Matrix<size_t> mat (tree.edge_count(), tree.edge_count());

    // For calculating the distance between edges, we use the distances between nodes and for every
    // pair of edged find the nodes at the ends of the edges that are closest to each other. This
    // is then the shortest distance between the two edges.
    auto node_depth_mat = node_path_length_matrix(tree);

    #pragma omp parallel for
    for( size_t ei = 0; ei < tree.edge_count(); ++ei ) {
        auto const& row_edge = tree.edge_at( ei );
        assert( row_edge.index() == ei );

        for( auto const& col_edge : tree.edges() ) {

            // Set the diagonal element of the matrix. We don't need to compare nodes in this case.
            if (row_edge.index() == col_edge->index()) {
                mat(row_edge.index(), row_edge.index()) = 0;
                continue;
            }

            // primary-primary case
            auto pp = node_depth_mat(
                row_edge.primary_node().index(),
                col_edge->primary_node().index()
            );

            // primary-secondary case
            auto ps = node_depth_mat(
                row_edge.primary_node().index(),
                col_edge->secondary_node().index()
            );

            // secondary-primary case
            auto sp = node_depth_mat(
                row_edge.secondary_node().index(),
                col_edge->primary_node().index()
            );

            // Find min. Make sure that the fourth case "secondary-secondary" is not shorter
            // (if this ever happens, the tree is broken).
            auto dist = std::min({ pp, ps, sp });
            assert( dist <= node_depth_mat(
                row_edge.secondary_node().index(),
                col_edge->secondary_node().index()
            ));

            // Store in matrix.
            mat( row_edge.index(), col_edge->index() ) = dist + 1;
        }
    }

    return mat;
}

std::vector<size_t> edge_path_length_vector(
    Tree const& tree,
    TreeEdge const& edge
) {
    if( ! belongs_to( tree, edge )) {
        throw std::runtime_error(
            "Cannot caluclate node_path_length_vector, as the given Edge does not belong to the Tree."
        );
    }

    auto max_val = std::numeric_limits<size_t>::max();
    std::vector<size_t> vec( tree.edge_count(), max_val );

    // We just need two rows of the distance matrix - let's take the vectors instead for speed.
    auto p_node_dist = node_path_length_vector(tree, edge.primary_node());
    auto s_node_dist = node_path_length_vector(tree, edge.secondary_node());

    for( auto const& col_edge : tree.edges() ) {

        if( edge.index() == col_edge->index() ) {
            vec[ edge.index() ] = 0;
            continue;
        }

        // primary-primary case
        double pp = p_node_dist[ col_edge->primary_node().index() ];

        // primary-secondary case
        double ps = p_node_dist[ col_edge->secondary_node().index() ];

        // secondary-primary case
        double sp = s_node_dist[ col_edge->primary_node().index() ];

        // Find min. Make sure that the fourth case "secondary-secondary" is not shorter
        // (if this ever happens, the tree is broken).
        double dist = std::min({ pp, ps, sp });
        assert(dist <= s_node_dist[ col_edge->secondary_node().index() ]);

        // Store in vector.
        vec[ col_edge->index() ] = dist + 1;
    }

    return vec;
}

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

std::vector< std::pair< TreeNode const*, size_t >> closest_leaf_depth_vector (
    const Tree& tree
) {
    // prepare a result vector with the size of number of nodes.
    std::vector< std::pair< TreeNode const*, size_t >> vec;
    vec.resize(tree.node_count(), {nullptr, 0});

    // fill the vector for every node.
    // this could be speed up by doing a postorder traversal followed by some sort of inside-out
    // traversal (preorder might do the job). but for now, this simple O(n^2) version works, too.
    for (auto node_it = tree.begin_nodes(); node_it != tree.end_nodes(); ++node_it) {
        auto node = node_it->get();

        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->index()].first == nullptr);

        // look for closest leaf node by doing a levelorder traversal.
        for( auto it : levelorder( *node ) ) {
            // if we find a leaf, leave the loop.
            if (it.node().is_leaf()) {
                vec[node->index()].first  = &it.node();
                vec[node->index()].second =  it.depth();
                break;
            }
        }
    }

    return vec;
}

} // namespace tree
} // namespace genesis
