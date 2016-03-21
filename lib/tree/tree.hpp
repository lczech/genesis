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

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "tree/default/tree.hpp"
#include "tree/tree_edge.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

template <class TreeType>
bool validate( TreeType const& tree );

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

// =================================================================================================
//     Tree
// =================================================================================================

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
 *     integers stored in those elements: `nodes_[i]->index_ == i`.
 *  *  The link that is stored in a node has to be the one pointing towards the root.
 *  *  The primary link of an edge has to point towards the root, the secondary away from it.
 */
template <class NodeDataType = DefaultTreeNodeData, class EdgeDataType = DefaultTreeEdgeData>
class Tree
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------------

    typedef Tree    <NodeDataType, EdgeDataType> TreeType;
    typedef TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode<NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge<NodeDataType, EdgeDataType> EdgeType;

    typedef std::vector<std::unique_ptr<LinkType>> LinkContainer;
    typedef std::vector<std::unique_ptr<NodeType>> NodeContainer;
    typedef std::vector<std::unique_ptr<EdgeType>> EdgeContainer;

    // -------------------------------------------------------------------------
    //     Construction and Destruction
    // -------------------------------------------------------------------------

    Tree () {};

    Tree (const TreeType& other);
    TreeType& operator = (const TreeType& other);

    virtual ~Tree() = default;
    void clear();

    void swap (TreeType& other);

    void import_content (LinkContainer& links, NodeContainer& nodes, EdgeContainer& edges);
    void export_content (LinkContainer& links, NodeContainer& nodes, EdgeContainer& edges);

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    bool empty() const;

          LinkType* root_link();
    const LinkType* root_link() const;

          NodeType* root_node();
    const NodeType* root_node() const;

          LinkType* link_at(size_t index);
    const LinkType* link_at(size_t index) const;

          NodeType* node_at(size_t index);
    const NodeType* node_at(size_t index) const;

          EdgeType* edge_at(size_t index);
    const EdgeType* edge_at(size_t index) const;

    size_t link_count() const;
    size_t node_count() const;
    size_t edge_count() const;

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    typedef TreeIteratorEulertour <      LinkType,       NodeType,       EdgeType>      IteratorEulertour;
    typedef TreeIteratorEulertour <const LinkType, const NodeType, const EdgeType> ConstIteratorEulertour;

    typedef TreeIteratorPreorder  <      LinkType*,       NodeType*,       EdgeType*>      IteratorPreorder;
    typedef TreeIteratorPreorder  <const LinkType*, const NodeType*, const EdgeType*> ConstIteratorPreorder;

    //~ typedef TreeIteratorInorder   <      LinkType*,       NodeType*,       EdgeType*>      IteratorInorder;
    //~ typedef TreeIteratorInorder   <const LinkType*, const NodeType*, const EdgeType*> ConstIteratorInorder;

    typedef TreeIteratorPostorder <      LinkType*,       NodeType*,       EdgeType*>      IteratorPostorder;
    typedef TreeIteratorPostorder <const LinkType*, const NodeType*, const EdgeType*> ConstIteratorPostorder;

    typedef TreeIteratorLevelorder<      LinkType*,       NodeType*,       EdgeType*>      IteratorLevelorder;
    typedef TreeIteratorLevelorder<const LinkType*, const NodeType*, const EdgeType*> ConstIteratorLevelorder;

    typedef typename std::vector<std::unique_ptr<LinkType>>::iterator               IteratorLinks;
    typedef typename std::vector<std::unique_ptr<LinkType>>::const_iterator    ConstIteratorLinks;

    typedef typename std::vector<std::unique_ptr<NodeType>>::iterator               IteratorNodes;
    typedef typename std::vector<std::unique_ptr<NodeType>>::const_iterator    ConstIteratorNodes;

    typedef typename std::vector<std::unique_ptr<EdgeType>>::iterator               IteratorEdges;
    typedef typename std::vector<std::unique_ptr<EdgeType>>::const_iterator    ConstIteratorEdges;

    // TODO so far, the End... iterators are called anew for every comparison in a loop like
    // TODO it != tree.end_inorder(), which will slow it down compared to having e.g. a fixed
    // TODO end iterator object or so... not sure, if worth the effort.

    // -----------------------------------------------------
    //     Eulertour
    // -----------------------------------------------------

    IteratorEulertour begin_eulertour()
    {
        if (link_count() == 0) {
            return IteratorEulertour(nullptr);
            // return IteratorEulertour();
        }
        return IteratorEulertour(root_link());
    }

    IteratorEulertour begin_eulertour(LinkType* link)
    {
        return IteratorEulertour(link);
    }

    IteratorEulertour begin_eulertour(NodeType* node)
    {
        return IteratorEulertour(node->primary_link());
        // return IteratorEulertour(node);
    }

    IteratorEulertour end_eulertour()
    {
        return IteratorEulertour(nullptr);
        // return IteratorEulertour();
    }

    ConstIteratorEulertour begin_eulertour() const
    {
        if (link_count() == 0) {
            return ConstIteratorEulertour(nullptr);
            // return ConstIteratorEulertour();
        }
        return ConstIteratorEulertour(root_link());
    }

    ConstIteratorEulertour begin_eulertour(const LinkType* link) const
    {
        return ConstIteratorEulertour(link);
    }

    ConstIteratorEulertour begin_eulertour(const NodeType* node) const
    {
        return ConstIteratorEulertour(node->primary_link());
        // return ConstIteratorEulertour(node);
    }

    ConstIteratorEulertour end_eulertour() const
    {
        return ConstIteratorEulertour(nullptr);
        // return ConstIteratorEulertour();
    }

    // -----------------------------------------------------
    //     Preorder
    // -----------------------------------------------------

    IteratorPreorder begin_preorder()
    {
        if (link_count() == 0) {
            return IteratorPreorder(nullptr);
        }
        return IteratorPreorder(root_link());
    }

    IteratorPreorder begin_preorder(LinkType* link)
    {
        return IteratorPreorder(link);
    }

    IteratorPreorder begin_preorder(NodeType* node)
    {
        return IteratorPreorder(node->primary_link());
    }

    IteratorPreorder end_preorder()
    {
        return IteratorPreorder(nullptr);
    }

    ConstIteratorPreorder begin_preorder() const
    {
        if (link_count() == 0) {
            return ConstIteratorPreorder(nullptr);
        }
        return ConstIteratorPreorder(root_link());
    }

    ConstIteratorPreorder begin_preorder(const LinkType* link) const
    {
        return ConstIteratorPreorder(link);
    }

    ConstIteratorPreorder begin_preorder(const NodeType* node) const
    {
        return ConstIteratorPreorder(node->primary_link());
    }

    ConstIteratorPreorder end_preorder() const
    {
        return ConstIteratorPreorder(nullptr);
    }

    // -----------------------------------------------------
    //     Inorder
    // -----------------------------------------------------

    //~ IteratorInorder begin_inorder()
    //~ {
    //if (link_count() == 0) {
    //     return IteratorInorder(nullptr);
    // }
        //~ return IteratorInorder(root_link());
    //~ }
