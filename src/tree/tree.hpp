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
//     Forward Declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeIteratorRoundtrip;

// =============================================================================
//     Tree
// =============================================================================

template <class NodeDataType = DefaultNodeData, class EdgeDataType = DefaultEdgeData>
class Tree
{
    friend class TreeIteratorRoundtrip<NodeDataType, EdgeDataType>;

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode<NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge<NodeDataType, EdgeDataType> EdgeType;

    typedef TreeIteratorRoundtrip<NodeDataType, EdgeDataType> IteratorRoundtrip;

    // -----------------------------------------------------
    //     Construction and Destruction
    // -----------------------------------------------------

    Tree () {}
    void clear();
    virtual ~Tree();

    // -----------------------------------------------------
    //     Read and Write
    // -----------------------------------------------------

    bool FromNewickFile (const std::string& fn);
    bool FromNewickString (const std::string& tree);
    void FromTreeBroker (TreeBroker& broker);

    void        ToNewickFile   (const std::string& fn);
    std::string ToNewickString ();
    void        ToTreeBroker   (TreeBroker& broker);

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

    inline IteratorRoundtrip BeginRoundtrip()
    {
        return IteratorRoundtrip(this);
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    int  MaxRank() const;
    bool IsBifurcating() const;

    // -----------------------------------------------------
    //     Debug and Dump
    // -----------------------------------------------------

    bool Validate() const;

    std::string DumpAll() const;
    std::string DumpLinks() const;
    std::string DumpNodes() const;
    std::string DumpEdges() const;
    std::string DumpRoundtrip() const;

protected:
    int LinkPointerToIndex (LinkType* link) const;
    int NodePointerToIndex (NodeType* node) const;
    int EdgePointerToIndex (EdgeType* edge) const;

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    std::vector<LinkType*> links_;
    std::vector<NodeType*> nodes_;
    std::vector<EdgeType*> edges_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree.tpp"

// Also include the iterator classes, they are good friends of ours.
#include "tree/tree_iterator.hpp"

#endif // include guard
