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

// =================================================================================================
//     Forward declarations
// =================================================================================================

template <class NodeDataType, class EdgeDataType>
class Tree;

template <class NodeDataType, class EdgeDataType>
class TreeEdge;

template <class NodeDataType, class EdgeDataType>
class TreeNode;

// =================================================================================================
//     TreeLink
// =================================================================================================

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
        : index_( 0       )
        , next_(  nullptr )
        , outer_( nullptr )
        , node_(  nullptr )
        , edge_(  nullptr )
    {}

    TreeLink( size_t index, LinkType* next, LinkType* outer, NodeType* node, EdgeType* edge )
        : index_( index )
        , next_(  next  )
        , outer_( outer )
        , node_(  node  )
        , edge_(  edge  )
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

    size_t index() const;

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
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeLink& reset_index( size_t val );

    TreeLink& reset_next(  LinkType* val );
    TreeLink& reset_outer( LinkType* val );

    TreeLink& reset_node(  NodeType* val );
    TreeLink& reset_edge(  EdgeType* val );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    bool is_leaf() const;
    bool is_inner() const;

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

private:

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
