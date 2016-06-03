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
 * @brief Implementation of Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Names
// =================================================================================================

/**
 * @brief Returns a list of all node names of a Tree.
 *
 * If `leaves_only` is set to true, nodes names of inner nodes are not included.
 * Unnamed nodes (`node.data.name == ""`) are always excluded.
 */
template <class TreeType>
std::vector<std::string> node_names(
    TreeType const& tree,
    bool leaves_only
) {
    std::vector<std::string> ret;
    for (auto it = tree.begin_nodes(); it != tree.end_nodes(); ++it) {
        if ( (*it)->is_inner() && leaves_only ) {
            continue;
        }
        if ( (*it)->data.name == "" ) {
            continue;
        }
        ret.push_back( (*it)->data.name );
    }
    return ret;
}

/**
 * @brief Finds a Node, given its name. If not found, nullptr is returned.
 */
template <class TreeType>
typename TreeType::NodeType const* find_node(
    TreeType const& tree,
    const std::string& name,
    bool replace_underscores
) {
    auto clean_name = name;
    if (replace_underscores) {
        clean_name = utils::replace_all(name, "_", " ");
    }

    for (auto it = tree.begin_nodes(); it != tree.end_nodes(); ++it) {
        if ((*it)->data.name == clean_name) {
            return it->get();
        }
    }

    return nullptr;
}

/**
 * @brief Finds a Node, given its name. If not found, nullptr is returned.
 */
template <class TreeType>
typename TreeType::NodeType* find_node(
    TreeType& tree,
    const std::string& name,
    bool replace_underscores
) {
    // Avoid code duplication according to Scott Meyers.
    auto const& ctree = static_cast< TreeType const >( tree );
    return const_cast< typename TreeType::NodeType* >(
        find_node( ctree, name, replace_underscores )
    );
}

// =================================================================================================
//     Branch Length
// =================================================================================================

/**
 * @brief Get a vector of all branch lengths of a Tree, index by the
 * @link TreeEdge::index() edge index@endlink.
 */
template <class TreeType>
std::vector<double> branch_lengths(
    TreeType const& tree
) {
    std::vector<double> result;
    result.reserve( tree.edge_count() );
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        result.push_back( tree.edge_at(i).data.branch_length );
    }
    return result;
}

/**
 * @brief Set all branch lengths of a Tree to a given value.
 */
template <class TreeType>
void set_all_branch_lengths(
    TreeType& tree,
    double length
) {
    for( auto& edge : tree.edges() ) {
        edge->data.branch_length = length;
    }
}

} // namespace tree
} // namespace genesis
