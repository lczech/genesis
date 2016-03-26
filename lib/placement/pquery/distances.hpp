#ifndef GENESIS_PLACEMENT_PQUERY_DISTANCES_H_
#define GENESIS_PLACEMENT_PQUERY_DISTANCES_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {
    template<class NodeDataType, class EdgeDataType>
    class TreeNode;

    template<class NodeDataType, class EdgeDataType>
    class TreeEdge;
}

namespace placement {
    class  Pquery;
    class  PqueryPlacement;
    struct PqueryPlain;

    class  PlacementTreeEdgeData;
    class  PlacementTreeNodeData;

    typedef tree::TreeNode< PlacementTreeNodeData, PlacementTreeEdgeData > PlacementTreeNode;
    typedef tree::TreeEdge< PlacementTreeNodeData, PlacementTreeEdgeData > PlacementTreeEdge;
}

namespace utils {
    template <typename T>
    class Matrix;
}

// =================================================================================================
//     Distance Measuring
// =================================================================================================

namespace placement {
/*
// double distance (
//     const PqueryPlacement& placement_a,
//     const PqueryPlacement& placement_b
// );

double distance (
    const PqueryPlacement& placement_a,
    const PqueryPlacement& placement_b,
    const utils::Matrix<double>& node_distance_matrix
);

double distance (
    const PqueryPlacement& placement,
    const Pquery& pquery,
    const utils::Matrix<double>& node_distance_matrix
);

// double distance (
//     const PqueryPlain& pqry_a,
//     const PqueryPlain& pqry_b
// );

double distance (
    const PqueryPlain& pqry_a,
    const PqueryPlain& pqry_b,
    const utils::Matrix<double>& node_distance_matrix
);

// double distance (
//     const Pquery& pqry_a,
//     const Pquery& pqry_b
// );

double distance (
    const Pquery& pqry_a,
    const Pquery& pqry_b,
    const utils::Matrix<double>& node_distance_matrix
);

double distance (
    const PqueryPlacement& placement,
    const PlacementTreeNode& node,
    const utils::Matrix<double>& node_distance_matrix
);

double distance (
    const Pquery& pquery,
    const PlacementTreeNode& node,
    const utils::Matrix<double>& node_distance_matrix
);

} // namespace placement
} // namespace genesis
*/
#endif // include guard
