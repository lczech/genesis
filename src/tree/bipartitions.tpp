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

template <class NDT, class EDT>
void Bipartitions<NDT, EDT>::GetSubtreeEdges(std::vector<Bipartitions<NDT, EDT>::NodeType*> nodes)
{
    Make();
    size_t num_leaves = tree_->LeafCount();
    BipartitionType comp_bp = BipartitionType(num_leaves);

    LOG_DBG << "entering init loop";
    for (NodeType* n : nodes) {
        int leaf_idx = node_to_leaf_map_[n->Index()];
        if (leaf_idx == -1) {
            LOG_WARN << "Node not leaf.";
        }
        LOG_DBG1 << "leaf index " << leaf_idx;
        comp_bp.leaf_nodes_.Set(leaf_idx);
    }
    LOG_DBG1 << "comp " << comp_bp.leaf_nodes_.Dump();
    LOG_DBG;

    BipartitionType* res;
    size_t min_count = 0;
    for (BipartitionType& bi : bipartitions_) {
        if (!bi.link_) {
            continue;
        }
        //~ LOG_DBG1 << "at node " << bi.link_->Node()->name;

        if (comp_bp.leaf_nodes_ <= bi.leaf_nodes_) {
            //~ LOG_DBG1 << "found  " << bi.leaf_nodes_.Dump();
            if (min_count == 0 || bi.leaf_nodes_.Count() < min_count) {
                //~ LOG_DBG1 << "min";
                res = &bi;
                min_count = bi.leaf_nodes_.Count();
            }
        }
        if (comp_bp.leaf_nodes_ <= ~(bi.leaf_nodes_)) {
            //~ LOG_DBG1 << "foundx " << (~bi.leaf_nodes_).Dump();
            //~ LOG_DBG1 << "min_count " << min_count << ", (~bi.leaf_nodes_).Count " << (~bi.leaf_nodes_).Count();
            if (min_count == 0 || (~bi.leaf_nodes_).Count() < min_count) {
                //~ LOG_DBG1 << "min";
                bi.Invert();
                res = &bi;
                min_count = bi.leaf_nodes_.Count();
            } else {
                //~ LOG_DBG1 << "no";
            }
            //~ LOG_DBG;
        }
    }

    std::vector<std::string> leaf_names;

    LOG_DBG << "iterate nodes, starting at " << res->link_->Node()->name;
    for (
        typename TreeType::ConstIteratorPreorder it = tree_->BeginPreorder(res->link_->Next());
        it != tree_->EndPreorder() && it.Link() != res->link_->Outer();
        ++it
    ) {
        LOG_DBG1 << it.Node()->name << " with edge " << it.Edge()->Dump();
        if (it.Node()->IsLeaf()) {
            leaf_names.push_back(it.Node()->name);
        }
    }

    LOG_DBG << "leaf nodes of subtree:";
    for (std::string s : leaf_names) {
        LOG_DBG1 << s;
    }

    LOG_DBG << "end.";
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
