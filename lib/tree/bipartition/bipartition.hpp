#ifndef GENESIS_TREE_BIPARTITION_BIPARTITION_H_
#define GENESIS_TREE_BIPARTITION_BIPARTITION_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_edge.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "utils/math/bitvector.hpp"

namespace genesis {
namespace tree {

// =============================================================================
//     Forward Declarations
// =============================================================================

template <typename Tree>
class BipartitionSet;

// =============================================================================
//     Bipartition
// =============================================================================

template <typename Tree>
class Bipartition
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    typedef BipartitionSet<Tree> BipartitionSetType;
    friend  BipartitionSetType;

    typedef Tree                    TreeType;
    typedef typename Tree::NodeType NodeType;
    typedef typename Tree::LinkType LinkType;
    typedef typename Tree::EdgeType EdgeType;

    Bipartition (size_t num_leaves)
        : leaf_nodes_( utils::Bitvector(num_leaves) )
        , link_(nullptr)
    {}

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    LinkType* link()
    {
        return link_;
    }

    void invert()
    {
        leaf_nodes_.invert();
        link_ = link_->outer();
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    utils::Bitvector leaf_nodes_;
    LinkType*        link_;

};

} // namespace tree
} // namespace genesis

#endif // include guard
