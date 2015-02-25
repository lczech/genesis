#ifndef GNS_TREE_BIPARTITIONS_H_
#define GNS_TREE_BIPARTITIONS_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <vector>

#include "tree/tree_edge.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "utils/bitvector.hpp"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class Bipartitions;

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

    typedef Bipartitions<NodeDataType, EdgeDataType> BipartitionsType;
    typedef TreeLink    <NodeDataType, EdgeDataType> LinkType;

    friend BipartitionsType;

    Bipartition (size_t num_leaves) : leaf_nodes_(Bitvector(num_leaves)), link_(nullptr) {};

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    inline void Invert()
    {
        leaf_nodes_.Invert();
        link_ = link_->Outer();
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    Bitvector       leaf_nodes_;
    const LinkType* link_;

};

// =============================================================================
//     Bipartitions
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class Bipartitions
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    typedef Bipartition<NodeDataType, EdgeDataType> BipartitionType;
    typedef Tree<NodeDataType, EdgeDataType>        TreeType;

    Bipartitions (const TreeType* tree) : tree_(tree) {};

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    void Make();
    void MakeIndex();

    bool        Validate();
    std::string Dump();

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    const TreeType* tree_;

    std::vector<int>    node_to_leaf_map_;
    std::vector<size_t> leaf_to_node_map_;

    std::vector<BipartitionType> bipartitions_;

};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/bipartitions.tpp"

#endif // include guard
