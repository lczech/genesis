#ifndef GENESIS_UTILS_MATH_HAC_FUNCTIONS_H_
#define GENESIS_UTILS_MATH_HAC_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/utils/math/hac.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// ================================================================================================
//     HAC Functions
// ================================================================================================

/**
 * @brief Build a Newick-format tree for visualizing the result of a hierarchical agglomerative
 * clustering run.
 *
 * The resulting tree is a dendrogram of the observations, i.e., each leaf node represents
 * one observation. The @p labels vector needs to contain the labels for those observations,
 * in the order of elements that was used for running the clustering.
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
