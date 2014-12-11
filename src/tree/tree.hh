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

#include "tree/tree_branch.hh"
#include "tree/tree_broker.hh"
#include "tree/tree_link.hh"
#include "tree/tree_node.hh"

namespace genesis {

// =============================================================================
//     Tree
// =============================================================================

template <class NodeDataType = DefaultNodeData, class BranchDataType = DefaultBranchData>
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
    std::string DumpBranches() const;
    std::string DumpLinks() const;
    std::string DumpNodes() const;
    std::string DumpRoundtrip() const;

    int  MaxRank() const;
    bool IsBifurcating() const;

    bool Validate() const;

    // TODO introduce a validate function that checks the integrity of the tree:
    // TODO are all links, branches and nodes connected corretly to each other,
    // TODO is every one of them coverd exactly once when doing a full traversal?
    // TODO do all node->link_ links point to the root? same for all branch->primary?
    // TODO also, if we introduce indices to them for faster access, are those correct?
    // TODO this function will be curtial to ensure correctness of invariants once
    // TODO we start implementing stuff that modifies a tree (add nodes, move branches...)!

protected:
    int BranchPointerToIndex (TreeBranch<NodeDataType, BranchDataType>* branch) const;
    int LinkPointerToIndex   (TreeLink<NodeDataType,   BranchDataType>* link)   const;
    int NodePointerToIndex   (TreeNode<NodeDataType,   BranchDataType>* node)   const;

    std::vector<TreeLink<NodeDataType,   BranchDataType>*> links_;
    std::vector<TreeNode<NodeDataType,   BranchDataType>*> nodes_;
    std::vector<TreeBranch<NodeDataType, BranchDataType>*> branches_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree.inc.cc"

#endif // include guard
