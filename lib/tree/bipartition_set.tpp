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
void BipartitionSet<NDT, EDT>::Make()
{
    size_t num_leaves = tree_->LeafCount();
    MakeIndex();

    bipartitions_.clear();
    bipartitions_.resize(tree_->NodeCount(), BipartitionType(num_leaves));

    for (
        typename TreeType::ConstIteratorPostorder it = tree_->BeginPostorder();
        it != tree_->EndPostorder();
        ++it
    ) {
        if (it.IsLastIteration()) {
            continue;
        }

        BipartitionType bp(num_leaves);
        bp.link_ = it.Link();
        if (it.Node()->IsLeaf()) {
            int leaf_idx = node_to_leaf_map_[it.Node()->Index()];
            assert(leaf_idx > -1);
            bp.leaf_nodes_.Set(leaf_idx);
        } else {
            LinkType* l = it.Link()->Next();
            while (l != it.Link()) {
                bp.leaf_nodes_ |= bipartitions_[l->Outer()->Node()->Index()].leaf_nodes_;
                l = l->Next();
            }
        }
        bipartitions_[it.Node()->Index()] = bp;
    }
}

template <class NDT, class EDT>
void BipartitionSet<NDT, EDT>::MakeIndex()
{
    leaf_to_node_map_.clear();
    node_to_leaf_map_.clear();
    node_to_leaf_map_.resize(tree_->NodeCount());

    size_t leaf_idx = 0;
    for (
        typename TreeType::ConstIteratorNodes it = tree_->BeginNodes();
        it != tree_->EndNodes();
        ++it
    ) {
        if ((*it)->IsLeaf()) {
            node_to_leaf_map_[(*it)->Index()] = leaf_idx;
            leaf_to_node_map_.push_back((*it)->Index());
            ++leaf_idx;
        } else {
            node_to_leaf_map_[(*it)->Index()] = -1;
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
typename BipartitionSet<NDT, EDT>::BipartitionType* BipartitionSet<NDT, EDT>::FindSmallestSubtree (
    std::vector<BipartitionSet<NDT, EDT>::NodeType*> nodes
) {
    Make();
    Bitvector comp(tree_->LeafCount());

    // make bitvector containing all wanted nodes.
    for (NodeType* n : nodes) {
        int leaf_idx = node_to_leaf_map_[n->Index()];
        if (leaf_idx == -1) {
            LOG_WARN << "Node at index " << n->Index() << " is not leaf.";
            continue;
        }
        comp.Set(leaf_idx);
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
            if (min_count == 0 || bp.leaf_nodes_.Count() < min_count) {
                best_bp   = &bp;
                min_count = bp.leaf_nodes_.Count();
            }
        }
        if (comp <= ~(bp.leaf_nodes_)) {
            if (min_count == 0 || (~bp.leaf_nodes_).Count() < min_count)  {
                // TODO the invert messes with the data consistency of the bipartition. better make a copy!
                // TODO also, if there is a class subtree at some better, better return this instead of a bipartition.
                bp.Invert();
                best_bp   = &bp;
                min_count = bp.leaf_nodes_.Count();
            }
        }
    }

    return best_bp;
}

template <class NDT, class EDT>
std::vector<typename BipartitionSet<NDT, EDT>::EdgeType const *>
BipartitionSet<NDT, EDT>::GetSubtreeEdges (
    const BipartitionSet<NDT, EDT>::LinkType* subtree
) {
    std::vector<std::string> leaf_names;
    std::vector<const EdgeType*> ret;

    for (
        typename TreeType::ConstIteratorPreorder it = tree_->BeginPreorder(subtree->Next());
        it != tree_->EndPreorder() && it.Link() != subtree->Outer();
        ++it
    ) {
        if (it.Node()->IsLeaf()) {
            leaf_names.push_back(it.Node()->name);
        }
        if (it.IsFirstIteration()) {
            continue;
        }
        ret.push_back(it.Edge());
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
bool BipartitionSet<NDT, EDT>::Validate()
{
    return true;
}

template <class NDT, class EDT>
std::string BipartitionSet<NDT, EDT>::Dump()
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
        out << "\nNode " << bi.link_->Node()->Index()
            << ", Leaf " << node_to_leaf_map_[bi.link_->Node()->Index()]
            << "\n" << bi.leaf_nodes_.Dump() << "\n";
    }
    return out.str();
}

} // namespace genesis
