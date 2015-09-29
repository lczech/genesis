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

    inline bool to_tree_edge (
        NewickBrokerElement* element, typename TreeType::EdgeType& edge
    ) {
        edge.data.branch_length = element->branch_length;
        return true;
    }

    inline bool to_tree_node (
        NewickBrokerElement* element, typename TreeType::NodeType& node
    ) {
        node.data.name = element->name;
        return true;
    }

    inline void from_tree_edge (
        const typename TreeType::EdgeType& edge, NewickBrokerElement* element
    ) {
        NewickAdapter<TreeType>::set_branch_length(edge.data.branch_length, element);
    }

    inline void from_tree_node (
        const typename TreeType::NodeType& node, NewickBrokerElement* element
    ) {
        NewickAdapter<TreeType>::set_name(node.data.name, element);
    }

};

// =================================================================================================
//     Typedef
// =================================================================================================

typedef NewickProcessor<DefaultTreeNewickAdapter<DefaultTree>> DefaultTreeNewickProcessor;

} // namespace genesis

#endif // include guard
