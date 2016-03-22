/**
 * @brief Implementation of Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include <stdexcept>

#include "tree/iterator/levelorder.hpp"
#include "utils/core/logging.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Distance Measures
// =================================================================================================

/**
 * @brief
 *
 * The vector is indexed using the node().index() for every node.
 */
template <class Tree>
utils::Matrix<int>         node_depth_matrix    (
    const Tree& tree
) {
    // TODO implement!
    (void) tree;
    throw std::domain_error("Not yet implemented.");
    return utils::Matrix<int>(0,0);
}

/**
 * @brief Returns a vector containing the depth of all nodes with respect to the given start node.
 *
 * The vector is indexed using the node().index() for every node. Its elements give the depth of
 * each node with respect to the given start node. The depth is the number of edges visited on the
 * path between two nodes (0 for itself, 1 for immediate neighbours, etc).
 *
 * If no start node pointer is provided, the root is taken as node.
 */
template <class Tree>
std::vector<int>    node_depth_vector    (
    const Tree& tree,
    const typename Tree::NodeType* node
) {
    if (!node) {
        node = &tree.root_node();
    }

    // store the distance from each node to the given node.
    std::vector<int> vec;
    vec.resize(tree.node_count(), -1);
    vec[node->index()] = 0;

    // calculate the distance vector via levelorder iteration.
    for( auto it : levelorder( *node ) ) {
        // skip the starting node (it is already set to 0).
        if (it.is_first_iteration()) {
            continue;
        }

        // we do not have the distance of the current node, but the one of its "parent" (the one in
        // direction of the starting node)!
        assert(vec[it.node().index()] == -1);
        assert(vec[it.link().outer().node().index()] > -1);

        // the distance is the distance from the "parent" node (the next one in direction towards
        // the given node) plus 1.
        vec[it.node().index()] = 1 + vec[it.link().outer().node().index()];
    }

    return vec;
}

template <class Tree>
utils::Matrix<int>         edge_depth_matrix    (
    const Tree& tree
) {
    // TODO implement!
    (void) tree;
    throw std::domain_error("Not yet implemented.");
    return utils::Matrix<int>(0,0);
}

template <class Tree>
std::vector<int>    edge_depth_vector    (
    const Tree& tree,
    const typename Tree::EdgeType* edge
) {
    // TODO implement!
    (void) tree;
    (void) edge;
    throw std::domain_error("Not yet implemented.");
    return std::vector<int>();
}

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

/**
 * @brief Returns a vector containing the closest leaf node for each node, measured in number of
 * edges between them and its depth (number of edges between them).
 *
 * The vector is indexed using the node().index() for every node. Its value contains an std::pair,
 * where the first element is a NodeType* to the closest leaf node (with respect to its depth) and
 * the second element its depth with respect to the node at the given index of the vector. The depth
 * is the number of edges visited on the path between two nodes (0 for itself, 1 for immediate
 * neighbours, etc).
 *
 * Thus, leaf nodes will have a pointer to themselves and a depth value of 0, and for all other
 * nodes the depth will be the number of edges between it and the closest leaf node.
 *
 * There might be more than one leaf with the same depth to a given node. In this case, an
 * arbitrary one is used.
 */
template <class Tree>
std::vector< std::pair<const typename Tree::NodeType*, int> >  closest_leaf_depth_vector (
    const Tree& tree
) {
    // prepare a result vector with the size of number of nodes.
    std::vector< std::pair<typename Tree::NodeType const*, int> > vec;
    vec.resize(tree.node_count(), {nullptr, 0});

    // fill the vector for every node.
    // this could be speed up by doing a postorder traversal followed by some sort of inside-out
    // traversal (preorder might do the job). but for now, this simple O(n^2) version works, too.
    for (auto node_it = tree.begin_nodes(); node_it != tree.end_nodes(); ++node_it) {
        auto node = node_it->get();

        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->index()].first == nullptr);

        // look for closest leaf node by doing a levelorder traversal.
        for( auto it : levelorder( *node ) ) {
            // if we find a leaf, leave the loop.
            if (it.node().is_leaf()) {
                vec[node->index()].first  = &it.node();
                vec[node->index()].second =  it.depth();
                break;
            }
        }
    }

    return vec;
}

} // namespace tree
} // namespace genesis
