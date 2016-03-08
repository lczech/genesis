#ifndef GENESIS_PLACEMENT_PQUERY_PLAIN_H_
#define GENESIS_PLACEMENT_PQUERY_PLAIN_H_

/**
 * @brief Header of PqueryPlain class.
 *
 * @file
 * @ingroup placement
 */

#include <cstddef>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Pquery Placement Plain
// =================================================================================================

/**
 * @brief Simple POD struct for a Placement used for speeding up some calculations.
 *
 * It is not as flexible as the default representation, but its memory footprint is compact, because
 * of the lack of pointers.
 */
struct PqueryPlacementPlain
{
    size_t edge_index;
    size_t primary_node_index;
    size_t secondary_node_index;

    double branch_length;
    double pendant_length;
    double proximal_length;
    double like_weight_ratio;
};

// =================================================================================================
//     Pquery Plain
// =================================================================================================

/**
 * @brief
 */
struct PqueryPlain
{
    size_t                            index;
    std::vector<PqueryPlacementPlain> placements;
};

} // namespace placement
} // namespace genesis

#endif // include guard
