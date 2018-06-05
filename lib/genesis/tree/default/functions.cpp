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
 * @brief Implementation of Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/default/functions.hpp"

#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/function/tree_set.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Names
// =================================================================================================

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

double length(Tree const& tree)
{
    double len = 0.0;
    for( auto const& edge : tree.edges() ) {
        len += edge->data<DefaultEdgeData>().branch_length;
    }
    return len;
}

double height(Tree const& tree)
{
    auto dists = node_branch_length_distance_vector(tree);
    return *std::max_element(dists.begin(), dists.end());
}

double diameter( Tree const& tree )
{
    auto dist_mat = node_branch_length_distance_matrix( tree );
    return *std::max_element( dist_mat.begin(), dist_mat.end() );
}

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

void set_all_branch_lengths(
    Tree& tree,
    double length
) {
    for( auto& edge : tree.edges() ) {
        edge->data<DefaultEdgeData>().branch_length = length;
    }
}

void scale_all_branch_lengths(
    Tree&  tree,
    double factor
) {
    for( auto& edge : tree.edges() ) {
        edge->data<DefaultEdgeData>().branch_length *= factor;
    }
}

Tree average_branch_length_tree( TreeSet const& tset )
{
    using TreeType = Tree;

    if( tset.size() == 0 ) {
        return TreeType();
    }

    if( ! all_identical_topology( tset )) {
        throw std::runtime_error( "Trees in TreeSet do not have the same topology." );
    }

    // Prepare storage for average branch lengths.
    size_t num_edges = tset.at(0).tree.edge_count();
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
        for( auto it : preorder(ct.tree) ) {
            // The first iteration points to an edge which will be covered later again.
            // Skip it to prevent double coverage.
            if (it.is_first_iteration()) {
                continue;
            }

            avgs[idx] += it.edge().data<DefaultEdgeData>().branch_length;
            ++idx;
        }
    }

    // We know that all trees have the same topology. So we take a copy of the first one
    // (thus, also copying its node names) and modify its branch lengths.
    TreeType tree = TreeType( tset.at(0).tree );

    // Do the same kind of traversal as before in order to keep the indexing order (preorder) and
    // set the branch lengths.
    size_t idx = 0;
    for( auto it : preorder(tree) ) {
        // The first iteration points to an edge which will be covered later again.
        // Skip it to prevent double coverage.
        if (it.is_first_iteration()) {
            continue;
        }

        it.edge().data<DefaultEdgeData>().branch_length = avgs[idx] / tset.size();
        ++idx;
    }

    return tree;
}

} // namespace tree
} // namespace genesis
