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

template <class NodeDataType, class EdgeDataType>
class TreeIteratorPreorder;

template <class NodeDataType, class EdgeDataType>
class TreeIteratorPostorder;

template <class NodeDataType, class EdgeDataType>
class TreeIteratorLevelorder;

// =============================================================================
//     Tree
// =============================================================================

template <class NodeDataType = DefaultNodeData, class EdgeDataType = DefaultEdgeData>
class Tree
{
public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode<NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge<NodeDataType, EdgeDataType> EdgeType;

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
    //     Accessors
    // -----------------------------------------------------

    inline LinkType* RootLink()
    {
        return links_.front();
    }

    inline NodeType* RootNode()
    {
        return links_.front()->Node();
    }

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

    typedef TreeIteratorRoundtrip <NodeDataType, EdgeDataType> IteratorRoundtrip;
    typedef TreeIteratorPreorder  <NodeDataType, EdgeDataType> IteratorPreorder;
    typedef TreeIteratorPostorder <NodeDataType, EdgeDataType> IteratorPostorder;
    typedef TreeIteratorLevelorder<NodeDataType, EdgeDataType> IteratorLevelorder;

    typedef typename std::vector<LinkType*>::iterator          IteratorLinks;
    typedef typename std::vector<NodeType*>::iterator          IteratorNodes;
    typedef typename std::vector<EdgeType*>::iterator          IteratorEdges;

    // -----------------------
    //     Roundtrip
    // -----------------------

    inline IteratorRoundtrip BeginRoundtrip()
    {
        return IteratorRoundtrip(links_.front());
    }

    inline IteratorRoundtrip BeginRoundtrip(LinkType* link)
    {
        return IteratorRoundtrip(link);
    }

    inline IteratorRoundtrip BeginRoundtrip(NodeType* node)
    {
        return IteratorRoundtrip(node->PrimaryLink());
    }

    inline IteratorRoundtrip EndRoundtrip()
    {
        return IteratorRoundtrip(nullptr);
    }

    // -----------------------
    //     Preorder
    // -----------------------

    inline IteratorPreorder BeginPreorder()
    {
        return IteratorPreorder(links_.front());
    }

    inline IteratorPreorder BeginPreorder(LinkType* link)
    {
        return IteratorPreorder(link);
    }

    inline IteratorPreorder BeginPreorder(NodeType* node)
    {
        return IteratorPreorder(node->PrimaryLink());
    }

    inline IteratorPreorder EndPreorder()
    {
        return IteratorPreorder(nullptr);
    }

    // -----------------------
    //     Postorder
    // -----------------------

    inline IteratorPostorder BeginPostorder()
    {
        return IteratorPostorder(links_.front());
    }

    inline IteratorPostorder BeginPostorder(LinkType* link)
    {
        return IteratorPostorder(link);
    }

    inline IteratorPostorder BeginPostorder(NodeType* node)
    {
        return IteratorPostorder(node->PrimaryLink());
    }

    inline IteratorPostorder EndPostorder()
    {
        return IteratorPostorder(nullptr);
    }

    // -----------------------
    //     Levelorder
    // -----------------------

    inline IteratorLevelorder BeginLevelorder()
    {
        return IteratorLevelorder(links_.front());
    }

    inline IteratorLevelorder BeginLevelorder(LinkType* link)
    {
        return IteratorLevelorder(link);
    }

    inline IteratorLevelorder BeginLevelorder(NodeType* node)
    {
        return IteratorLevelorder(node->PrimaryLink());
    }

    inline IteratorLevelorder EndLevelorder()
    {
        return IteratorLevelorder(nullptr);
    }

    // -----------------------
    //     Links
    // -----------------------

    inline IteratorLinks BeginLinks()
    {
        return links_.begin();
    }

    inline IteratorLinks EndLinks()
    {
        return links_.end();
    }

    // -----------------------
    //     Nodes
    // -----------------------

    inline IteratorNodes BeginNodes()
    {
        return nodes_.begin();
    }

    inline IteratorNodes EndNodes()
    {
        return nodes_.end();
    }

    // -----------------------
    //     Edges
    // -----------------------

    inline IteratorEdges BeginEdges()
    {
        return edges_.begin();
    }

    inline IteratorEdges EndEdges()
    {
        return edges_.end();
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    // TODO add other interesting member functions: http://en.wikipedia.org/wiki/Tree_%28data_structure%29

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
