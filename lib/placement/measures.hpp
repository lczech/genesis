#ifndef GENESIS_PLACEMENT_MEASURES_H_
#define GENESIS_PLACEMENT_MEASURES_H_

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

template<class NodeDataType, class EdgeDataType>
class TreeNode;

template<class NodeDataType, class EdgeDataType>
class TreeEdge;

template<typename T>
class Matrix;

class Histogram;
class PlacementMap;
class PlacementMapSet;
class PlacementTreeEdgeData;
class PlacementTreeNodeData;
struct PqueryPlain;

typedef TreeNode <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeNode;
typedef TreeEdge <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeEdge;

// =================================================================================================
//     Placement Measures
// =================================================================================================

double pquery_distance (
    const PqueryPlain&     pqry_a,
    const PqueryPlain&     pqry_b,
    const Matrix<double>&  node_distances,
    bool                   with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Earth Movers Distance
// ----------------------------------------------------------------------------

double earth_movers_distance (
    const PlacementMap& map_a,
    const PlacementMap& map_b,
    bool                with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Center of Gravity
// ----------------------------------------------------------------------------

std::pair<PlacementTreeEdge*, double> center_of_gravity (
    const PlacementMap& map,
    bool                with_pendant_length = false
);

double center_of_gravity_variance (
    const PlacementMap& map,
    bool                with_pendant_length = false
);

double center_of_gravity_distance (
    const PlacementMap& map_a,
    const PlacementMap& map_b,
    bool                with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Pairwise Distance
// ----------------------------------------------------------------------------

double pairwise_distance (
    const PlacementMap& map_a,
    const PlacementMap& map_b,
    bool                with_pendant_length = false
);

// double closest_pair_distance (
//     const PlacementMap& map_a,
//     const PlacementMap& map_b,
//     bool                with_pendant_length = false
// );

// ----------------------------------------------------------------------------
//     Node-Based Distances
// ----------------------------------------------------------------------------

Histogram node_distance_histogram (
    // const PlacementMap& map,
    const PlacementTreeNode& node,
    bool                with_pendant_length = false
);

std::vector<Histogram> node_distance_histograms (
    const PlacementMap& map,
    bool                with_pendant_length = false
);

double node_histogram_distance (
    const PlacementMap& map_a,
    const PlacementMap& map_b,
    bool                with_pendant_length = false
);

Matrix<double> node_histogram_distance_matrix (
    const PlacementMapSet& maps,
    bool                   with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Variance
// ----------------------------------------------------------------------------

double variance (
    const PlacementMap& map,
    bool                with_pendant_length = false
);

} // namespace genesis

#endif // include guard
