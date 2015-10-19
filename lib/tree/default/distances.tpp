/**
 * @brief Implementation of Default Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include <algorithm>

namespace genesis {

// =================================================================================================
//     Distance Related Propoerties
// =================================================================================================

template <class Tree>
double length(const Tree& tree)
{
    // TODO if we introduce some naming convention for with/withou branch length, the without
    // branch lengths version of this function will basically return edges.size()... is that needed?
    // TODO use an algorithm here
    double len = 0.0;
    for (const auto edge_it = tree.begin_edges(); edge_it != tree.end_edges(); ++edge_it) {
        len += edge_it->data.branch_length;
    }
    return len;
}

template <class Tree>
double height(const Tree& tree)
{
    auto dists = node_distance_vector(tree);
    return *std::max_element(dists.begin(), dists.end());
}

template <class Tree>
double depth(const Tree& tree)
{
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    (void) tree;
    return 0.0;
}

template <class Tree>
double deepest_distance(const Tree& tree)
{
    // TODO this should better be called diameter, return the double of this value. also, it can
    // be calculated in a different way: take the max of the node distance matrix. this should yield
    // the same value. this is also a good way of verifiying the result for testing!
    // TODO diameter method already exists as a method called heigth, see above!
    // TODO also, find a naming convention for weighted (with branch lengths) and unweighted diamter and
    // all other functions dealing with tree distances!

    double max = 0.0;

    auto leaf_dist = closest_leaf_distance_vector(tree);

    for (const auto& e = tree.begin_edge(); e != tree.end_edges(); ++e) {
        int idx_p = e->primary_node()->index();
        int idx_s = e->secondary_node()->index();
        double d = (leaf_dist[idx_p].second + e->data.branch_length + leaf_dist[idx_s].second) / 2;

        if (d > max) {
            max = d;
        }
    }
    return max;
}

// =================================================================================================
//     Branch Distance Measures
// =================================================================================================

template <class Tree>
Matrix<double>      node_distance_matrix (
    const Tree& tree
) {
    Matrix<double> mat (tree.node_count(), tree.node_count(), -1.0);

    // fill every row of the matrix
    for (auto row_it = tree.begin_nodes(); row_it != tree.end_nodes(); ++row_it) {
        const auto row_node = row_it->get();

        // set the diagonal element of the matrix.
        mat(row_node->index(), row_node->index()) = 0.0;

        // the columns are filled using a levelorder traversal. this makes sure that for every node
        // we know how to calculate the distance to the current row node.
        // unfortunately, this prevents us from simply calculating the upper triangle of the matrix
        // and copying it (distance is symmetric), because we do not really know which nodes are in
        // which half during a levelorder traversal...
        for (
            auto it = tree.begin_levelorder(row_node->link());
            it != tree.end_levelorder();
            ++it
        ) {
            // skip the diagonal of the matrix.
            if (it.is_first_iteration()) {
                continue;
            }

            // make sure we don't have touched the current position, but have calculated
            // the needed dependency already.
            assert(mat(row_node->index(), it.node()->index()) == -1.0);
            assert(mat(row_node->index(), it.link()->outer()->node()->index()) > -1.0);

            // the distance to the current row node is: the length of the current branch plus
            // the distance from the other end of that branch to the row node.
            mat(row_node->index(), it.node()->index())
                = it.edge()->data.branch_length
                + mat(row_node->index(), it.link()->outer()->node()->index());
        }
    }

    return mat;
}

template <class Tree>
std::vector<double> node_distance_vector (
    const Tree& tree,
    const typename Tree::NodeType* node
) {
    if (!node) {
        node = tree.root_node();
    }

    // Store the distance from each node to the given node.
    auto vec = std::vector<double>(tree.node_count(), -1.0);
    vec[node->index()] = 0.0;

    // Calculate the distance vector via levelorder iteration.
    for (
        auto it = tree.begin_levelorder(node);
        it != tree.end_levelorder();
        ++it
    ) {
        // Skip the starting node (it is already set to 0).
        if (it.is_first_iteration()) {
            continue;
        }

        // We do not have the distance of the current node, but the one of its "parent" (the one in
        // direction of the starting node)!
        assert(vec[it.node()->index()] == -1.0);
        assert(vec[it.link()->outer()->node()->index()] > -1.0);

        // The distance is the distance from the "parent" node (the next one in direction towards
        // the starting node) plus the branch length.
        vec[it.node()->index()]
            = vec[it.link()->outer()->node()->index()]
            + it.edge()->data.branch_length;
    }

    return vec;
}

template <class Tree>
Matrix<double>      edge_distance_matrix (
    const Tree& tree
) {
    Matrix<double> mat (tree.edge_count(), tree.edge_count());
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    return mat;
}

template <class Tree>
std::vector<double> edge_distance_vector (
    const Tree& tree,
    const typename Tree::EdgeType* edge
) {
    std::vector<double> vec (tree.edge_count());
    (void) edge;
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    return vec;
}

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

template <class Tree>
std::vector<std::pair<const typename Tree::NodeType*, double>> closest_leaf_distance_vector(
    const Tree& tree
) {
    // prepare a result vector with the size of number of nodes.
    std::vector<std::pair<const typename Tree::NodeType*, double>> vec;
    vec.resize(tree.node_count(), {nullptr, 0.0});

    // we need the pairwise distances between all nodes, so we can do quick loopups.
    auto node_distances = node_distance_matrix(tree);

    // fill the vector for every node.
    // there is probably a faster way of doing this: preorder traversal with pruning. but for now,
    // this simple O(n^2) version works.
    for (auto node_it = tree.begin_nodes(); node_it != tree.end_nodes(); ++node_it) {
        auto node = node_it->get();

        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->index()].first == nullptr);

        typename Tree::NodeType* min_node = nullptr;
        double min_dist = 0.0;

        // try out all other nodes, and find the closest leaf.
        for (auto other_it = tree.begin_nodes(); other_it != tree.end_nodes(); ++other_it) {
            auto other = other_it->get();

            if (!other->is_leaf()) {
                continue;
            }

            double dist = node_distances(node->index(), other->index());
            if (min_node == nullptr || dist < min_dist) {
                min_node = other;
                min_dist = dist;
            }
        }

        vec[node->index()].first  = min_node;
        vec[node->index()].second = min_dist;
    }

    return vec;
}

} // namespace genesis
