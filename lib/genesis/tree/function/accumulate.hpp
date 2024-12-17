#ifndef GENESIS_TREE_FUNCTION_ACCUMULATE_H_
#define GENESIS_TREE_FUNCTION_ACCUMULATE_H_

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
 * @ingroup tree
 */

#include "genesis/tree/tree.hpp"
#include "genesis/tree/iterator/levelorder.hpp"
#include "genesis/tree/iterator/node_links.hpp"
#include "genesis/tree/iterator/postorder.hpp"

#include <cassert>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree Edge Data Accumulation
// =================================================================================================

/**
 * @brief Given a Tree and a vector of values per edge, accumulate the values outwards.
 *
 * This function takes a vector of values per edge of the given @p tree, and accumulates them
 * inside-out. Starting at edges from the root, the function works its way outwards of the tree,
 * and accumulates the given values along each branch. As a result, each edge then contains the
 * sum of all values from that edge to the root.
 *
 * @see accumulate_edge_values_inwards() for the opposite function.
 */
template<typename T>
std::vector<T> accumulate_edge_values_outwards(
    Tree const& tree,
    std::vector<T> const& edge_values
) {
    // Input check
    if( tree.edge_count() != edge_values.size()  ) {
        throw std::invalid_argument(
            "Cannot accumulate edge values with vector of different size than the tree has edges"
        );
    }

    // Prepare the result
    auto result = std::vector<T>( edge_values.size(), 0.0 );
    assert( result.size() == tree.edge_count() );

    // Inside-out, accumulate the values of edges
    for( auto const& node_it : levelorder( tree )) {
        if( is_root( node_it.node() )) {
            continue;
        }
        assert(
            node_it.edge().index() != node_it.edge().primary_node().link().edge().index() ||
            is_root(node_it.edge().primary_node())
        );
        assert(
            result[node_it.edge().index()] == result[node_it.edge().primary_node().link().edge().index()] ||
            is_root(node_it.edge().primary_node())
        );
        result[node_it.edge().index()] += edge_values[node_it.edge().index()];
        for( auto const& link_it : node_links( node_it.node() )) {
            if( link_it.is_first_iteration() ) {
                continue;
            }
            assert( result[link_it.edge().index()] == 0.0 );
            result[link_it.edge().index()] = result[node_it.edge().index()];
        }
    }
    return result;
}

/**
 * @brief Given a Tree and a vector of values per edge, accumulate the values inwards.
 *
 * This function takes a vector of values per edge of the given @p tree, and accumulates them
 * outside-in. Starting at leaf edges, the function works its way towards the root of the tree,
 * and accumulates the given values along each branch. As a result, each edge then contains the
 * sum of all values from that edge towards all its children.
 *
 * @see accumulate_edge_values_outwards() for the opposite function.
 */
template<typename T>
std::vector<T> accumulate_edge_values_inwards(
    Tree const& tree,
    std::vector<T> const& edge_values
) {
    // Input check
    if( tree.edge_count() != edge_values.size()  ) {
        throw std::invalid_argument(
            "Cannot accumulate edge values with vector of different size than the tree has edges"
        );
    }

    // Prepare the result
    auto result = std::vector<T>( edge_values.size(), 0.0 );
    assert( result.size() == tree.edge_count() );

    // Outside-in, accumulate the values of edges
    for( auto const& node_it : postorder( tree )) {
        if( is_root( node_it.node() )) {
            continue;
        }
        result[node_it.edge().index()] += edge_values[node_it.edge().index()];
        for( auto const& link_it : node_links( node_it.node() )) {
            if( link_it.is_first_iteration() ) {
                continue;
            }
            result[node_it.edge().index()] += result[link_it.edge().index()];
        }
    }
    return result;
}

} // namespace tree
} // namespace genesis

#endif // include guard
