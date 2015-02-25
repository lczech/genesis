/**
 * @brief Implementation of Bipartitions class.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <sstream>

namespace genesis {

// =============================================================================
//     Bipartition
// =============================================================================

/**
 * @brief
 */

// =============================================================================
//     Bipartitions
// =============================================================================

template <class NDT, class EDT>
void Bipartitions<NDT, EDT>::Make()
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
            size_t leaf_idx = node_to_leaf_map_[it.Node()->Index()];
            bp.leaf_nodes_.Set(leaf_idx);
        }
        bipartitions_[it.Node()->Index()] = bp;
    }
}

template <class NDT, class EDT>
void Bipartitions<NDT, EDT>::MakeIndex()
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

// -------------------------------------------------------------
//     Dump and Debug
// -------------------------------------------------------------

template <class NDT, class EDT>
bool Bipartitions<NDT, EDT>::Validate()
{
    return true;
}

template <class NDT, class EDT>
std::string Bipartitions<NDT, EDT>::Dump()
{
    std::ostringstream out;

    out << "Node to Leaf Map:\n";
    for (size_t i = 0; i < node_to_leaf_map_.size(); ++i) {
        out << "    " << i << " --> " << node_to_leaf_map_[i] << "\n";
    }

    out << "Leaf to Node Map:\n";
    for (size_t i = 0; i < leaf_to_node_map_.size(); ++i) {
        out << "    " << i << " --> " << leaf_to_node_map_[i] << "\n";
    }

    for (BipartitionType bi : bipartitions_) {
        out << "";
    }
    return out.str();
}

} // namespace genesis
