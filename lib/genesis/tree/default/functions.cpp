/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/tree/default/functions.hpp"

#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Names
// =================================================================================================

/**
 * @brief Returns an unordered set of all TreeNode names of a Tree.
 *
 * If `leaves_only` is set to true, nodes names of inner nodes are not included.
 * Unnamed nodes (`node.data.name == ""`) are always excluded.
 * The only difference to node_names_sorted() is the type of container used for storing the result.
 *
 * The provided Tree needs to have TreeNode%s with data types deriveed from DefaultNodeData.
 */
std::unordered_set<std::string> node_names(
    Tree const& tree,
    bool leaves_only
) {
    std::unordered_set<std::string> name_set;
    for( auto const& node : tree.nodes() ) {
        if( node->is_inner() && leaves_only ) {
            continue;
        }
        auto const name = node->data<DefaultNodeData>().name;
        if( name == "" ) {
            continue;
        }
        name_set.insert( std::move( name ));
    }
    return name_set;
}

/**
 * @brief Returns a set of all TreeNode names of a Tree.
 *
 * If `leaves_only` is set to true, nodes names of inner nodes are not included.
 * Unnamed nodes (`node.data.name == ""`) are always excluded.
 * The only difference to node_names() is the type of container used for storing the result.
 *
 * The provided Tree needs to have TreeNode%s with data types deriveed from DefaultNodeData.
 */
utils::SortedVector<std::string> node_names_sorted(
    Tree const& tree,
    bool leaves_only
) {
    utils::SortedVector<std::string> name_set;
    for( auto const& node : tree.nodes() ) {
        if( node->is_inner() && leaves_only ) {
            continue;
        }
        auto const name = node->data<DefaultNodeData>().name;
        if( name == "" ) {
            continue;
        }
        name_set.insert( std::move( name ));
    }
    return name_set;
}

/**
 * @brief Returns a set of all TreeNode names of a TreeSet.
 *
 * The function returns the set of all names of all Tree%s in the set. See
 * @link node_names( Tree const&, bool ) node_names(...)@endlink this version of the
 * function for details.
 */
std::unordered_set<std::string> node_names(
    TreeSet const& tree_set,
    bool leaves_only
) {
    // It would be faster to directly insert into the resulting container, but this version
    // avoids code duplication and is fast enough for now.
    std::unordered_set<std::string> name_set;
    for( auto const& tree : tree_set ) {
        auto tree_name_set = node_names( tree.tree, leaves_only );
        name_set.insert( tree_name_set.begin(), tree_name_set.end() );
    }
    return name_set;
}

/**
 * @brief Returns a set of all TreeNode names of a TreeSet.
 *
 * The function returns the set of all names of all Tree%s in the set. See
 * @link node_names_sorted( Tree const&, bool ) node_names_sorted(...)@endlink this version of the
 * function for details.
 */
utils::SortedVector<std::string> node_names_sorted(
    TreeSet const& tree_set,
    bool leaves_only
) {
    // It would be faster to directly insert into the resulting container, but this version
    // avoids code duplication and is fast enough for now.
    utils::SortedVector<std::string> name_set;
    for( auto const& tree : tree_set ) {
        // We can use the unsorted version here, which should be a bit faster (not tested...).
        // Sorting is then done when inserting the names into the final set.
        auto tree_name_set = node_names( tree.tree, leaves_only );
        name_set.insert( tree_name_set.begin(), tree_name_set.end() );
    }
    return name_set;
}

/**
 * @brief Finds a Node, given its name. If not found, nullptr is returned.
 */
TreeNode const* find_node(
    Tree const& tree,
    const std::string& name,
    bool replace_underscores
) {
    auto clean_name = name;
    if (replace_underscores) {
        clean_name = utils::replace_all(name, "_", " ");
    }

    for (auto it = tree.begin_nodes(); it != tree.end_nodes(); ++it) {
        if( it->get()->data<DefaultNodeData>().name == clean_name) {
            return it->get();
        }
    }

    return nullptr;
}

/**
 * @brief Finds a Node, given its name. If not found, nullptr is returned.
 */
TreeNode* find_node(
    Tree& tree,
    const std::string& name,
    bool replace_underscores
) {
    // Avoid code duplication according to Scott Meyers.
    auto const& ctree = static_cast< Tree const& >( tree );
    return const_cast< TreeNode* >(
        find_node( ctree, name, replace_underscores )
    );
}

// =================================================================================================
//     Branch Length
// =================================================================================================

/**
 * @brief Get the length of the tree, i.e., the sum of all branch lengths.
 */
double length(Tree const& tree)
{
    double len = 0.0;
    for( auto const& edge : tree.edges() ) {
        len += edge->data<DefaultEdgeData>().branch_length;
    }
    return len;
}

/**
 * @brief Get the height of the tree, i.e., the longest distance from the root to a leaf,
 * measured using the branch_length.
 */
double height(Tree const& tree)
{
    auto dists = node_branch_length_distance_vector(tree);
    return *std::max_element(dists.begin(), dists.end());
}

/**
 * @brief Get the diameter of the tree, i.e., the longest distance between any two nodes,
 * measured using the branch_length.
 */
double diameter( Tree const& tree )
{
    auto dist_mat = node_branch_length_distance_matrix( tree );
    return *std::max_element( dist_mat.begin(), dist_mat.end() );
}

/**
 * @brief Get a vector of all branch lengths of a Tree, index by the
 * @link TreeEdge::index() edge index@endlink.
 */
std::vector<double> branch_lengths(
    Tree const& tree
) {
    std::vector<double> result;
    result.reserve( tree.edge_count() );
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        result.push_back( tree.edge_at(i).data<DefaultEdgeData>().branch_length );
    }
    return result;
}

/**
 * @brief Set all branch lengths of a Tree to a given value.
 *
 * See also scale_all_branch_lengths() for a scaling function.
 */
void set_all_branch_lengths(
    Tree& tree,
    double length
) {
    for( auto& edge : tree.edges() ) {
        edge->data<DefaultEdgeData>().branch_length = length;
    }
}

/**
 * @brief Scale all branch lengths of a Tree by a given factor.
 *
 * This function simply multiplies all branch lengths with the given factor.
 * See also set_all_branch_lengths() for setting the branch lengths to a value.
 */
void scale_all_branch_lengths(
    Tree&  tree,
    double factor
) {
    for( auto& edge : tree.edges() ) {
        edge->data<DefaultEdgeData>().branch_length *= factor;
    }
}

} // namespace tree
} // namespace genesis
