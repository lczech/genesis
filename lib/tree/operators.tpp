/**
 * @brief Implementation of Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/iterators/preorder.hpp"

namespace genesis {

// =================================================================================================
//     Equality
// =================================================================================================

template <class TreeTypeL, class TreeTypeR>
bool equal(
    const TreeTypeL& lhs,
    const TreeTypeR& rhs,
    std::function<bool
        (const typename TreeTypeL::NodeType&, const typename TreeTypeR::NodeType&)
    > node_comparator,
    std::function<bool
        (const typename TreeTypeL::EdgeType&, const typename TreeTypeR::EdgeType&)
    > edge_comparator
) {
    // Check array sizes.
    if (lhs.link_count() != rhs.link_count() ||
        lhs.node_count() != rhs.node_count() ||
        lhs.edge_count() != rhs.edge_count()
    ) {
        return false;
    }

    // do a preorder traversal on both trees in parallel
    auto it_l = lhs.begin_preorder();
    auto it_r = rhs.begin_preorder();
    for (
        ;
        it_l != lhs.end_preorder() && it_r != rhs.end_preorder();
        ++it_l, ++it_r
    ) {
        if (it_l.node()->rank() != it_r.node()->rank()   ||
            !node_comparator(*it_l.node(), *it_r.node()) ||
            !edge_comparator(*it_l.edge(), *it_r.edge())
        ) {
            return false;
        }
    }

    // check whether we are done with both trees
    if (it_l != lhs.end_preorder() || it_r != rhs.end_preorder()) {
        return false;
    }

    return true;
}

template <class TreeTypeL, class TreeTypeR>
bool equal(const TreeTypeL& lhs, const TreeTypeR& rhs)
{
    auto node_comparator = [] (
        const typename TreeTypeL::NodeType& node_l,
        const typename TreeTypeR::NodeType& node_r
    ) {
        return node_l == node_r;
    };

    auto edge_comparator = [] (
        const typename TreeTypeL::EdgeType& edge_l,
        const typename TreeTypeR::EdgeType& edge_r
    ) {
        return edge_l == edge_r;
    };

    return equal(lhs, rhs, node_comparator, edge_comparator);
}

template <class TreeTypeL, class TreeTypeR>
bool identical_topology(const TreeTypeL& lhs, const TreeTypeR& rhs)
{
    auto node_comparator = [] (
        const typename TreeTypeL::NodeType& node_l,
        const typename TreeTypeR::NodeType& node_r
    ) {
        (void) node_l;
        (void) node_r;
        return true;
    };

    auto edge_comparator = [] (
        const typename TreeTypeL::EdgeType& edge_l,
        const typename TreeTypeR::EdgeType& edge_r
    ) {
        (void) edge_l;
        (void) edge_r;
        return true;
    };

    return equal(lhs, rhs, node_comparator, edge_comparator);
}

} // namespace genesis
