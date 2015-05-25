#ifndef GENESIS_TREE_BIPARTITIONSET_H_
#define GENESIS_TREE_BIPARTITIONSET_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <vector>

#include "tree/bipartition.hpp"

namespace genesis {

template <class NodeDataType, class EdgeDataType>
class BipartitionSet
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    typedef Bipartition <NodeDataType, EdgeDataType> BipartitionType;

    typedef Tree        <NodeDataType, EdgeDataType> TreeType;
    typedef TreeLink    <NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode    <NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge    <NodeDataType, EdgeDataType> EdgeType;

    BipartitionSet (const TreeType* tree) : tree_(tree) {};

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    void Make();
    void MakeIndex();

    BipartitionType*             FindSmallestSubtree (std::vector<NodeType*> nodes);
    std::vector<const EdgeType*> GetSubtreeEdges     (const LinkType*        subtree);

    bool        Validate();
    std::string Dump();

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
#include "tree/bipartition_set.tpp"

#endif // include guard
