#ifndef GENESIS_PLACEMENT_IO_EDGE_COLOR_H_
#define GENESIS_PLACEMENT_IO_EDGE_COLOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace color {
    class Color;
}

template<class NDT, class EDT>
class Tree;

class PlacementTreeNodeData;
class PlacementTreeEdgeData;

typedef Tree<PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTree;

// =================================================================================================
//     Placement Edge Color Functions
// =================================================================================================

std::vector<color::Color> placement_color_count_gradient( PlacementTree const& tree, bool linear = false );

} // namespace genesis

#endif // include guard
