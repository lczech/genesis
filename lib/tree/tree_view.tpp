/**
 * @brief Implementation of TreeView class.
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>
#include <sstream>
#include <vector>

namespace genesis {

// =============================================================================
//     Viewing Methods
// =============================================================================

/**
 * @brief
 *
 * TODO this method assumes that the tree node has a name. not good.
 */
template <typename TreeType>
std::string TreeView::compact (
    const TreeType& tree,
    const std::function<std::string (typename TreeType::ConstIteratorPreorder& it)> print_line
) {
    std::ostringstream res;

    // Stores a count of how many child nodes each node has left for viewing.
    auto ranks   = std::vector<size_t>(tree.node_count(), 0);

    // Store the current stack of parents while traversing.
    auto parents = std::vector<size_t>();

    for (auto it = tree.begin_preorder(); it != tree.end_preorder(); ++it) {
        // Index of current and its parent node.
        size_t cur_idx = it.node()->index();
        size_t par_idx = it.link()->outer()->node()->index();

        // Set parent stack correctly (including current node), and store current rank.
        while (!parents.empty() && parents.back() != par_idx) {
            parents.pop_back();
        }
        parents.push_back(cur_idx);
        ranks[cur_idx] = it.node()->rank();

        // The root node is special: We have to account for one more child, as it does not have a
        // parent. Also, we do not draw any lines or indention for the root.
        if (it.is_first_iteration()) {
            ++ranks[cur_idx];
            // TODO this should also use the print_line function. current users of this method then need to make sure that they check for the first iteration themselves in case they want to display is specially.
            res << it.node()->data.name << "\n";
            continue;
        }

        // This point in code is reached for all nodes but the root. Thus, we already have at least
        // the root and the current node added to the parents stack. Also, the second but last
        // element will be the parent of the current node (and the last one the node itself).
        assert(parents.size() > 1 && parents[parents.size() - 2] == par_idx);

        // Draw indention lines for all non-immediate parents of the current node. If their rank
        // is zero, there will no other children follow, so do not draw a line then.
        for (size_t i = 0; i < parents.size() - 2; ++i) {
            if (ranks[parents[i]] > 0) {
                res << "│   ";
            } else {
                res << "    ";
            }
        }

        // We are about to draw a child of the parent. Prior to drawing, we need to reduce the
        // parents rank counter. If it then is zero, the current node is the last child of its
        // parent (which is drawn differently).
        // Also assert that it is not zero already, because this would mean that we are currently
        // processing more children of the parent than its rank indicated.
        assert(ranks[par_idx] > 0);
        --ranks[par_idx];

        // Draw the lines down from the immediate parent of the current node.
        if (ranks[par_idx] > 0) {
            res << "├── ";
        } else {
            res << "└── ";
        }

        // Print the actual information about the current node.
        res << print_line(it) << "\n";
    }

    return res.str();
}

/**
 * @brief
 */
template <typename TreeType>
std::string TreeView::compact (const TreeType& tree)
{
    // TODO this should move to default tree, as it uses the name data member
    auto print_line = [] (typename TreeType::ConstIteratorPreorder& it)
    {
        return it.node()->data.name;
    };
    return compact(tree, print_line);
}

} // namespace genesis
