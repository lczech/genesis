#ifndef GNS_TREE_TREE_H_
#define GNS_TREE_TREE_H_

/**
 * @brief Provides the basic interface for phylogenetic trees.
 *
 * For more information, see Tree class.
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <vector>

#include "tree/tree_broker.hpp"
#include "tree/tree_edge.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"

namespace genesis {

// =============================================================================
//     Tree
// =============================================================================

template <class NodeDataType = DefaultNodeData, class EdgeDataType = DefaultEdgeData>
class Tree
{
public:
    Tree () {}
    void clear();
    virtual ~Tree();

    bool FromNewickFile (const std::string& fn);
    bool FromNewickString (const std::string& tree);
    void FromTreeBroker (TreeBroker& broker);

    void        ToNewickFile   (const std::string& fn);
    std::string ToNewickString ();
    void        ToTreeBroker   (TreeBroker& broker);

    std::string DumpAll() const;
    std::string DumpEdges() const;
    std::string DumpLinks() const;
    std::string DumpNodes() const;
    std::string DumpRoundtrip() const;

    int  MaxRank() const;
    bool IsBifurcating() const;

    bool Validate() const;

    // TODO introduce a validate function that checks the integrity of the tree:
    // TODO are all links, edges and nodes connected corretly to each other,
    // TODO is every one of them coverd exactly once when doing a full traversal?
    // TODO do all node->link_ links point to the root? same for all edge->primary?
    // TODO also, if we introduce indices to them for faster access, are those correct?
    // TODO this function will be curtial to ensure correctness of invariants once
    // TODO we start implementing stuff that modifies a tree (add nodes, move branches...)!

protected:
    int EdgePointerToIndex (TreeEdge<NodeDataType, EdgeDataType>* edge) const;
    int LinkPointerToIndex (TreeLink<NodeDataType, EdgeDataType>* link) const;
    int NodePointerToIndex (TreeNode<NodeDataType, EdgeDataType>* node) const;

    std::vector<TreeLink<NodeDataType, EdgeDataType>*> links_;
    std::vector<TreeNode<NodeDataType, EdgeDataType>*> nodes_;
    std::vector<TreeEdge<NodeDataType, EdgeDataType>*> edges_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree.tpp"

#endif // include guard
