/**
 * @brief Implementation of Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// =================================================================================================
//     Distance Measures
// =================================================================================================

template <class Tree>
Matrix<int>         node_depth_matrix    (
    const Tree& tree
) {
    Matrix<int> mat (tree.node_count(), tree.node_count());
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    return mat;
}

template <class Tree>
std::vector<int>    node_depth_vector    (
    const Tree& tree,
    const typename Tree::NodeType* node
) {
    if (!node) {
        node = tree.root_node();
    }

    // store the distance from each node to the given node.
    std::vector<int> vec;
    vec.resize(tree.node_count(), -1);
    vec[node->index()] = 0;

    // calculate the distance vector via levelorder iteration.
    for (
        auto it = tree.begin_levelorder(node);
        it != tree.end_levelorder();
        ++it
    ) {
        // skip the starting node (it is already set to 0).
        if (it.is_first_iteration()) {
            continue;
        }

        // we do not have the distance of the current node, but the one of its "parent" (the one in
        // direction of the starting node)!
        assert(vec[it.node()->index()] == -1);
        assert(vec[it.link()->outer()->node()->index()] > -1);

        // the distance is the distance from the "parent" node (the next one in direction towards
        // the given node) plus 1.
        vec[it.node()->index()] = 1 + vec[it.link()->outer()->node()->index()];
    }

    return vec;
}

template <class Tree>
Matrix<int>         edge_depth_matrix    (
    const Tree& tree
) {
    Matrix<int> mat (tree.edge_count(), tree.edge_count());
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    return mat;
}

template <class Tree>
std::vector<int>    edge_depth_vector    (
    const Tree& tree,
    const typename Tree::EdgeType* edge
) {
    std::vector<int> vec (tree.edge_count());
    (void) edge;
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    return vec;
}

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

template <class Tree>
std::vector< std::pair<const typename Tree::NodeType*, int> >  closest_leaf_depth_vector (
    const Tree& tree
) {
    // prepare a result vector with the size of number of nodes.
    std::vector< std::pair<const typename Tree::NodeType*, int> > vec;
    vec.resize(tree.node_count(), {nullptr, 0});

    // fill the vector for every node.
    // this could be speed up by doing a postorder traversal followed by some sort of inside-out
    // traversal (preorder might do the job). but for now, this simple O(n^2) version works, too.
    for (auto node_it = tree.begin_nodes(); node_it != tree.end_nodes(); ++node_it) {
        auto node = node_it->get();

        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->index()].first == nullptr);

        // look for closest leaf node by doing a levelorder traversal.
        for (
            auto it = tree.begin_levelorder(node);
            it != tree.end_levelorder();
            ++it
        ) {
            // if we find a leaf, leave the loop.
            if (it.node()->is_leaf()) {
                vec[node->index()].first  = it.node();
                vec[node->index()].second = it.depth();
                break;
            }
        }
    }

    return vec;
}

} // namespace genesis
