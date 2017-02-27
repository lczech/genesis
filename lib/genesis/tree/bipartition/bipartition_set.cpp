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
 * @brief Implementation of BipartitionSet class.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/bipartition/bipartition_set.hpp"

#include "genesis/tree/bipartition/bipartition_set.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/text/string.hpp"

#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace tree {

// -------------------------------------------------------------
//     Dump and Debug
// -------------------------------------------------------------

void BipartitionSet::make()
{
    const size_t num_leaves = leaf_node_count( tree_ );
    make_index();

    bipartitions_.clear();
    bipartitions_.resize(tree_.node_count(), Bipartition(num_leaves));

    for( auto it : postorder(tree_) ) {
        if (it.is_last_iteration()) {
            continue;
        }

        Bipartition bp(num_leaves);
        bp.link_ = &it.link();
        if (it.node().is_leaf()) {
            const int leaf_idx = node_to_leaf_map_[it.node().index()];
            assert(leaf_idx > -1);
            bp.leaf_nodes_.set(leaf_idx);
        } else {
            TreeLink* l = &it.link().next();
            while( l != &it.link() ) {
                bp.leaf_nodes_ |= bipartitions_[l->outer().node().index()].leaf_nodes_;
                l = &l->next();
            }
        }
        bipartitions_[it.node().index()] = bp;
    }
}

void BipartitionSet::make_index()
{
    leaf_to_node_map_.clear();
    node_to_leaf_map_.clear();
    node_to_leaf_map_.resize(tree_.node_count());

    size_t leaf_idx = 0;
    for (
        auto it = tree_.begin_nodes();
        it != tree_.end_nodes();
        ++it
    ) {
        if ((*it)->is_leaf()) {
            node_to_leaf_map_[(*it)->index()] = leaf_idx;
            leaf_to_node_map_.push_back((*it)->index());
            ++leaf_idx;
        } else {
            node_to_leaf_map_[(*it)->index()] = -1;
        }
    }
    assert(leaf_idx == leaf_to_node_map_.size());
}

/**
 * @brief Finds the smallest subtree (measured in number of nodes) that contains all given nodes.
 *
 * A subtree is defined by one of the two parts of a tree that are splitted by one edge. Thus, this
 * function tries all subtrees by leaving out each edge once.
 *
 * If no fitting subtree exists, the function returns a `nullptr`.
 */
Bipartition* BipartitionSet::find_smallest_subtree (
    std::vector<TreeNode*> nodes
) {
    make();
    utils::Bitvector comp( leaf_node_count(tree_) );

    // make bitvector containing all wanted nodes.
    for (TreeNode* n : nodes) {
        int leaf_idx = node_to_leaf_map_[n->index()];
        if (leaf_idx == -1) {
            throw std::runtime_error(
                "Node at index " + utils::to_string( n->index() ) + " is not leaf."
            );
        }
        comp.set(leaf_idx);
    }

    Bipartition* best_bp   = nullptr;
    size_t           min_count = 0;

    // loop over all bipartitions and compare their bitvectors to the given one, to find one that
    // is a superset. try both ways (normal and inverted) for each bipartition.
    for (Bipartition& bp : bipartitions_) {
        if (!bp.link_) {
            continue;
        }

        if (comp <= bp.leaf_nodes_) {
            if (min_count == 0 || bp.leaf_nodes_.count() < min_count) {
                best_bp   = &bp;
                min_count = bp.leaf_nodes_.count();
            }
        }
        if (comp <= ~(bp.leaf_nodes_)) {
            if (min_count == 0 || (~bp.leaf_nodes_).count() < min_count)  {
                // TODO the invert messes with the data consistency of the bipartition. better make a copy!
                // TODO also, if there is a class subtree at some better, better return this instead of a bipartition.
                bp.invert();
                best_bp   = &bp;
                min_count = bp.leaf_nodes_.count();
            }
        }
    }

    return best_bp;
}

std::unordered_set<size_t> BipartitionSet::get_subtree_edges (
    TreeLink* subtree
) {
    // std::vector<std::string> leaf_names;
    std::unordered_set<size_t> ret;

    // We don't want to use the standard iterator wrapper function here, as we are going
    // to end the iteration after the end of the subtree, instead of iterating the whole tree.
    // So we need to use the iterator class directly.
    using Preorder = IteratorPreorder< TreeLink, TreeNode, TreeEdge >;

    for(
        auto it = Preorder(subtree->next());
        it != Preorder() && &it.link() != &subtree->outer();
        ++it
    ) {
        // TODO not typesafe!
        // if( it.node().is_leaf() ) {
        //     leaf_names.push_back( node_data_cast< DefaultNodeData >( it.node() ).name );
        // }
        if (it.is_first_iteration()) {
            continue;
        }
        ret.insert( it.edge().index() );
    }

    // LOG_DBG << "leaf nodes of subtree:";
    // for (std::string s : leaf_names) {
    //     LOG_DBG1 << s;
    // }

    return ret;
}

// -------------------------------------------------------------
//     Dump and Debug
// -------------------------------------------------------------

bool BipartitionSet::validate()
{
    return true;
}

std::string BipartitionSet::dump()
{
    std::ostringstream out;

    out << "Node to Leaf Map:\n";
    for (size_t i = 0; i < node_to_leaf_map_.size(); ++i) {
        out << "    " << i << " --> " << node_to_leaf_map_[i] << "\n";
    }

    out << "\nLeaf to Node Map:\n";
    for (size_t i = 0; i < leaf_to_node_map_.size(); ++i) {
        out << "    " << i << " --> " << leaf_to_node_map_[i] << "\n";
    }

    for (Bipartition bi : bipartitions_) {
        if (!bi.link_) {
            continue;
        }
        out << "\nNode " << bi.link_->node().index()
            << ", Leaf " << node_to_leaf_map_[bi.link_->node().index()]
            << "\n" << bi.leaf_nodes_.dump() << "\n";
    }
    return out.str();
}

} // namespace tree
} // namespace genesis
