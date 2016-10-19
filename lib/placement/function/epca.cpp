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

#include "placement/function/epca.hpp"

#include "placement/function/helper.hpp"
#include "placement/function/sample_set.hpp"

#include "tree/iterator/postorder.hpp"

#include "utils/math/matrix.hpp"
#include "utils/math/matrix/pca.hpp"
#include "utils/math/matrix/operators.hpp"

// testing
#include "utils/core/logging.hpp"
#include "utils/core/fs.hpp"
#include "utils/text/string.hpp"


#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Edge PCA
// =================================================================================================

std::vector<double> epca_imbalance_vector( Sample const& smp )
{
    // Result vector: imbalance of masses at each edge of the tree.
    auto vec = std::vector<double>( smp.tree().edge_count(), 0.0 );

    // We need the masses per edge, and their sum, for later.
    auto const masses   = placement_weight_per_edge( smp );
    auto const mass_sum = std::accumulate( masses.begin(), masses.end(), 0.0 );

    // Collect the placement masses at each link of the tree.
    // Use init to -1 as indicator for assertions.
    auto link_masses = std::vector<double>( smp.tree().link_count(), -1.0 );

    for( auto tree_it : postorder( smp.tree() )) {

        // Skip the last iteration. We are interested in edges, not in nodes.
        if( tree_it.is_last_iteration() ) {
            continue;
        }

        // Get the indices of the links at both sides of the current edge.
        // cur_idx is the link that points away from the root,
        // out_idx is the link that points towards it (i.e., its subtree contains the root).
        auto const cur_idx = tree_it.link().index();
        auto const out_idx = tree_it.link().outer().index();

        // Assert that we have not processed those links before.
        assert( link_masses[ cur_idx ] < 0.0 );
        assert( link_masses[ out_idx ] < 0.0 );

        // Assert that the cur_idx belongs to the link away from the root.
        // This is the case if the primary link of its node is the link itself,
        // because the node uses this link to point towards the root - thus, the link itself
        // is away from the root, while the out_idx link lies towards it.
        assert( smp.tree().link_at( cur_idx ).node().primary_link().index() == cur_idx );

        // Some more ways to do the same assertion, just to be sure.
        assert( tree_it.edge().index() == smp.tree().link_at( cur_idx ).edge().index() );
        assert( tree_it.edge().primary_link().index() == out_idx );
        assert( tree_it.edge().secondary_link().index() == cur_idx );

        // Leaf links have no mass.
        if( tree_it.link().is_leaf() ) {
            link_masses[ cur_idx ] = 0.0;

        // If the link belongs to an inner node, we calculate its mass as the sum of the masses
        // of the other links of this node. Those have already been processed, as we are doing
        // postorder traversal.
        } else {

            // Collect the mass.
            double round_sum = 0.0;

            // Iterate around all other links of the node that belongs to the cur_idx link.
            auto round_link = &tree_it.link().next();
            while( round_link != &tree_it.link() ) {

                // We are doing postorder traversal, so we should have seen this link before.
                assert( link_masses[ round_link->index() ] >= 0.0 );

                // The mass of the subtree behind this link can be calculated from the total mass
                // minus the mass of the link itself.
                round_sum += mass_sum - link_masses[ round_link->index() ];

                // Next link of the node.
                round_link = &round_link->next();
            }

            link_masses[ cur_idx ] = round_sum;
        }

        // Calculate the mass at the other side of the edge.
        link_masses[out_idx] = mass_sum - link_masses[cur_idx] - masses[tree_it.edge().index()];

        // Finally, calculate the imbalance of the current edge,
        // normalized by the total mass on the tree.
        vec[ tree_it.edge().index() ] = ( link_masses[out_idx] - link_masses[cur_idx] ) / mass_sum;
    }

    return vec;
}

