#ifndef GENESIS_TREE_TREE_EDGE_H_
#define GENESIS_TREE_TREE_EDGE_H_

/**
 * @brief
 *
 * For more information, see TreeEdge.
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
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

template <class NodeDataType, class EdgeDataType>
class  TreeNode;

// =================================================================================================
//     TreeEdge
// =================================================================================================

template <class NodeDataType, class EdgeDataType>
class TreeEdge
{
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

    TreeEdge()
        : index_( 0 )
        , link_p_( nullptr )
        , link_s_( nullptr )
    {}

    TreeEdge( size_t index, LinkType* primary_link, LinkType* secondary_link )
        : index_(  index )
        , link_p_( primary_link )
        , link_s_( secondary_link )
    {}

    ~TreeEdge() = default;

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.

    TreeEdge( TreeEdge const& ) = delete;
    TreeEdge( TreeEdge&& )      = delete;

    TreeEdge& operator= ( TreeEdge const& ) = delete;
    TreeEdge& operator= ( TreeEdge&& )      = delete;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    size_t index() const;

    LinkType      & primary_link();
    LinkType const& primary_link() const;

    LinkType      & secondary_link();
    LinkType const& secondary_link() const;

    NodeType      & primary_node();
    NodeType const& primary_node() const;

    NodeType      & secondary_node();
    NodeType const& secondary_node() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeEdge& reset_index( size_t val );

    TreeEdge& reset_primary_link(   LinkType* val );
    TreeEdge& reset_secondary_link( LinkType* val );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

public:

    EdgeDataType data;

private:

    size_t       index_;

    LinkType*    link_p_;
    LinkType*    link_s_;
};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the Implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_edge.tpp"

#endif // include guard
