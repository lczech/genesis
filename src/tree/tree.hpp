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

#include "tree/newick_broker.hpp"
#include "tree/tree_edge.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "utils/math.hpp"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeIteratorRoundtrip;

template <class NodeDataType, class EdgeDataType>
class TreeIteratorPreorder;

//~ template <class NodeDataType, class EdgeDataType>
//~ class TreeIteratorInorder;

template <class NodeDataType, class EdgeDataType>
class TreeIteratorPostorder;

template <class NodeDataType, class EdgeDataType>
class TreeIteratorLevelorder;

// =============================================================================
//     Tree
// =============================================================================

/**
 * @brief Basic class for representing phylogenetic tree topologies.
 *
 * Data in the Tree is not directly stored in the elements (Links, Nodes, Edges) of the Tree.
 * Instead, data belonging to nodes and edges can be stored there by using appropriate template
 * parameters when creating a tree, with classes that are able to hold the needed data.
 * This ensures a maximum of flexibility for using Trees, because any type of data can be plugged
 * in this way.
 *
 * Additional information concerning the whole tree, as well as special algorithms working on the
 * tree and its data are also not direct members of the tree - for the same reason of flexibility
 * and extensibility: Instead of adding more and more data and algorithms to the tree, we create
 * new classes that encapsulate a tree and all the other needed code to work with it.
 * See Placements for an example.
 *
 * Thus, the tree itself only contains the needed information to store and work with a topology.
 *
 * This class has several redundant ways of representing the same information. On the one hand, this
 * makes using a Tree easy, as one can chose the representaion of data that best fits a given task.
 * On the other hand, maintaining all those invariants when changing the tree topology is tedious.
 * Here is a (comprehensive?) list of invariants of this class:
 *
 *  *  The (virtual) root node and one of its links are stored as first element in the arrays:
 *     `nodes_[0]` and `links_[0]`.
 *  *  The indices in all three arrays (`nodes_`, `links_` and `edges_`) have to match the index
 *     integers stored in those elements: `nodes_[i] == nodes_[i]->index_`.
 *  *  The link that is stored in a node has to be the one pointing towards the root.
 *  *  The primary link of an edge has to point towards the root, the secondary away from it.
 */
template <class NodeDataType = DefaultNodeData, class EdgeDataType = DefaultEdgeData>
class Tree
{
public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef Tree    <NodeDataType, EdgeDataType> TreeType;
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
    void FromNewickBroker (NewickBroker& broker);

    void        ToNewickFile   (const std::string& fn);
    std::string ToNewickString ();
    void        ToNewickBroker (NewickBroker& broker);

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    inline LinkType* RootLink() const
    {
        return links_.front();
    }

    inline NodeType* RootNode() const
    {
        return links_.front()->Node();
    }

    inline LinkType* LinkAt(size_t index) const
    {
        return links_[index];
    }

    inline NodeType* NodeAt(size_t index) const
    {
        return nodes_[index];
    }

    inline EdgeType* EdgeAt(size_t index) const
    {
        return edges_[index];
    }

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

    typedef TreeIteratorRoundtrip <NodeDataType, EdgeDataType> IteratorRoundtrip;
    typedef TreeIteratorPreorder  <NodeDataType, EdgeDataType> IteratorPreorder;
    //~ typedef TreeIteratorInorder   <NodeDataType, EdgeDataType> IteratorInorder;
    typedef TreeIteratorPostorder <NodeDataType, EdgeDataType> IteratorPostorder;
    typedef TreeIteratorLevelorder<NodeDataType, EdgeDataType> IteratorLevelorder;

    typedef typename std::vector<LinkType*>::iterator          IteratorLinks;
    typedef typename std::vector<NodeType*>::iterator          IteratorNodes;
    typedef typename std::vector<EdgeType*>::iterator          IteratorEdges;

    // TODO so far, the End... iterators are called anew for every comparison in a loop like
    // TODO it != tree.EndInorder(), which will slow it down compared to having e.g. a fixed
    // TODO end iterator object or so... not sure, if worth the effort.

    // -----------------------
    //     Roundtrip
    // -----------------------

    inline IteratorRoundtrip BeginRoundtrip()
    {
        return IteratorRoundtrip(RootLink());
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
        return IteratorPreorder(RootLink());
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
    //     Inorder
    // -----------------------

    //~ inline IteratorInorder BeginInorder()
    //~ {
        //~ return IteratorInorder(RootLink());
    //~ }
//~
    //~ inline IteratorInorder BeginInorder(LinkType* link)
    //~ {
        //~ return IteratorInorder(link);
    //~ }
//~
    //~ inline IteratorInorder BeginInorder(NodeType* node)
    //~ {
        //~ return IteratorInorder(node->PrimaryLink());
    //~ }
//~
    //~ inline IteratorInorder EndInorder()
    //~ {
        //~ return IteratorInorder(nullptr);
    //~ }

    // -----------------------
    //     Postorder
    // -----------------------

    inline IteratorPostorder BeginPostorder()
    {
        return IteratorPostorder(RootLink());
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
        return IteratorLevelorder(RootLink());
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

    Matrix<double>* NodeDistanceMatrix();

    size_t LinksSize() const;
    size_t NodesSize() const;
    size_t EdgesSize() const;

    bool HasIdenticalTopology(TreeType& other);
    bool HasIdenticalEdgeData(TreeType& other) const;
    bool HasIdenticalNodeData(TreeType& other) const;
    bool HasIdenticalData(TreeType& other) const;

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

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

/**
 * @brief Short name for a Tree using the default data types DefaultNodeData and DefaultEdgeData.
 *
 * The same type can also be produced by using `Tree<>`.
 */
typedef Tree<DefaultNodeData, DefaultEdgeData> DefaultTree;

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree.tpp"

// Also include the iterator classes, they are good friends of ours.
#include "tree/tree_iterator.hpp"

#endif // include guard