void epca( SampleSet const& samples )
{
    if( samples.size() == 0 ) {
        throw std::runtime_error(
            "Cannot calculate Edge PCA on an empty SampleSet."
        );
    }
    if( ! all_identical_trees( samples )) {
        throw std::runtime_error(
            "Cannot calculate Edge PCA on trees that have a different topology."
        );
    }

    LOG_DBG << "calculating imbalance_matrix";

    assert( samples.size() > 0 );
    auto const edge_count = samples.at( 0 ).sample.tree().edge_count();
    (void) edge_count;

    // Get the indices of all edges that do not lead to a tip.
    std::vector<size_t> inner_edge_indices;
    for( auto const& edge_it : samples.at( 0 ).sample.tree().edges() ) {
        if( edge_it->secondary_node().is_inner() ) {
            inner_edge_indices.push_back( edge_it->index() );
        }
    }

    LOG_DBG << "inner_edge_indices " << inner_edge_indices.size();

    auto imbalance_matrix = utils::Matrix<double>( samples.size(), inner_edge_indices.size() );
    for( size_t s = 0; s < samples.size(); ++s ) {
        auto const& smp = samples[s].sample;
        auto imbalance_vec = epca_imbalance_vector( smp );

        // Copy the imbalance vector.
        // TODO optimize.
        // for( size_t e = 0; e < edge_count; ++e ) {
        //     imbalance_matrix( s, e ) = imbalance_vec[ e ];
        // }

        // We need to have the right number of imbalance values, which also needs to be
        // smaller than the number of inner edges (there can be no tree with just inner
        // edges, thus the total number of edges has to be bigger).
        assert( imbalance_vec.size() == edge_count );
        assert( imbalance_vec.size() >  inner_edge_indices.size() );

        // Copy those imbalance values to the matrix that belong to inner edges.
        for( size_t i = 0; i < inner_edge_indices.size(); ++i ) {
            auto idx = inner_edge_indices[i];
            imbalance_matrix( s, i ) = imbalance_vec[ idx ];
        }
    }


    // printf("\nimbalance_matrix:\n");
    // for( size_t r = 0; r < imbalance_matrix.rows(); ++r ) {
    //     for( size_t c = 0; c < imbalance_matrix.cols(); ++c ) {
    //         printf("%12.4f", imbalance_matrix(r,c));
    //     }
    //     printf("\n");
    // }

    // auto t_imbalance_matrix = utils::transpose(imbalance_matrix);
    // utils::file_write( utils::to_string( imbalance_matrix ), "/home/lucas/tmp/bv_epca/imbalance.mat.csv" );

    LOG_DBG << "running pca";

    auto pca = utils::principal_component_analysis( imbalance_matrix, 3, utils::PcaStandardization::kCovariance );
    // auto pca = utils::principal_component_analysis( imbalance_matrix, 5 );

    printf("\nEigenvalues:\n");
    for( auto val : pca.eigenvalues ) {
        printf("%18.5f\n", val );
    }

    printf("\nEigenvectors:\n");
    for( size_t r = 0; r < 20; ++r ) {
    // for( size_t r = 0; r < pca.eigenvectors.rows(); ++r ) {
        for( size_t c = 0; c < pca.eigenvectors.cols(); ++c ) {
            printf("%12.4f", pca.eigenvectors(r,c));
        }
        printf("\n");
    }
    printf("... %u rows in total\n", static_cast<unsigned int>(pca.eigenvectors.rows()));

    printf("\nProjections of row-points on first 3 prin. comps.:\n");
    for (size_t i = 0; i < 10; i++) {
    // for (size_t i = 0; i < pca.projection.rows(); i++) {
        for (size_t j = 0; j < pca.projection.cols(); j++)  {
            printf("%12.4f", pca.projection(i,j));
        }
        printf("\n");
    }
    printf("... %u rows in total\n", static_cast<unsigned int>(pca.projection.rows()));

    // utils::file_write( utils::to_string( pca.projection ), "/home/lucas/tmp/bv_epca/my.proj" );

}

} // namespace placement
} // namespace genesis
