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

#include <functional>
#include <string>
#include <vector>

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
 * A tree in this implementation consists of three types of elements: Links, Nodes and Edges.
 * The topoloty of the tree is completely described by the links, while nodes and edges add the
 * capability to store data on the tree.
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

    typedef std::vector<LinkType*> LinkArray;
    typedef std::vector<NodeType*> NodeArray;
    typedef std::vector<EdgeType*> EdgeArray;

    // -----------------------------------------------------
    //     Construction and Destruction
    // -----------------------------------------------------

    Tree () {};

    Tree (const TreeType& other);
    TreeType& operator = (const TreeType& other);

    // TODO make copy constructor and assignment operator work with other tree types.
    // the following declarations compile, but do not work. maybe they still help for implementing.
    /*
    template <class OtherNodeDataType, class OtherEdgeDataType>
    Tree (const Tree<OtherNodeDataType, OtherEdgeDataType>& other);
    template <class OtherNodeDataType, class OtherEdgeDataType>
    TreeType& operator = (Tree<OtherNodeDataType, OtherEdgeDataType> other);

    template <class OtherNodeDataType, class OtherEdgeDataType, template <typename, typename> class OtherTreeType>
    Tree (const OtherTreeType<OtherNodeDataType, OtherEdgeDataType>& other);
    template <class OtherNodeDataType, class OtherEdgeDataType, template <typename, typename> class OtherTreeType>
    TreeType& operator = (OtherTreeType<OtherNodeDataType, OtherEdgeDataType> other);
    */

    virtual ~Tree();
    void clear();

    void swap (TreeType& other);

    // TODO make swap work with other tree types.
    // the following declarations compile, but do not work. maybe they still help for implementing.
    /*
    template <class OtherNodeDataType, class OtherEdgeDataType>
    void swap (Tree<OtherNodeDataType, OtherEdgeDataType>& other);

    template <class OtherNodeDataType, class OtherEdgeDataType, template <typename, typename> class OtherTreeType>
    void swap (OtherTreeType<OtherNodeDataType, OtherEdgeDataType>& other);
    */

    void Import(LinkArray& links, NodeArray& nodes, EdgeArray& edges);
    void Export(LinkArray& links, NodeArray& nodes, EdgeArray& edges);

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

    /**
     * @brief Returns the number of Links of the Tree.
     */
    inline size_t LinksSize() const
    {
        return links_.size();
    }

    /**
     * @brief Returns the number of Nodes of the Tree.
     */
    inline size_t NodesSize() const
    {
        return nodes_.size();
    }

    /**
     * @brief Returns the number of Edges of the Tree.
     */
    inline size_t EdgesSize() const
    {
        return edges_.size();
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

    typedef typename std::vector<LinkType*>::const_iterator    ConstIteratorLinks;
    typedef typename std::vector<NodeType*>::const_iterator    ConstIteratorNodes;
    typedef typename std::vector<EdgeType*>::const_iterator    ConstIteratorEdges;

    // TODO so far, the End... iterators are called anew for every comparison in a loop like
    // TODO it != tree.EndInorder(), which will slow it down compared to having e.g. a fixed
    // TODO end iterator object or so... not sure, if worth the effort.

    // -----------------------
    //     Roundtrip
    // -----------------------

    inline IteratorRoundtrip BeginRoundtrip() const
    {
        return IteratorRoundtrip(RootLink());
    }

    inline IteratorRoundtrip BeginRoundtrip(const LinkType* link) const
    {
        return IteratorRoundtrip(link);
    }

    inline IteratorRoundtrip BeginRoundtrip(const NodeType* node) const
    {
        return IteratorRoundtrip(node->PrimaryLink());
    }

    inline IteratorRoundtrip EndRoundtrip() const
    {
        return IteratorRoundtrip(nullptr);
    }

    // -----------------------
    //     Preorder
    // -----------------------

    inline IteratorPreorder BeginPreorder() const
    {
        return IteratorPreorder(RootLink());
    }

    inline IteratorPreorder BeginPreorder(const LinkType* link) const
    {
        return IteratorPreorder(link);
    }

    inline IteratorPreorder BeginPreorder(const NodeType* node) const
    {
        return IteratorPreorder(node->PrimaryLink());
    }

    inline IteratorPreorder EndPreorder() const
    {
        return IteratorPreorder(nullptr);
    }

    // -----------------------
    //     Inorder
    // -----------------------

    //~ inline IteratorInorder BeginInorder() const
    //~ {
        //~ return IteratorInorder(RootLink());
    //~ }
//~
    //~ inline IteratorInorder BeginInorder(const LinkType* link) const
    //~ {
        //~ return IteratorInorder(link);
    //~ }
//~
    //~ inline IteratorInorder BeginInorder(const NodeType* node) const
    //~ {
        //~ return IteratorInorder(node->PrimaryLink());
    //~ }
//~
    //~ inline IteratorInorder EndInorder() const
    //~ {
        //~ return IteratorInorder(nullptr);
    //~ }

    // -----------------------
    //     Postorder
    // -----------------------

    inline IteratorPostorder BeginPostorder() const
    {
        return IteratorPostorder(RootLink());
    }

    inline IteratorPostorder BeginPostorder(const LinkType* link) const
    {
        return IteratorPostorder(link);
    }

    inline IteratorPostorder BeginPostorder(const NodeType* node) const
    {
        return IteratorPostorder(node->PrimaryLink());
    }

    inline IteratorPostorder EndPostorder() const
    {
        return IteratorPostorder(nullptr);
    }

    // -----------------------
    //     Levelorder
    // -----------------------

    inline IteratorLevelorder BeginLevelorder() const
    {
        return IteratorLevelorder(RootLink());
    }

    inline IteratorLevelorder BeginLevelorder(const LinkType* link) const
    {
        return IteratorLevelorder(link);
    }

    inline IteratorLevelorder BeginLevelorder(const NodeType* node) const
    {
        return IteratorLevelorder(node->PrimaryLink());
    }

    inline IteratorLevelorder EndLevelorder() const
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

    inline ConstIteratorLinks BeginLinks() const
    {
        return links_.cbegin();
    }

    inline ConstIteratorLinks EndLinks() const
    {
        return links_.cend();
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

    inline ConstIteratorNodes BeginNodes() const
    {
        return nodes_.cbegin();
    }

    inline ConstIteratorNodes EndNodes() const
    {
        return nodes_.cend();
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

    inline ConstIteratorEdges BeginEdges() const
    {
        return edges_.cbegin();
    }

    inline ConstIteratorEdges EndEdges() const
    {
        return edges_.cend();
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    // TODO add other interesting member functions: http://en.wikipedia.org/wiki/Tree_%28data_structure%29

    int  MaxRank() const;
    bool IsBifurcating() const;

    Matrix<int>*        NodeDepthMatrix    ()                               const;
    std::vector<int>    NodeDepthVector    (const NodeType* node = nullptr) const;
    Matrix<double>*     NodeDistanceMatrix ()                               const;
    std::vector<double> NodeDistanceVector (const NodeType* node = nullptr) const;

    static bool Equal(
        const TreeType& lhs,
        const TreeType& rhs,
        const std::function<bool (TreeType::IteratorPreorder&, TreeType::IteratorPreorder&)> comparator
    );
    bool Equal(
        const TreeType& other,
        const std::function<bool (TreeType::IteratorPreorder&, TreeType::IteratorPreorder&)> comparator
    ) const;

    bool HasIdenticalTopology(const TreeType& other) const;
    bool HasIdenticalEdgeData(const TreeType& other) const;
    bool HasIdenticalNodeData(const TreeType& other) const;
    bool HasIdenticalData    (const TreeType& other) const;

    // -----------------------------------------------------
    //     Debug and Dump
    // -----------------------------------------------------

    bool Validate() const;

    std::string Dump() const;
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
