#ifndef GENESIS_PLACEMENT_FUNCTION_HELPER_H_
#define GENESIS_PLACEMENT_FUNCTION_HELPER_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample.hpp"

#include <unordered_map>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Helper Functions
// =================================================================================================

std::unordered_map<int, PlacementTree::EdgeType*> edge_num_to_edge_map( PlacementTree const& tree );
std::unordered_map<int, PlacementTree::EdgeType*> edge_num_to_edge_map( Sample const & smp );

std::unordered_map< size_t, std::vector< PqueryPlacement const* >> placements_per_edge(
    Sample const& smp
);

std::vector<PqueryPlacement const*> placements_per_edge(
    Sample            const& smp,
    PlacementTreeEdge const& edge
);

std::vector<PqueryPlain> plain_queries( Sample const & smp );

} // namespace placement
} // namespace genesis

#endif // include guard
