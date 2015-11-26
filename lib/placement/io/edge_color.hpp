#ifndef GENESIS_PLACEMENT_IO_EDGE_COLOR_H_
#define GENESIS_PLACEMENT_IO_EDGE_COLOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"

#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Color;

// =================================================================================================
//     Placement Edge Color Functions
// =================================================================================================

std::vector<Color> placement_color_count_gradient( PlacementTree const& tree, bool linear = false );

} // namespace genesis

#endif // include guard
