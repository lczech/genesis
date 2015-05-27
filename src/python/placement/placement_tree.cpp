/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/placement_tree.hpp"

#include "../src/python/tree/tree.hpp"

void BoostPythonExport_PlacementTree()
{
    using namespace genesis;

    BoostPythonExport_Tree<PlacementTreeNodeData, PlacementTreeEdgeData>("PlacementTree");
}
