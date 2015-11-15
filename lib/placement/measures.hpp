#ifndef GENESIS_PLACEMENT_MEASURES_H_
#define GENESIS_PLACEMENT_MEASURES_H_

/**
 * @brief Header for Placement Measures functions.
 *
 * @file
 * @ingroup placement
 */

#include <utility>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

template<class NodeDataType, class EdgeDataType>
class TreeEdge;

template<typename T>
class Matrix;

class PlacementMap;
class PlacementTreeEdgeData;
class PlacementTreeNodeData;
struct PqueryPlain;

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
//     Node Histogram Distance
// ----------------------------------------------------------------------------

double node_histogram_distance (
    const PlacementMap& map_a,
    const PlacementMap& map_b,
    bool                with_pendant_length = false
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
