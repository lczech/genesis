#ifndef GENESIS_PLACEMENT_PLACEMENT_TREE_H_
#define GENESIS_PLACEMENT_PLACEMENT_TREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <vector>

#include "tree/tree.hpp"

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class PqueryPlacement;

// =================================================================================================
//     Placement Tree Node Data
// =================================================================================================

class PlacementTreeNodeData : public DefaultTreeNodeData
{
public:

};

// =================================================================================================
//     Placement Tree Edge Data
// =================================================================================================

class PlacementTreeEdgeData : public DefaultTreeEdgeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementTreeEdgeData &other) const
    {
        // TODO add a comparison of pqueries as well ?! is that good?!
        return other.branch_length == branch_length && other.edge_num == edge_num;
    }

    inline bool operator != (const PlacementTreeEdgeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    size_t placement_count() const;
    double placement_mass() const;

    void sort_placements();

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    inline std::string dump() const
    {
        return "Length: " + std::to_string(branch_length) + "\tEdge Num: " + std::to_string(edge_num);
    }

    int                           edge_num;
    std::vector<PqueryPlacement*> placements;
};

// =================================================================================================
//     Definitions and Typedefs
// =================================================================================================

// let's avoid tedious names!
typedef TreeEdge <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeEdge;
typedef TreeLink <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeLink;
typedef TreeNode <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeNode;

typedef Tree     <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTree;

} // namespace genesis

#endif // include guard
