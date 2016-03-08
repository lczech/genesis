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

namespace utils {
    class Color;
}

template<class NDT, class EDT>
class Tree;

namespace placement {
    class PlacementTreeNodeData;
    class PlacementTreeEdgeData;
}

typedef Tree< placement::PlacementTreeNodeData, placement::PlacementTreeEdgeData > PlacementTree;

// =================================================================================================
//     Placement Edge Color Functions
// =================================================================================================

namespace placement {

std::vector<utils::Color> placement_color_count_gradient( PlacementTree const& tree, bool linear = false );

} // namespace placement
} // namespace genesis

#endif // include guard
