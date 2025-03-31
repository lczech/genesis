#ifndef GENESIS_UTILS_MATH_HAC_FUNCTIONS_H_
#define GENESIS_UTILS_MATH_HAC_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/io/output_target.hpp"
#include "genesis/utils/math/hac.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// ================================================================================================
//     HAC Analysis and Output Functions
// ================================================================================================

/**
 * @brief Construct a distance matrix between all clusters in a hierarchical agglomerative
 * clustering run.
 *
 * In the HierarchicalAgglomerativeClustering itself, we store the distances between clusters
 * as vectors per cluster, instead of a combined matrix. This saves memory due to the symmetry,
 * and makes merging easier, as the rows of the matrix are independent.
 *
 * However, in order to analyze a clutering afterwards, it is convenient to have all pairwise
 * distances as a matrix instead. This function creates such a matrix by copying the distances
 * of all HierarchicalAgglomerativeClustering::Cluster::distances.
 *
 * Note that not all values in the matrix will be filled. During the clustering, we only compute
 * distances between clusters that are still active. Hence, once a cluster has been merged into
 * some new cluster in the process, all remaining distances to clusters that are created from
 * later mergers are not computed, and hence will be zero in the matrix.
 *
 * @see HierarchicalAgglomerativeClustering
 */
template<typename T>
Matrix<double> hac_distance_matrix(
    HierarchicalAgglomerativeClustering<T> const& clustering
) {
    auto const& clusters = clustering.clusters();
    auto const num_elem = clusters.size();
    auto result = Matrix<double>( num_elem, num_elem, 0.0 );
    for( size_t r = 0; r < num_elem; ++r ) {
        for( size_t c = 0; c < r; ++c ) {
            assert( clusters[r].distances.size() == r );
            auto const val = clusters[r].distances[c];
            result( r, c ) = val;
            result( c, r ) = val;
        }
    }
    return result;
}

/**
 * @brief Write a table summarizing the clusters in a hierarchical agglomerative
 * clustering run.
 *
 * This writes a table to the given @p target consisting of columns representing the
 * HierarchicalAgglomerativeClustering::Cluster entries of the run. If @p labels are given,
 * an additional column is added listing those labels for the initial clusters; clusters that
 * result from merging these initial clusters during the clustering will not have a label.
 *
 * @see HierarchicalAgglomerativeClustering
 */
template<typename T>
void hac_write_cluster_table(
    HierarchicalAgglomerativeClustering<T> const& clustering,
    std::shared_ptr<utils::BaseOutputTarget> target,
    std::vector<std::string> const& labels = std::vector<std::string>{},
    char delimiter = '\t'
) {
    auto const& clusters = clustering.clusters();
    auto& os = target->ostream();

    // Write header
    os << "index" << delimiter;
    os << ( labels.empty() ? "" : "label" ) << delimiter;
    os << "active" << delimiter;
    os << "merger_index\n";

    // Write rows
    for( size_t i = 0; i < clusters.size(); ++i ) {
        os << i << delimiter;
        if( !labels.empty() ) {
            if( i < labels.size() ) {
                os << labels[i] << delimiter;
            } else {
                os << "." << delimiter;
            }
        }
        os << ( clusters[i].active ? "1" : "0" ) << delimiter;
        if( clusters[i].merger_index < std::numeric_limits<size_t>::max() ) {
            os << clusters[i].merger_index << "\n";
        } else {
            os << "NA\n";
        }
    }
}

/**
 * @brief Write a table summarizing the mergers in a hierarchical agglomerative
 * clustering run.
 *
 * This writes a table to the given @p target consisting of columns representing the
 * HierarchicalAgglomerativeClustering::Merger entries of the run.
 *
 * @see HierarchicalAgglomerativeClustering
 */
