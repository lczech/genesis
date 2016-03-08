/**
 * @brief Implementation of Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "utils/text/string.hpp"

namespace genesis {

// =================================================================================================
//     Node Names
// =================================================================================================

/**
 * @brief Returns a list of all node names of a Tree.
 *
 * If `leaves_only` is set to true, nodes names of inner nodes are not included.
 * Unnamed nodes (`node.data.name == ""`) are always excluded.
 */
template <class TreeType>
std::vector<std::string> node_names(
    TreeType& tree,
    bool leaves_only
) {
    std::vector<std::string> ret;
    for (auto it = tree.begin_nodes(); it != tree.end_nodes(); ++it) {
        if ( (*it)->is_inner() && leaves_only ) {
            continue;
        }
        if ( (*it)->data.name == "" ) {
            continue;
        }
        ret.push_back( (*it)->data.name );
    }
    return ret;
}

/**
 * @brief Finds a Node, given its name. If not found, nullptr is returned.
 */
template <class TreeType>
typename TreeType::NodeType* find_node(
    TreeType& tree,
    const std::string& name,
    bool replace_underscores
) {
    auto clean_name = name;
    if (replace_underscores) {
        clean_name = utils::replace_all(name, "_", " ");
    }

    for (auto it = tree.begin_nodes(); it != tree.end_nodes(); ++it) {
        if ((*it)->data.name == clean_name) {
            return it->get();
        }
    }

    return nullptr;
}

} // namespace genesis
