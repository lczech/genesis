#ifndef GENESIS_TREE_BIPARTITION_BIPARTITION_SET_H_
#define GENESIS_TREE_BIPARTITION_BIPARTITION_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <vector>

#include "tree/bipartition/bipartition.hpp"

namespace genesis {

template <typename Tree>
class BipartitionSet
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    typedef Bipartition <Tree> BipartitionType;

    typedef Tree                    TreeType;
    typedef typename Tree::NodeType NodeType;
    typedef typename Tree::LinkType LinkType;
    typedef typename Tree::EdgeType EdgeType;

    BipartitionSet (const TreeType* tree) : tree_(tree) {};

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    void make();
    void make_index();

    BipartitionType*             find_smallest_subtree (std::vector<NodeType*> nodes);
    std::vector<const EdgeType*> get_subtree_edges     (const LinkType*        subtree);

    bool        validate();
    std::string dump();

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    const TreeType*              tree_;

    std::vector<int>             node_to_leaf_map_;
    std::vector<size_t>          leaf_to_node_map_;

    std::vector<BipartitionType> bipartitions_;

};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/bipartition/bipartition_set.tpp"

#endif // include guard
