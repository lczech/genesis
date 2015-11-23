#ifndef GENESIS_PLACEMENT_IO_NEWICK_ADAPTER_H_
#define GENESIS_PLACEMENT_IO_NEWICK_ADAPTER_H_

/**
 * @brief Header of NewickAdapter class.
 *
 * @file
 * @ingroup placement
 */

#include <string>

#include "placement/placement_tree.hpp"
#include "tree/default_tree_newick_adapter.hpp"
#include "tree/io/newick_processor.hpp"

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

    inline bool to_tree_edge (
        NewickBrokerElement const& element, typename PlacementTree::EdgeType& edge
    ) {
        if (!DefaultTreeNewickAdapter<PlacementTree>::to_tree_edge(element, edge)) {
            return false;
        }

        edge.data.edge_num      = -1;
        if (element.tags.size() != 1) {
            LOG_WARN << "Edge for element '" << element.name << "' does not contain the single "
                     << "tag value denoting the edge_num for placements.";
            return false;
        }
        edge.data.edge_num = std::stoi(element.tags[0]);
        return true;
    }

    inline bool to_tree_node (
        NewickBrokerElement const& element, typename PlacementTree::NodeType& node
    ) {
        return DefaultTreeNewickAdapter<PlacementTree>::to_tree_node(element, node);
    }

    inline void from_tree_edge (
        const typename PlacementTree::EdgeType& edge, NewickBrokerElement& element
    ) {
        DefaultTreeNewickAdapter<PlacementTree>::from_tree_edge(edge, element);

        element.comments.push_back(std::to_string(edge.data.placement_count()));
        element.tags.push_back(std::to_string(edge.data.edge_num));
    }

    inline void from_tree_node (
        const typename PlacementTree::NodeType& node, NewickBrokerElement& element
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