//~
    //~ IteratorInorder begin_inorder(LinkType* link)
    //~ {
        //~ return IteratorInorder(link);
    //~ }
//~
    //~ IteratorInorder begin_inorder(NodeType* node)
    //~ {
        //~ return IteratorInorder(node->primary_link());
    //~ }
//~
    //~ IteratorInorder end_inorder()
    //~ {
        //~ return IteratorInorder(nullptr);
    //~ }

    //~ ConstIteratorInorder begin_inorder() const
    //~ {
    //if (link_count() == 0) {
    //     return ConstIteratorInorder(nullptr);
    // }
        //~ return ConstIteratorInorder(root_link());
    //~ }
//~
    //~ ConstIteratorInorder begin_inorder(const LinkType* link) const
    //~ {
        //~ return ConstIteratorInorder(link);
    //~ }
//~
    //~ ConstIteratorInorder begin_inorder(const NodeType* node) const
    //~ {
        //~ return ConstIteratorInorder(node->primary_link());
    //~ }
//~
    //~ ConstIteratorInorder end_inorder() const
    //~ {
        //~ return ConstIteratorInorder(nullptr);
    //~ }

    // -----------------------------------------------------
    //     Postorder
    // -----------------------------------------------------

    IteratorPostorder begin_postorder()
    {
        if (link_count() == 0) {
            return IteratorPostorder(nullptr);
        }
        return IteratorPostorder(root_link());
    }

    IteratorPostorder begin_postorder(LinkType* link)
    {
        return IteratorPostorder(link);
    }

    IteratorPostorder begin_postorder(NodeType* node)
    {
        return IteratorPostorder(node->primary_link());
    }

    IteratorPostorder end_postorder()
    {
        return IteratorPostorder(nullptr);
    }

    ConstIteratorPostorder begin_postorder() const
    {
        if (link_count() == 0) {
            return ConstIteratorPostorder(nullptr);
        }
        return ConstIteratorPostorder(root_link());
    }

    ConstIteratorPostorder begin_postorder(const LinkType* link) const
    {
        return ConstIteratorPostorder(link);
    }

    ConstIteratorPostorder begin_postorder(const NodeType* node) const
    {
        return ConstIteratorPostorder(node->primary_link());
    }

    ConstIteratorPostorder end_postorder() const
    {
        return ConstIteratorPostorder(nullptr);
    }

    // -----------------------------------------------------
    //     Levelorder
    // -----------------------------------------------------

    IteratorLevelorder begin_levelorder()
    {
        if (link_count() == 0) {
            return IteratorLevelorder(nullptr);
        }
        return IteratorLevelorder(root_link());
    }

    IteratorLevelorder begin_levelorder(LinkType* link)
    {
        return IteratorLevelorder(link);
    }

    IteratorLevelorder begin_levelorder(NodeType* node)
    {
        return IteratorLevelorder(node->primary_link());
    }

    IteratorLevelorder end_levelorder()
    {
        return IteratorLevelorder(nullptr);
    }

    ConstIteratorLevelorder begin_levelorder() const
    {
        if (link_count() == 0) {
            return ConstIteratorLevelorder(nullptr);
        }
        return ConstIteratorLevelorder(root_link());
    }

    ConstIteratorLevelorder begin_levelorder(const LinkType* link) const
    {
        return ConstIteratorLevelorder(link);
    }

    ConstIteratorLevelorder begin_levelorder(const NodeType* node) const
    {
        return ConstIteratorLevelorder(node->primary_link());
    }

    ConstIteratorLevelorder end_levelorder() const
    {
        return ConstIteratorLevelorder(nullptr);
    }

    // -----------------------------------------------------
    //     Links
    // -----------------------------------------------------

    IteratorLinks begin_links()
    {
        return links_.begin();
    }

    IteratorLinks end_links()
    {
        return links_.end();
    }

    ConstIteratorLinks begin_links() const
    {
        return links_.cbegin();
    }

    ConstIteratorLinks end_links() const
    {
        return links_.cend();
    }

    // -----------------------------------------------------
    //     Nodes
    // -----------------------------------------------------

    IteratorNodes begin_nodes()
    {
        return nodes_.begin();
    }

    IteratorNodes end_nodes()
    {
        return nodes_.end();
    }

    ConstIteratorNodes begin_nodes() const
    {
        return nodes_.cbegin();
    }

    ConstIteratorNodes end_nodes() const
    {
        return nodes_.cend();
    }

    // -----------------------------------------------------
    //     Edges
    // -----------------------------------------------------

    IteratorEdges begin_edges()
    {
        return edges_.begin();
    }

    IteratorEdges end_edges()
    {
        return edges_.end();
    }

    ConstIteratorEdges begin_edges() const
    {
        return edges_.cbegin();
    }

    ConstIteratorEdges end_edges() const
    {
        return edges_.cend();
    }

    // -------------------------------------------------------------------------
    //     Debug and Dump
    // -------------------------------------------------------------------------

    friend std::ostream& operator << ( std::ostream& out, TreeType const& tree )
    {
        out << "Node Count: " << tree.node_count() << "\n";
        out << "Edge Count: " << tree.edge_count() << "\n";
        out << "Link Count: " << tree.link_count() << "\n";
        return out;
    }

    /**
     * @brief Validate the correctness of all Tree pointers etc.
     *
     * This function is defined in tree/operators/functions.hpp
     */
    friend bool validate<TreeType>( TreeType const& tree );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<std::unique_ptr<LinkType>> links_;
    std::vector<std::unique_ptr<NodeType>> nodes_;
    std::vector<std::unique_ptr<EdgeType>> edges_;
};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree.tpp"

#endif // include guard
