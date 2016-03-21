#ifndef GENESIS_TREE_TREE_LINK_H_
#define GENESIS_TREE_TREE_LINK_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

namespace genesis {
namespace tree {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class Tree;

template <class NodeDataType, class EdgeDataType>
class TreeEdge;

template <class NodeDataType, class EdgeDataType>
class TreeNode;

// =============================================================================
//     TreeLink
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeLink
{
    friend class Tree<NodeDataType, EdgeDataType>;

public:

    // ---------------------------------------------------------------------
    //     Typedefs
    // ---------------------------------------------------------------------

    typedef Tree    <NodeDataType, EdgeDataType> TreeType;
    typedef TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode<NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge<NodeDataType, EdgeDataType> EdgeType;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TreeLink()
        : next_(  nullptr )
        , outer_( nullptr )
        , node_(  nullptr )
        , edge_(  nullptr )
    {}

    ~TreeLink() = default;

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.

    TreeLink( TreeLink const& ) = delete;
    TreeLink( TreeLink&& )      = delete;

    TreeLink& operator= ( TreeLink const& ) = delete;
    TreeLink& operator= ( TreeLink&& )      = delete;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    LinkType      & next();
    LinkType const& next() const;

    LinkType      & prev();
    LinkType const& prev() const;

    LinkType      & outer();
    LinkType const& outer() const;

    EdgeType      & edge();
    EdgeType const& edge() const;

    NodeType      & node();
    NodeType const& node() const;

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the index of this Link.
     */
    size_t index() const
    {
        return index_;
    }

    /**
     * @brief Returns true iff the node of this link is a leaf node.
     */
    bool is_leaf() const
    {
        return next_ == this;
    }

    /**
     * @brief Returns true iff the node of this link is an inner node.
     */
    bool is_inner() const
    {
        return next_ != this;
    }

    /**
     * @brief Returns a string containing dump information about this link.
     *
     * At the moment, a link does not contain any information, so an empty string is returned.
     * This might change in the future, in case links also contain data.
     */
    std::string dump() const
    {
        return "";
    }

// TODO !!! make protected again, and use some other mechanism for setting the members !!!
//~ protected:

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    size_t index_;

    LinkType* next_;
    LinkType* outer_;

    NodeType* node_;
    EdgeType* edge_;
};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the Implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_link.tpp"

#endif // include guard