template<typename T>
void hac_write_merger_table(
    HierarchicalAgglomerativeClustering<T> const& clustering,
    std::shared_ptr<utils::BaseOutputTarget> target,
    char delimiter = '\t'
) {
    auto const& mergers = clustering.mergers();
    auto& os = target->ostream();

    // Write header
    os << "index" << delimiter;
    os << "cluster_index_p" << delimiter;
    os << "cluster_index_a" << delimiter;
    os << "cluster_index_b" << delimiter;
    os << "distance" << delimiter;
    os << "branch_length_a" << delimiter;
    os << "branch_length_b" << "\n";

    // Write rows
    for( size_t i = 0; i < mergers.size(); ++i ) {
        os << i << "\t";
        os << mergers[i].cluster_index_p << delimiter;
        os << mergers[i].cluster_index_a << delimiter;
        os << mergers[i].cluster_index_b << delimiter;
        os << mergers[i].distance << delimiter;
        os << mergers[i].branch_length_a << delimiter;
        os << mergers[i].branch_length_b << "\n";
    }
}

/**
 * @brief Build a Newick-format tree for visualizing the result of a hierarchical agglomerative
 * clustering run.
 *
 * The resulting tree is a dendrogram of the observations, i.e., each leaf node represents
 * one observation. The @p labels vector needs to contain the labels for those observations,
 * in the order of elements that was used for running the clustering.
 *
 * @see HierarchicalAgglomerativeClustering
 */
template<typename T>
std::string hac_dendrogram(
    HierarchicalAgglomerativeClustering<T> const& clustering,
    std::vector<std::string> const& labels,
    bool with_branch_lengths = true
) {
    // Validity checks
    if( labels.size() != clustering.clusters().size() - clustering.mergers().size() ) {
        throw std::invalid_argument(
            "List of labels for dendrogram does not have the same size as the observations."
        );
    }
    for( auto const& label : labels ) {
        if( label.empty() ) {
            throw std::invalid_argument( "List of labels for dendrogram contains empty labels." );
        }
        auto const all_newick_compatible = std::all_of(
            label.cbegin(), label.cend(),
            []( char c ) {
                auto const is_quote = ( c == '\"' ) || ( c == '\'' );
                auto const is_paren = ( c == '(' ) || ( c == ')' ) || ( c == '[' ) || ( c == ']' );
                auto const is_weird = ( c == ':' ) || ( c == ';' ) || ( c == ',' ) || ( c == '=' );
                return is_graph(c) && (!is_quote) && (!is_paren) && (!is_weird);
            }
        );
        if( ! all_newick_compatible ) {
            throw std::invalid_argument(
                "List of labels for dendrogram contains invalid characters that cannot be used "
                "for a Newick-style tree, such as spaces or any of `:;()[],=`"
            );
        }
    }

    // Build the newick-style string, which is constructed backwards.
    auto list = labels;
    for( size_t i = 0; i < clustering.mergers().size(); ++i ) {
        auto const& cm = clustering.mergers()[i];
        std::string node_a = std::move( list[ cm.cluster_index_a ] );
        std::string node_b = std::move( list[ cm.cluster_index_b ] );
        list[ cm.cluster_index_a ].clear();
        list[ cm.cluster_index_b ].clear();
        if( with_branch_lengths ) {
            node_a += ":" + to_string_nice( cm.branch_length_a );
            node_b += ":" + to_string_nice( cm.branch_length_b );
        }
        list.push_back(
            "(" + std::move( node_a ) + "," + std::move( node_b ) + ")" +
            std::to_string( i + labels.size() )
        );
    }

    // The above loop will end with a single entry at the end of the list if the clustering
    // was run in full. However, if clusters were deactivated by the keep_active_function(),
    // we might end up with individual trees for each deactivated cluster, which we now need
    // to collect.
    // return list.back() + ";";
    std::string result;
    for( auto& entry : list ) {
        if( !entry.empty() ) {
            if( !result.empty() ) {
                result += '\n';
            }
            result += std::move( entry ) + ";";
        }
    }
    return result;
}

} // namespace utils
} // namespace genesis

#endif // include guard
