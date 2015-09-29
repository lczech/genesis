/**
 * @brief Implementation of BipartitionSet class.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include "tree/bipartition_set.hpp"
#include "utils/logging.hpp"

namespace genesis {

// -------------------------------------------------------------
//     Dump and Debug
// -------------------------------------------------------------

template <class NDT, class EDT>
void BipartitionSet<NDT, EDT>::make()
{
    size_t num_leaves = tree_->leaf_count();
    make_index();

    bipartitions_.clear();
    bipartitions_.resize(tree_->node_count(), BipartitionType(num_leaves));

    for (
        typename TreeType::ConstIteratorPostorder it = tree_->begin_postorder();
        it != tree_->end_postorder();
        ++it
    ) {
        if (it.is_last_iteration()) {
            continue;
        }

        BipartitionType bp(num_leaves);
        bp.link_ = it.link();
        if (it.node()->is_leaf()) {
            int leaf_idx = node_to_leaf_map_[it.node()->index()];
            assert(leaf_idx > -1);
            bp.leaf_nodes_.set(leaf_idx);
        } else {
            LinkType* l = it.link()->next();
            while (l != it.link()) {
                bp.leaf_nodes_ |= bipartitions_[l->outer()->node()->index()].leaf_nodes_;
                l = l->next();
            }
        }
        bipartitions_[it.node()->index()] = bp;
    }
}

template <class NDT, class EDT>
void BipartitionSet<NDT, EDT>::make_index()
{
    leaf_to_node_map_.clear();
    node_to_leaf_map_.clear();
    node_to_leaf_map_.resize(tree_->node_count());

    size_t leaf_idx = 0;
    for (
        typename TreeType::ConstIteratorNodes it = tree_->begin_nodes();
        it != tree_->end_nodes();
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
}

/**
 * @brief Finds the smallest subtree (measured in number of nodes) that contains all given nodes.
 *
 * A subtree is defined by one of the two parts of a tree that are splitted by one edge. Thus, this
 * function tries all subtrees by leaving out each edge once.
 *
 * If no fitting subtree exists, the function returns a `nullptr`.
 */
template <class NDT, class EDT>
typename BipartitionSet<NDT, EDT>::BipartitionType* BipartitionSet<NDT, EDT>::find_smallest_subtree (
    std::vector<BipartitionSet<NDT, EDT>::NodeType*> nodes
) {
    make();
    Bitvector comp(tree_->leaf_count());

    // make bitvector containing all wanted nodes.
    for (NodeType* n : nodes) {
        int leaf_idx = node_to_leaf_map_[n->index()];
        if (leaf_idx == -1) {
            LOG_WARN << "Node at index " << n->index() << " is not leaf.";
            continue;
        }
        comp.set(leaf_idx);
    }

    BipartitionType* best_bp   = nullptr;
    size_t           min_count = 0;

    // loop over all bipartitions and compare their bitvectors to the given one, to find one that
    // is a superset. try both ways (normal and inverted) for each bipartition.
    for (BipartitionType& bp : bipartitions_) {
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

template <class NDT, class EDT>
std::vector<typename BipartitionSet<NDT, EDT>::EdgeType const *>
BipartitionSet<NDT, EDT>::get_subtree_edges (
    const BipartitionSet<NDT, EDT>::LinkType* subtree
) {
    std::vector<std::string> leaf_names;
    std::vector<const EdgeType*> ret;

    for (
        typename TreeType::ConstIteratorPreorder it = tree_->begin_preorder(subtree->next());
        it != tree_->end_preorder() && it.link() != subtree->outer();
        ++it
    ) {
        if (it.node()->is_leaf()) {
            leaf_names.push_back(it.node()->data.name);
        }
        if (it.is_first_iteration()) {
            continue;
        }
        ret.push_back(it.edge());
    }

    LOG_DBG << "leaf nodes of subtree:";
    for (std::string s : leaf_names) {
        LOG_DBG1 << s;
    }

    return ret;
}

// -------------------------------------------------------------
//     Dump and Debug
// -------------------------------------------------------------

template <class NDT, class EDT>
bool BipartitionSet<NDT, EDT>::validate()
{
    return true;
}

template <class NDT, class EDT>
std::string BipartitionSet<NDT, EDT>::dump()
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

    for (BipartitionType bi : bipartitions_) {
        if (!bi.link_) {
            continue;
        }
        out << "\nNode " << bi.link_->node()->index()
            << ", Leaf " << node_to_leaf_map_[bi.link_->node()->index()]
            << "\n" << bi.leaf_nodes_.dump() << "\n";
    }
    return out.str();
}

} // namespace genesis
