#ifndef GENESIS_PLACEMENT_PLACEMENT_TREE_NEWICK_ADAPTER_H_
#define GENESIS_PLACEMENT_PLACEMENT_TREE_NEWICK_ADAPTER_H_

/**
 * @brief Header of NewickAdapter class.
 *
 * @file
 * @ingroup placement
 */

#include <string>

#include "placement/placement_map.hpp"
#include "placement/placement_tree.hpp"
#include "tree/default_tree_newick_adapter.hpp"
#include "tree/newick_processor.hpp"

namespace genesis {

// =================================================================================================
//     Placement Tree Newick Adapter
// =================================================================================================

/**
 * @brief
 */
class PlacementTreeNewickAdapter : public DefaultTreeNewickAdapter<PlacementTree>
{
public:

    inline void to_tree_edge (
        NewickBrokerElement* element, typename PlacementTree::EdgeType& edge
    ) {
        DefaultTreeNewickAdapter<PlacementTree>::to_tree_edge(element, edge);

        edge.edge_num      = -1;
        if (element->tags.size() != 1) {
            LOG_WARN << "Edge for element '" << element->name << "' does not contain the single "
                     << "tag value denoting the edge_num for placements.";
            return;
        }
        edge.edge_num = std::stoi(element->tags[0]);
    }

    inline void to_tree_node (
        NewickBrokerElement* element, typename PlacementTree::NodeType& node
    ) {
        DefaultTreeNewickAdapter<PlacementTree>::to_tree_node(element, node);
    }

    inline void from_tree_edge (
        const typename PlacementTree::EdgeType& edge, NewickBrokerElement* element
    ) {
        DefaultTreeNewickAdapter<PlacementTree>::from_tree_edge(edge, element);

        element->comments.push_back(std::to_string(edge.placement_count()));
        element->tags.push_back(std::to_string(edge.edge_num));
    }

    inline void from_tree_node (
        const typename PlacementTree::NodeType& node, NewickBrokerElement* element
    ) {
        DefaultTreeNewickAdapter<PlacementTree>::from_tree_node(node, element);
    }

};

// =================================================================================================
//     Typedef
// =================================================================================================

typedef NewickProcessor<PlacementTreeNewickAdapter> PlacementTreeNewickProcessor;

} // namespace genesis

#endif // include guard
