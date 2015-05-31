#ifndef GENESIS_TREE_BIPARTITION_H_
#define GENESIS_TREE_BIPARTITION_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_edge.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "utils/bitvector.hpp"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class BipartitionSet;

// =============================================================================
//     Bipartition
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class Bipartition
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    typedef BipartitionSet<NodeDataType, EdgeDataType> BipartitionSetType;
    friend  BipartitionSetType;

    typedef Tree     <NodeDataType, EdgeDataType> TreeType;
    typedef TreeLink <NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode <NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge <NodeDataType, EdgeDataType> EdgeType;

    Bipartition (size_t num_leaves) : leaf_nodes_(Bitvector(num_leaves)), link_(nullptr) {};

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    inline const LinkType* link()
    {
        return link_;
    }

    inline void invert()
    {
        leaf_nodes_.Invert();
        link_ = link_->outer();
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    Bitvector       leaf_nodes_;
    const LinkType* link_;

};

} // namespace genesis

#endif // include guard
