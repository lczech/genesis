#ifndef GENESIS_PLACEMENT_FUNCTION_MEASURES_H_
#define GENESIS_PLACEMENT_FUNCTION_MEASURES_H_

/**
 * @brief Header for Placement Measures functions.
 *
 * @file
 * @ingroup placement
 */

#include <utility>
#include <vector>

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
    class Sample;
    class SampleSet;
    class PlacementTreeEdgeData;
    class PlacementTreeNodeData;
    struct PqueryPlain;

    typedef tree::TreeNode< PlacementTreeNodeData, PlacementTreeEdgeData > PlacementTreeNode;
    typedef tree::TreeEdge< PlacementTreeNodeData, PlacementTreeEdgeData > PlacementTreeEdge;
}

namespace utils {
    template<typename T>
    class Matrix;

    class Histogram;
}

// =================================================================================================
//     Placement Measures
// =================================================================================================

namespace placement {

double pquery_distance (
    const PqueryPlain&     pqry_a,
    const PqueryPlain&     pqry_b,
    const utils::Matrix<double>&  node_distances,
    bool                   with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Earth Movers Distance
// ----------------------------------------------------------------------------

double earth_movers_distance (
    const Sample& map_a,
    const Sample& map_b,
    bool                with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Center of Gravity
// ----------------------------------------------------------------------------

std::pair<PlacementTreeEdge*, double> center_of_gravity (
    const Sample& map,
    bool                with_pendant_length = false
);

double center_of_gravity_variance (
    const Sample& map,
    bool                with_pendant_length = false
);

double center_of_gravity_distance (
    const Sample& map_a,
    const Sample& map_b,
    bool                with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Pairwise Distance
// ----------------------------------------------------------------------------

double pairwise_distance (
    const Sample& map_a,
    const Sample& map_b,
    bool                with_pendant_length = false
);

// double closest_pair_distance (
//     const Sample& map_a,
//     const Sample& map_b,
//     bool                with_pendant_length = false
// );

// ----------------------------------------------------------------------------
//     Node-Based Distances
// ----------------------------------------------------------------------------

utils::Histogram node_distance_histogram (
    // const Sample& map,
    const PlacementTreeNode& node,
    bool                with_pendant_length = false
);

std::vector< utils::Histogram > node_distance_histograms (
    const Sample& map,
    bool                with_pendant_length = false
);

double node_histogram_distance (
    const Sample& map_a,
    const Sample& map_b,
    bool                with_pendant_length = false
);

utils::Matrix<double> node_histogram_distance_matrix (
    const SampleSet& maps,
    bool                   with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Variance
// ----------------------------------------------------------------------------

double variance (
    const Sample& map,
    bool                with_pendant_length = false
);

} // namespace placement
} // namespace genesis

#endif // include guard
