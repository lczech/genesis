/**
 * @brief Implementation of Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// =================================================================================================
//     Find
// =================================================================================================

template <class TreeType>
typename TreeType::NodeType* find_node(const TreeType& tree, const std::string& name)
{
    auto clean_name = string_replace_all(name, "_", " ");

    // TODO introduce wrapper class for range iterating...
    // for (const auto& n : tree.nodes()) {
    for (auto it = tree.begin_nodes(); it != tree.end_nodes(); ++it) {
        if ((*it)->data.name == clean_name) {
            return it->get();
        }
    }

    return nullptr;
}

} // namespace genesis
