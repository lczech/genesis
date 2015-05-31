#ifndef GENESIS_TREE_TREE_H_
#define GENESIS_TREE_TREE_H_

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
#include <utility>
#include <vector>

#include "tree/default_tree.hpp"
#include "tree/tree_edge.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "utils/matrix.hpp"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class TreeIteratorEulertour;

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class TreeIteratorPreorder;

//~ template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
//~ class TreeIteratorInorder;

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class TreeIteratorPostorder;

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
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
template <class NodeDataType = DefaultTreeNodeData, class EdgeDataType = DefaultTreeEdgeData>
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

    void import_content (LinkArray& links, NodeArray& nodes, EdgeArray& edges);
    void export_content (LinkArray& links, NodeArray& nodes, EdgeArray& edges);

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    inline LinkType* root_link() const
    {
        return links_.front();
    }

    inline NodeType* root_node() const
    {
        return links_.front()->node();
    }

    inline LinkType* link_at(size_t index) const
    {
        return links_[index];
    }

    inline NodeType* node_at(size_t index) const
    {
        return nodes_[index];
    }

    inline EdgeType* edge_at(size_t index) const
    {
        return edges_[index];
    }

    /**
     * @brief Returns the number of Links of the Tree.
     */
    inline size_t link_count() const
    {
        return links_.size();
    }

    /**
     * @brief Returns the number of Nodes of the Tree.
     */
    inline size_t node_count() const
    {
        return nodes_.size();
    }

    /**
     * @brief Returns the number of Edges of the Tree.
     */
    inline size_t edge_count() const
    {
        return edges_.size();
    }

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

    typedef TreeIteratorEulertour <      LinkType*,       NodeType*,       EdgeType*>      IteratorEulertour;
    typedef TreeIteratorEulertour <const LinkType*, const NodeType*, const EdgeType*> ConstIteratorEulertour;

    typedef TreeIteratorPreorder  <      LinkType*,       NodeType*,       EdgeType*>      IteratorPreorder;
    typedef TreeIteratorPreorder  <const LinkType*, const NodeType*, const EdgeType*> ConstIteratorPreorder;

    //~ typedef TreeIteratorInorder   <      LinkType*,       NodeType*,       EdgeType*>      IteratorInorder;
    //~ typedef TreeIteratorInorder   <const LinkType*, const NodeType*, const EdgeType*> ConstIteratorInorder;

    typedef TreeIteratorPostorder <      LinkType*,       NodeType*,       EdgeType*>      IteratorPostorder;
    typedef TreeIteratorPostorder <const LinkType*, const NodeType*, const EdgeType*> ConstIteratorPostorder;

    typedef TreeIteratorLevelorder<      LinkType*,       NodeType*,       EdgeType*>      IteratorLevelorder;
    typedef TreeIteratorLevelorder<const LinkType*, const NodeType*, const EdgeType*> ConstIteratorLevelorder;

    typedef typename std::vector<LinkType*>::iterator               IteratorLinks;
    typedef typename std::vector<LinkType*>::const_iterator    ConstIteratorLinks;

    typedef typename std::vector<NodeType*>::iterator               IteratorNodes;
    typedef typename std::vector<NodeType*>::const_iterator    ConstIteratorNodes;

    typedef typename std::vector<EdgeType*>::iterator               IteratorEdges;
    typedef typename std::vector<EdgeType*>::const_iterator    ConstIteratorEdges;

    // TODO so far, the End... iterators are called anew for every comparison in a loop like
    // TODO it != tree.end_inorder(), which will slow it down compared to having e.g. a fixed
    // TODO end iterator object or so... not sure, if worth the effort.

    // -----------------------
    //     Eulertour
    // -----------------------

    inline IteratorEulertour begin_eulertour()
    {
        return IteratorEulertour(root_link());
    }

    inline IteratorEulertour begin_eulertour(const LinkType* link)
    {
        return IteratorEulertour(link);
    }

    inline IteratorEulertour begin_eulertour(const NodeType* node)
    {
        return IteratorEulertour(node->primary_link());
    }

    inline IteratorEulertour end_eulertour()
    {
        return IteratorEulertour(nullptr);
    }

    inline ConstIteratorEulertour begin_eulertour() const
    {
        return ConstIteratorEulertour(root_link());
    }

    inline ConstIteratorEulertour begin_eulertour(const LinkType* link) const
    {
        return ConstIteratorEulertour(link);
    }

    inline ConstIteratorEulertour begin_eulertour(const NodeType* node) const
    {
        return ConstIteratorEulertour(node->primary_link());
    }

    inline ConstIteratorEulertour end_eulertour() const
    {
        return ConstIteratorEulertour(nullptr);
    }

    // -----------------------
    //     Preorder
    // -----------------------

    inline IteratorPreorder begin_preorder()
    {
        return IteratorPreorder(root_link());
    }

    inline IteratorPreorder begin_preorder(const LinkType* link)
    {
        return IteratorPreorder(link);
    }

    inline IteratorPreorder begin_preorder(const NodeType* node)
    {
        return IteratorPreorder(node->primary_link());
    }

    inline IteratorPreorder end_preorder()
    {
        return IteratorPreorder(nullptr);
    }

    inline ConstIteratorPreorder begin_preorder() const
    {
        return ConstIteratorPreorder(root_link());
    }

    inline ConstIteratorPreorder begin_preorder(const LinkType* link) const
    {
        return ConstIteratorPreorder(link);
    }

    inline ConstIteratorPreorder begin_preorder(const NodeType* node) const
    {
        return ConstIteratorPreorder(node->primary_link());
    }

    inline ConstIteratorPreorder end_preorder() const
    {
        return ConstIteratorPreorder(nullptr);
    }

    // -----------------------
    //     Inorder
    // -----------------------

    //~ inline IteratorInorder begin_inorder()
    //~ {
        //~ return IteratorInorder(root_link());
    //~ }
