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

template <typename T>
class Matrix;

class Pquery;
class PqueryPlacement;
class PqueryPlain;

class PlacementTreeNode;

// =================================================================================================
//     Distance Measuring
// =================================================================================================

// double distance (
//     const PqueryPlacement& placement_a,
//     const PqueryPlacement& placement_b
// );

double distance (
    const PqueryPlacement& placement_a,
    const PqueryPlacement& placement_b,
    const Matrix<double>& node_distance_matrix
);

double distance (
    const PqueryPlacement& placement,
    const Pquery& pquery,
    const Matrix<double>& node_distance_matrix
);

// double distance (
//     const PqueryPlain& pqry_a,
//     const PqueryPlain& pqry_b
// );

double distance (
    const PqueryPlain& pqry_a,
    const PqueryPlain& pqry_b,
    const Matrix<double>& node_distance_matrix
);

// double distance (
//     const Pquery& pqry_a,
//     const Pquery& pqry_b
// );

double distance (
    const Pquery& pqry_a,
    const Pquery& pqry_b,
    const Matrix<double>& node_distance_matrix
);

double distance (
    const PqueryPlacement& placement,
    const PlacementTreeNode& node,
    const Matrix<double>& node_distance_matrix
);

double distance (
    const Pquery& pquery,
    const PlacementTreeNode& node,
    const Matrix<double>& node_distance_matrix
);

} // namespace genesis

#endif // include guard
