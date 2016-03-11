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

namespace tree {
    template<class NodeDataType, class EdgeDataType>
    class Tree;
}

namespace placement {
    class PlacementTreeNodeData;
    class PlacementTreeEdgeData;

    typedef tree::Tree< PlacementTreeNodeData, PlacementTreeEdgeData > PlacementTree;
}

// =================================================================================================
//     Placement Edge Color Functions
// =================================================================================================

namespace placement {

std::vector<utils::Color> placement_color_count_gradient( PlacementTree const& tree, bool linear = false );

} // namespace placement
} // namespace genesis

#endif // include guard
