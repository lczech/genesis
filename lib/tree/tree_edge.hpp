#ifndef GENESIS_TREE_TREE_EDGE_H_
#define GENESIS_TREE_TREE_EDGE_H_

/**
 * @brief This class represents an edge inside of a tree.
 *
 * For more information, see TreeEdge class.
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/newick_processor.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

template <class NodeDataType, class EdgeDataType>
class  TreeNode;

// =============================================================================
//     TreeEdge
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeEdge
{
    friend class Tree<NodeDataType, EdgeDataType>;

public:
    TreeEdge() : link_p_(nullptr), link_s_(nullptr) {}

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.
    TreeEdge (const TreeEdge&) = delete;
    TreeEdge& operator = (const TreeEdge&) = delete;

    // ---------------------------------------------------------------------
    //     Typedefs
    // ---------------------------------------------------------------------

    typedef Tree    <NodeDataType, EdgeDataType> TreeType;
    typedef TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode<NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge<NodeDataType, EdgeDataType> EdgeType;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the link of this edge that points towards the root.
     */
    inline LinkType* primary_link() const
    {
        return link_p_;
    }

    /**
     * @brief Returns the link of this edge that points away from the root.
     */
    inline LinkType* secondary_link() const
    {
        return link_s_;
    }

    NodeType* primary_node() const;
    NodeType* secondary_node() const;

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the index of this Link.
     */
    inline size_t index() const
    {
        return index_;
    }

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    EdgeDataType data;

// TODO !!! make protected again, and use some other mechanism for setting the members !!!
//~ protected:

    size_t index_;

    LinkType* link_p_;
    LinkType* link_s_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_edge.tpp"

#endif // include guard
