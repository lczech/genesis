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
 * @ingroup tree
 */

#include "utils/core/logging.hpp"

#include <algorithm>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Count Properties
// =================================================================================================

/**
 * @brief Return the highest rank of the nodes of the Tree.
 */
template <class Tree>
int max_rank( Tree const& tree )
{
    int max = -1;
    for (size_t i = 0; i < tree.node_count(); ++i) {
        int rank = tree.node_at(i).rank();
        // if (rank == 1) {
        //     LOG_WARN << "Node with rank 1 found. This is a node without furcation, and usually "
        //              << "indicates an error.";
        // }
        max = std::max(rank, max);
    }
    return max;
}

/**
 * @brief Return whether the Tree is bifurcating.
 */
template <class Tree>
bool is_bifurcating( Tree const& tree )
{
    return max_rank( tree ) == 2;
}

/**
 * @brief Count the number of leaf nodes of a Tree.
 */
template <class Tree>
size_t leaf_nodes_count( Tree const& tree )
{
    size_t sum = 0;
    for (size_t i = 0; i < tree.node_count(); ++i) {
        auto const& n = tree.node_at(i);
        if (n.is_leaf()) {
            ++sum;
        }
    }
    return sum;
}

/**
 * @brief Count the number of inner nodes.
 */
template <class Tree>
size_t inner_nodes_count( Tree const& tree )
{
    return tree.node_count() - leaf_nodes_count( tree );
}

} // namespace tree
} // namespace genesis