//~
    //~ inline IteratorInorder begin_inorder(const LinkType* link)
    //~ {
        //~ return IteratorInorder(link);
    //~ }
//~
    //~ inline IteratorInorder begin_inorder(const NodeType* node)
    //~ {
        //~ return IteratorInorder(node->primary_link());
    //~ }
//~
    //~ inline IteratorInorder end_inorder()
    //~ {
        //~ return IteratorInorder(nullptr);
    //~ }

    //~ inline ConstIteratorInorder begin_inorder() const
    //~ {
        //~ return ConstIteratorInorder(root_link());
    //~ }
//~
    //~ inline ConstIteratorInorder begin_inorder(const LinkType* link) const
    //~ {
        //~ return ConstIteratorInorder(link);
    //~ }
//~
    //~ inline ConstIteratorInorder begin_inorder(const NodeType* node) const
    //~ {
        //~ return ConstIteratorInorder(node->primary_link());
    //~ }
//~
    //~ inline ConstIteratorInorder end_inorder() const
    //~ {
        //~ return ConstIteratorInorder(nullptr);
    //~ }

    // -----------------------
    //     Postorder
    // -----------------------

    inline IteratorPostorder begin_postorder()
    {
        return IteratorPostorder(root_link());
    }

    inline IteratorPostorder begin_postorder(const LinkType* link)
    {
        return IteratorPostorder(link);
    }

    inline IteratorPostorder begin_postorder(const NodeType* node)
    {
        return IteratorPostorder(node->primary_link());
    }

    inline IteratorPostorder end_postorder()
    {
        return IteratorPostorder(nullptr);
    }

    inline ConstIteratorPostorder begin_postorder() const
    {
        return ConstIteratorPostorder(root_link());
    }

    inline ConstIteratorPostorder begin_postorder(const LinkType* link) const
    {
        return ConstIteratorPostorder(link);
    }

    inline ConstIteratorPostorder begin_postorder(const NodeType* node) const
    {
        return ConstIteratorPostorder(node->primary_link());
    }

    inline ConstIteratorPostorder end_postorder() const
    {
        return ConstIteratorPostorder(nullptr);
    }

    // -----------------------
    //     Levelorder
    // -----------------------

    inline IteratorLevelorder begin_levelorder()
    {
        return IteratorLevelorder(root_link());
    }

    inline IteratorLevelorder begin_levelorder(const LinkType* link)
    {
        return IteratorLevelorder(link);
    }

    inline IteratorLevelorder begin_levelorder(const NodeType* node)
    {
        return IteratorLevelorder(node->primary_link());
    }

    inline IteratorLevelorder end_levelorder()
    {
        return IteratorLevelorder(nullptr);
    }

    inline ConstIteratorLevelorder begin_levelorder() const
    {
        return ConstIteratorLevelorder(root_link());
    }

    inline ConstIteratorLevelorder begin_levelorder(const LinkType* link) const
    {
        return ConstIteratorLevelorder(link);
    }

    inline ConstIteratorLevelorder begin_levelorder(const NodeType* node) const
    {
        return ConstIteratorLevelorder(node->primary_link());
    }

    inline ConstIteratorLevelorder end_levelorder() const
    {
        return ConstIteratorLevelorder(nullptr);
    }

    // -----------------------
    //     Links
    // -----------------------

    inline IteratorLinks begin_links()
    {
        return links_.begin();
    }

    inline IteratorLinks end_links()
    {
        return links_.end();
    }

    inline ConstIteratorLinks begin_links() const
    {
        return links_.cbegin();
    }

    inline ConstIteratorLinks end_links() const
    {
        return links_.cend();
    }

    // -----------------------
    //     Nodes
    // -----------------------

    inline IteratorNodes begin_nodes()
    {
        return nodes_.begin();
    }

    inline IteratorNodes end_nodes()
    {
        return nodes_.end();
    }

    inline ConstIteratorNodes begin_nodes() const
    {
        return nodes_.cbegin();
    }

    inline ConstIteratorNodes end_nodes() const
    {
        return nodes_.cend();
    }

    // -----------------------
    //     Edges
    // -----------------------

    inline IteratorEdges begin_edges()
    {
        return edges_.begin();
    }

    inline IteratorEdges end_edges()
    {
        return edges_.end();
    }

    inline ConstIteratorEdges begin_edges() const
    {
        return edges_.cbegin();
    }

    inline ConstIteratorEdges end_edges() const
    {
        return edges_.cend();
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    // TODO add other interesting member functions: http://en.wikipedia.org/wiki/Tree_%28data_structure%29

    NodeType* find_node(std::string name) const;

    int    max_rank() const;
    bool   is_bifurcating() const;

    size_t leaf_count() const;
    size_t inner_count() const;

    // -----------------------
    //     Distances
    // -----------------------

    double length() const;

    Matrix<int>*        node_depth_matrix    ()                               const;
    std::vector<int>    node_depth_vector    (const NodeType* node = nullptr) const;
    Matrix<double>*     node_distance_matrix ()                               const;
    std::vector<double> node_distance_vector (const NodeType* node = nullptr) const;

    typedef std::vector< std::pair<const NodeType*, int> >    NodeIntVectorType;
    typedef std::vector< std::pair<const NodeType*, double> > NodeDoubleVectorType;

    NodeIntVectorType    closest_leaf_depth_vector()    const;
    NodeDoubleVectorType closest_leaf_distance_vector() const;

    double deepest_distance() const;

    // -----------------------
    //     Comparisons
    // -----------------------

    static bool Equal(
        const TreeType& lhs,
        const TreeType& rhs,
        const std::function<bool
            (TreeType::ConstIteratorPreorder&, TreeType::ConstIteratorPreorder&)> comparator
    );
    bool Equal(
        const TreeType& other,
        const std::function<bool
            (TreeType::ConstIteratorPreorder&, TreeType::ConstIteratorPreorder&)> comparator
    ) const;

    bool HasIdenticalTopology(const TreeType& other) const;

    // -----------------------------------------------------
    //     Debug and Dump
    // -----------------------------------------------------

    bool Validate() const;

    std::string Dump() const;
    std::string DumpLists() const;

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

protected:

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
