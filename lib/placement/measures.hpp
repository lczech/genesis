#ifndef GENESIS_PLACEMENT_MEASURES_H_
#define GENESIS_PLACEMENT_MEASURES_H_

/**
 * @brief Header of PlacementMeasures class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"

namespace genesis {

// =================================================================================================
//     PlacementMeasures
// =================================================================================================

/**
 * @brief
 */
class PlacementMeasures
{
public:

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    static bool with_pendant_length;

    // -------------------------------------------------------------------------
    //     Distances
    // -------------------------------------------------------------------------

protected:
    static double pquery_distance (
        const PqueryPlain&     pqry_a,
        const PqueryPlain&     pqry_b,
        const Matrix<double>&  node_distances
    );

public:

    // Earth Movers Distance

    static double earth_movers_distance (
        const PlacementMap& map_a, const PlacementMap& map_b
    );

    // Center of Gravity

    static std::pair<PlacementTreeEdge*, double> center_of_gravity (const PlacementMap& map);

    static double center_of_gravity_variance (const PlacementMap& map);

    static double center_of_gravity_distance (
        const PlacementMap& map_a, const PlacementMap& map_b
    );

    // Pairwise Distance

    static double pairwise_distance (
        const PlacementMap& map_a, const PlacementMap& map_b
    );

    // static double closest_pair_distance (
    //     const PlacementMap& map_a, const PlacementMap& map_b
    // );

    // -------------------------------------------------------------------------
    //     Variance
    // -------------------------------------------------------------------------

public:
    static double variance (const PlacementMap& map);

protected:
    static void variance_thread (
        const int                       offset,
        const int                       incr,
        const std::vector<PqueryPlain>* pqrys,
        const Matrix<double>*           node_distances,
        double*                         partial
    );

    static double variance_partial (
        const PqueryPlain&              place_a,
        const std::vector<PqueryPlain>& pqrys_b,
        const Matrix<double>&           node_distances
    );
};

} // namespace genesis

#endif // include guard
