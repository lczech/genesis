#ifndef GENESIS_TREE_TREE_NODE_H_
#define GENESIS_TREE_TREE_NODE_H_

/**
 * @brief Header of TreeNode class template.
 *
 * For more information, see TreeNode.
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
class  TreeEdge;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

// =================================================================================================
//     TreeNode
// =================================================================================================

template <class NodeDataType, class EdgeDataType>
class TreeNode
{
    friend Tree<NodeDataType, EdgeDataType>;

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

    TreeNode()
        : index_( 0 )
        , link_(  nullptr )
    {}

    TreeNode( size_t index, LinkType* primary_link )
        : index_( index )
        , link_(  primary_link )
    {}

    ~TreeNode() = default;

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.

    TreeNode( TreeNode const& ) = delete;
    TreeNode( TreeNode&& )      = delete;

    TreeNode& operator= ( TreeNode const& ) = delete;
    TreeNode& operator= ( TreeNode&& )      = delete;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    size_t index() const;

    LinkType      & primary_link();
    LinkType const& primary_link() const;

    LinkType      & link();
    LinkType const& link() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeNode& reset_index( size_t val );

    TreeNode& reset_primary_link( LinkType* val );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    size_t rank() const;
    bool   is_leaf() const;
    bool   is_inner() const;

    //~ /** True if the node is the root, false otherwise. */
    //~ bool IsRoot();

    //~ /** Depth of the node in the tree, i.e. its distance from root. */
    //~ int Depth();

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

public:

    NodeDataType data;

private:

    size_t       index_;
    LinkType*    link_;
};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

#include "tree/tree_node.tpp"

#endif // include guard
