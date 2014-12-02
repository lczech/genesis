#ifndef GNS_PLACEMENT_PLACEMENTDATA_H_
#define GNS_PLACEMENT_PLACEMENTDATA_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>

#include "tree/tree_broker.hh"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

template <class NodeDataType, class BranchDataType>
class Tree;

// =============================================================================
//     PlacementBranchData
// =============================================================================

class PlacementBranchData
{
public:
    double branch_length;

    inline void FromTreeBrokerNode (TreeBrokerNode* node)
    {
        branch_length = node->branch_length;
    }

    inline void ToTreeBrokerNode (TreeBrokerNode* node) const
    {
        node->branch_length = branch_length;
    }

    inline std::string Dump() const
    {
        return "Length: " + std::to_string(branch_length);
    }
};

// =============================================================================
//     PlacementNodeData
// =============================================================================

class PlacementNodeData
{
public:
    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name;

    inline void FromTreeBrokerNode (TreeBrokerNode* node)
    {
        name = node->name;
    }

    inline void ToTreeBrokerNode (TreeBrokerNode* node) const
    {
        node->name = name;
    }

    inline std::string Dump() const
    {
        return "Name: '" + name + "'";
    }
};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

// let's avoid tedious names!
typedef Tree<PlacementNodeData, PlacementBranchData> PlacementTree;

} // namespace genesis

#endif // include guard
