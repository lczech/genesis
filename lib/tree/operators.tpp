/**
 * @brief Implementation of Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// =================================================================================================
//     Equality
// =================================================================================================

template <class TreeType>
bool equal(
    const TreeType& lhs,
    const TreeType& rhs,
    const std::function<bool
        (typename TreeType::ConstIteratorPreorder&, typename TreeType::ConstIteratorPreorder&)
    > comparator
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
        if (it_l.node()->rank() != it_r.node()->rank() || !comparator(it_l, it_r)) {
            return false;
        }
    }

    // check whether we are done with both trees
    if (it_l != lhs.end_preorder() || it_r != rhs.end_preorder()) {
        return false;
    }

    return true;
}

template <class TreeType>
bool equal(const TreeType& lhs, const TreeType& rhs)
{
    auto comparator = [] (
        typename TreeType::ConstIteratorPreorder& it_l,
        typename TreeType::ConstIteratorPreorder& it_r
    ) {
        return it_l.node() == it_r.node() && it_l.edge() == it_r.edge();
    };

    return equal(lhs, rhs, comparator);
}

template <class TreeTypeA, class TreeTypeB>
bool identical_topology(const TreeTypeA& lhs, const TreeTypeB& rhs)
{
    auto comparator = [] (
        typename TreeTypeA::ConstIteratorPreorder&,
        typename TreeTypeB::ConstIteratorPreorder&
    ) {
        return true;
    };

    return equal(lhs, rhs, comparator);
}

} // namespace genesis
