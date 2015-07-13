#ifndef GENESIS_TREE_DEFAULT_TREE_NEWICK_ADAPTER_H_
#define GENESIS_TREE_DEFAULT_TREE_NEWICK_ADAPTER_H_

/**
 * @brief Header of DefaultTreeNewickAdapter class.
 *
 * @file
 * @ingroup tree
 */

#include "tree/default_tree.hpp"
#include "tree/newick_adapter.hpp"
#include "tree/newick_processor.hpp"

namespace genesis {

// =================================================================================================
//     Default TreeNewick Adapter
// =================================================================================================

/**
 * @brief
 */
template <typename TreeType>
class DefaultTreeNewickAdapter : public NewickAdapter<TreeType>
{
public:

    inline void to_tree_edge (
        NewickBrokerElement* element, typename TreeType::EdgeType& edge
    ) {
        edge.branch_length = element->branch_length;
    }

    inline void to_tree_node (
        NewickBrokerElement* element, typename TreeType::NodeType& node
    ) {
        node.name = element->name;
    }

    inline void from_tree_edge (
        const typename TreeType::EdgeType& edge, NewickBrokerElement* element
    ) {
        NewickAdapter<TreeType>::set_branch_length(edge.branch_length, element);
    }

    inline void from_tree_node (
        const typename TreeType::NodeType& node, NewickBrokerElement* element
    ) {
        NewickAdapter<TreeType>::set_name(node.name, element);
    }

};

// =================================================================================================
//     Typedef
// =================================================================================================

typedef NewickProcessor<DefaultTreeNewickAdapter<DefaultTree>> DefaultTreeNewickProcessor;

} // namespace genesis

#endif // include guard
