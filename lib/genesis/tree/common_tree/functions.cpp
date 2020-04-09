/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
 * @brief Implementation of Common Tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/common_tree/functions.hpp"

#include "genesis/tree/common_tree/distances.hpp"
#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/function/tree_set.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Names
// =================================================================================================

std::vector<std::string> node_names(
    Tree const& tree,
    bool leaves_only
) {
    std::vector<std::string> names;
    for( auto const& node : tree.nodes() ) {
        if( is_inner( node ) && leaves_only ) {
            continue;
        }
        auto const name = node.data<CommonNodeData>().name;
        if( name.empty() ) {
            continue;
        }
        names.push_back( std::move( name ));
    }
    return names;
}

std::vector<std::string> node_names(
    std::vector<Tree> const& tree_set,
    bool leaves_only
) {
    // It would be faster to directly insert into the resulting container, but this version
    // avoids code duplication and is fast enough for now.
    std::vector<std::string> names;
    for( auto const& tree : tree_set ) {
        auto tree_name_set = node_names( tree, leaves_only );
        names.insert( names.end(), tree_name_set.begin(), tree_name_set.end() );
    }
    return names;
}

TreeNode const* find_node(
    Tree const& tree,
    const std::string& name,
    bool throw_on_failure,
    bool replace_underscores
) {
    auto clean_name = name;
    if (replace_underscores) {
        clean_name = utils::replace_all(name, "_", " ");
    }

    // Try to find the node, return immediately on success.
    for( auto const& node : tree.nodes() ) {
        if( node.data<CommonNodeData>().name == clean_name ) {
            return &node;
        }
    }

    // If we are here, we did not find the not. Fail.
    if( throw_on_failure ) {
        throw std::invalid_argument( "Cannot find node '" + name + "' in tree." );
    }
    return nullptr;
}

TreeNode* find_node(
    Tree& tree,
    const std::string& name,
    bool throw_on_failure,
    bool replace_underscores
) {
    // Avoid code duplication according to Scott Meyers.
    auto const& ctree = static_cast< Tree const& >( tree );
    return const_cast< TreeNode* >(
        find_node( ctree, name, throw_on_failure, replace_underscores )
    );
}

std::vector<TreeNode const*> find_nodes(
    Tree const& tree,
    std::vector<std::string> const& node_names,
    bool throw_on_failure,
    bool replace_underscores
) {
    // Prepare result
    std::vector<TreeNode const*> node_list;
    node_list.reserve( node_names.size() );

    // Find and return nodes
    for( auto const& taxon : node_names ) {
        node_list.push_back( find_node( tree, taxon, throw_on_failure, replace_underscores ));
    }
    return node_list;
}

std::vector<TreeNode*> find_nodes(
    Tree& tree,
    std::vector<std::string> const& node_names,
    bool throw_on_failure,
    bool replace_underscores
) {
    // Prepare result
    std::vector<TreeNode*> node_list;
    node_list.reserve( node_names.size() );

    // Find and return nodes
    for( auto const& taxon : node_names ) {
        node_list.push_back( find_node( tree, taxon, throw_on_failure, replace_underscores ));
    }
    return node_list;
}

// =================================================================================================
//     Branch Length
// =================================================================================================

double length(Tree const& tree)
{
    double len = 0.0;
    for( auto const& edge : tree.edges() ) {
        len += edge.data<CommonEdgeData>().branch_length;
    }
    return len;
}

double height(Tree const& tree)
{
    if( tree.empty() ) {
        return 0.0;
    }
    auto dists = node_branch_length_distance_vector(tree);
    return *std::max_element(dists.begin(), dists.end());
}

double diameter( Tree const& tree )
{
    if( tree.empty() ) {
        return 0.0;
    }
    auto dist_mat = node_branch_length_distance_matrix( tree );
    assert( ! dist_mat.empty() );
    assert( dist_mat.rows() == dist_mat.cols() );
    return *std::max_element( dist_mat.begin(), dist_mat.end() );
}

std::vector<double> branch_lengths(
    Tree const& tree
) {
    std::vector<double> result;
    result.reserve( tree.edge_count() );
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        result.push_back( tree.edge_at(i).data<CommonEdgeData>().branch_length );
    }
    return result;
}

void set_all_branch_lengths(
    Tree& tree,
    double length
) {
    for( auto& edge : tree.edges() ) {
        edge.data<CommonEdgeData>().branch_length = length;
    }
}

void scale_all_branch_lengths(
    Tree&  tree,
    double factor
) {
    for( auto& edge : tree.edges() ) {
        edge.data<CommonEdgeData>().branch_length *= factor;
    }
}

Tree average_branch_length_tree( std::vector<Tree> const& tset )
{
    if( tset.size() == 0 ) {
        return Tree();
    }

    if( ! identical_topology( tset )) {
        throw std::runtime_error(
            "Cannot calculate average branch length tree. Trees do not have the same topology."
        );
    }

    // Prepare storage for average branch lengths.
    size_t num_edges = tset.at(0).edge_count();
    auto avgs = std::vector<double>(num_edges, 0.0);

    // We traverse all trees (again, because all_identical_topology() already did this). This is
    // probably a bit slower than the previous version of this method which worked with less
    // traversals, but way easier to understand and debug.
    for( auto& ct : tset ) {
        // Use an index for position in the preorder traversal. This makes sure that the
        // index actually always points to the correct edges, indepently of their order in
        // different trees in the set.
        size_t idx = 0;

        // Do a preorder traversal and collect branch lengths.
        for( auto it : preorder(ct) ) {
            // The first iteration points to an edge which will be covered later again.
            // Skip it to prevent double coverage.
            if (it.is_first_iteration()) {
                continue;
            }

            avgs[idx] += it.edge().data<CommonEdgeData>().branch_length;
            ++idx;
        }
    }

    // We know that all trees have the same topology. So we take a copy of the first one
    // (thus, also copying its node names) and modify its branch lengths.
    auto tree = tset.at(0);

    // Do the same kind of traversal as before in order to keep the indexing order (preorder) and
    // set the branch lengths.
    size_t idx = 0;
    for( auto it : preorder(tree) ) {
        // The first iteration points to an edge which will be covered later again.
        // Skip it to prevent double coverage.
        if (it.is_first_iteration()) {
            continue;
        }

        it.edge().data<CommonEdgeData>().branch_length = avgs[idx] / tset.size();
        ++idx;
    }

    return tree;
}

} // namespace tree
} // namespace genesis
