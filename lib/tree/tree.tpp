/**
 * @brief Implementation of basic tree functions.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <algorithm>
#include <assert.h>
#include <sstream>

#include "tree/newick_processor.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Construction and Destruction
// =============================================================================

/**
 * @brief Copy constructor. Copies the topology, but not the data of a tree.
 *
 * This function creates all links, nodes and edges new, and shapes them so that the final
 * Tree has the same topology as the input Tree.
 *
 * The data of the nodes and edges might contain pointers and other structures that need a deep
 * copy, and we cannot know how to copy it here. It is thus the responsibility of the class that
 * uses the tree to make sure its own data is copied after calling this copy constructor.
 * See Placements for an example.
 *
 * Idea for a nice feature (not yet implemented):
 * The advantage of copying the topology only is that we are able to make this function completely
 * independend of the data, hence the `other` Tree does not need to share the same data types.
 * Some potential function declarations can be found in the header file tree.hpp.
 */
template <class NDT,  class EDT>
Tree<NDT, EDT>::Tree (const Tree<NDT, EDT>& other)
{
    // preparation.
    clear();
    links_.resize(other.links_.size());
    nodes_.resize(other.nodes_.size());
    edges_.resize(other.edges_.size());

    // create all objects. we need two loops per array, because the pointers have to exist
    // in order to be used with each other.
    for (size_t i = 0; i < links_.size(); ++i) {
        links_[i] = new LinkType();
    }
    for (size_t i = 0; i < nodes_.size(); ++i) {
        nodes_[i] = new NodeType();
    }
    for (size_t i = 0; i < edges_.size(); ++i) {
        edges_[i] = new EdgeType();
    }

    // set all pointers for the topology in a second round of loops.
    for (size_t i = 0; i < links_.size(); ++i) {
        LinkType* olink = other.links_[i];
        assert(olink->index_ == i);

        links_[i]->index_  = i;
        links_[i]->next_   = links_[olink->next_->index_];
        links_[i]->outer_  = links_[olink->outer_->index_];
        links_[i]->node_   = nodes_[olink->node_->index_];
        links_[i]->edge_   = edges_[olink->edge_->index_];
    }
    for (size_t i = 0; i < nodes_.size(); ++i) {
        NodeType* onode = other.nodes_[i];
        assert(onode->index_ == i);

        nodes_[i]->index_  = i;
        nodes_[i]->link_   = links_[onode->link_->index_];
    }
    for (size_t i = 0; i < edges_.size(); ++i) {
        EdgeType* oedge = other.edges_[i];
        assert(oedge->index_ == i);

        edges_[i]->index_  = i;
        edges_[i]->link_p_ = links_[oedge->link_p_->index_];
        edges_[i]->link_s_ = links_[oedge->link_s_->index_];
    }
}

/**
 * @brief Assignment operator. Copies the topology, but not the data of a tree.
 *
 * See Tree copy constructor for more information.
 */
template <class NDT, class EDT>
Tree<NDT, EDT>& Tree<NDT, EDT>::operator = (const Tree<NDT, EDT>& other)
{
    // check for self-assignment. we want this explicitly, in order to avoid unnecessary copies of
    // the tree, which would mean loosing the data in process.
    if (&other == this) {
        return *this;
    }

    // the Tree tmp is a copy of the right hand side object (automatically created using the
    // copy constructor). we can thus simply swap the arrays, and upon leaving the function,
    // tmp is automatically destroyed, so that its arrays are cleared and the data freed.
    Tree<NDT, EDT> tmp(other);
    std::swap(links_, tmp.links_);
    std::swap(nodes_, tmp.nodes_);
    std::swap(edges_, tmp.edges_);
    return *this;
}

/**
 * @brief Destructor. Calls clear() to free all memory used by the tree and its substructures.
 */
template <class NDT, class EDT>
Tree<NDT, EDT>::~Tree ()
{
    clear();
}

/**
 * @brief Deletes all data of the tree, including all links, nodes and edges.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::clear()
{
    for (LinkType* link : links_) {
        delete link;
    }
    for (NodeType* node : nodes_) {
        delete node;
    }
    for (EdgeType* edge : edges_) {
        delete edge;
    }

    std::vector<LinkType*>().swap(links_);
    std::vector<NodeType*>().swap(nodes_);
    std::vector<EdgeType*>().swap(edges_);
}

/**
 * @brief Swap.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::swap (Tree<NDT, EDT>& other)
{
    std::swap(links_, other.links_);
    std::swap(nodes_, other.nodes_);
    std::swap(edges_, other.edges_);
}

/**
 * @brief Imports all elements of a tree.
 *
 * This function overwrites the topology and data of this tree with a given set of links, nodes
 * and edges. Use with care! No checks are done concerning the validity of the passed input.
 *
 * Caveat: Only the pointers to the tree elements are copied, not the elements themselves. Thus,
 * this function is not intended for creating a deep copy. It merely is a fast way to pass pointers
 * to tree elements.
 *
 * Therefore, the main usage of this function is to get a tree from different TreeProcessor objects
 * for reading trees from files.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::Import(LinkArray& links, NodeArray& nodes, EdgeArray& edges)
{
    clear();
    links_ = links;
    nodes_ = nodes;
    edges_ = edges;
}

/**
 * @brief Exports all elements of a tree.
 *
 * Caveat: Only the pointers to the tree elements are copied, not the elements themselves. Thus,
 * this function is not intended for creating a deep copy. It merely is a fast way to pass pointers
 * to tree elements.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::Export(LinkArray& links, NodeArray& nodes, EdgeArray& edges)
{
    links = links_;
    nodes = nodes_;
    edges = edges_;
}

// =============================================================================
//     Member Functions
// =============================================================================

/**
 * @brief Find a Node, given its name.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::NodeType* Tree<NDT, EDT>::FindNode(std::string name) const
{
    // TODO check first whether replacing underscores is necessary!
    name = StringReplaceAll(name, "_", " ");
    for (NodeType* n : nodes_) {
        if (n->name == name) {
            return n;
        }
    }
    return nullptr;
}

/**
 * @brief Returns the highest rank of the nodes of the Tree.
 */
template <class NDT, class EDT>
int Tree<NDT, EDT>::MaxRank() const
{
    int max = -1;
    for (size_t i = 0; i < nodes_.size(); ++i) {
        int rank = nodes_[i]->Rank();
        if (rank == 1) {
            LOG_WARN << "Node with rank 1 found. This is a node without furcation, and usually "
                     << "indicates an error.";
        }
        max = std::max(rank, max);
    }
    return max;
}

/**
 * @brief Returns whether the Tree is bifurcating.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::IsBifurcating() const
{
    return MaxRank() == 2;
}

/**
 * @brief Count the number of leaf nodes.
 */
template <class NDT, class EDT>
size_t Tree<NDT, EDT>::LeafCount() const
{
    size_t sum = 0;
    for (NodeType* n : nodes_) {
        if (n->IsLeaf()) {
            ++sum;
        }
    }
    return sum;
}

/**
 * @brief Count the number of inner nodes.
 */
template <class NDT, class EDT>
size_t Tree<NDT, EDT>::InnerCount() const
{
    return nodes_.size() - LeafCount();
}

// =============================================================================
//     Distances
// =============================================================================

/**
 * @brief Returns the length of the tree (sum of all branch lengths).
 */
template <class NDT, class EDT>
double Tree<NDT, EDT>::Length() const
{
    double len = 0.0;
    for (EdgeType* e : edges_) {
        len += e->branch_length;
    }
    return len;
}

/**
 * @brief
 *
 * The vector is indexed using the Node()->Index() for every node.
 */
template <class NDT, class EDT>
Matrix<int>* Tree<NDT, EDT>::NodeDepthMatrix() const
{
    Matrix<int>* mat = new Matrix<int>(NodeCount(), NodeCount());
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    return mat;
}

/**
 * @brief Returns a vector containing the depth of all nodes with respect to the given start node.
 *
 * The vector is indexed using the Node()->Index() for every node. Its elements give the depth of
 * each node with respect to the given start node. The depth is the number of edges visited on the
 * path between two nodes (0 for itself, 1 for immediate neighbours, etc).
 *
 * If no start node pointer is provided, the root is taken as node.
 */
template <class NDT, class EDT>
std::vector<int> Tree<NDT, EDT>::NodeDepthVector(const NodeType* node) const
{
    if (!node) {
        node = RootNode();
    }

    // store the distance from each node to the given node.
    std::vector<int> vec;
    vec.resize(NodeCount(), -1);
    vec[node->Index()] = 0;

    // calculate the distance vector via levelorder iteration.
    for (
        ConstIteratorLevelorder it = BeginLevelorder(node);
        it != EndLevelorder();
        ++it
    ) {
        // skip the starting node (it is already set to 0).
        if (it.IsFirstIteration()) {
            continue;
        }

        // we do not have the distance of the current node, but the one of its "parent"!
        assert(vec[it.Node()->Index()] == -1);
        assert(vec[it.Link()->Outer()->Node()->Index()] > -1);

        // the distance is the distance from the "parent" node (the next one in direction towards
        // the given node) plus 1.
        vec[it.Node()->Index()] = 1 + vec[it.Link()->Outer()->Node()->Index()];
    }

    return vec;
}

/**
 * @brief Returns a distance matrix containing pairwise distances between all Nodes, using the
 * branch_length of the Edges as distance measurement.
 *
 * The elements of the matrix are indexed using Node()->Index().
 */
template <class NDT, class EDT>
Matrix<double>* Tree<NDT, EDT>::NodeDistanceMatrix() const
{
    Matrix<double>* mat = new Matrix<double>(NodeCount(), NodeCount(), -1.0);

    // fill every row of the matrix
    for (NodeType* row_node : nodes_) {
        // set the diagonal element of the matrix.
        (*mat)(row_node->Index(), row_node->Index()) = 0.0;

        // the columns are filled using a levelorder traversal. this makes sure that for every node
        // we know how to calculate the distance to the current row node.
        // unfortunately, this prevents us from simply calculating the upper triangle of the matrix
        // and copying it (distance is symmetric), because we do not really know which nodes are in
        // which half during a levelorder traversal...
        for (
            ConstIteratorLevelorder it = BeginLevelorder(row_node->Link());
            it != EndLevelorder();
            ++it
        ) {
            // skip the diagonal of the matrix.
            if (it.IsFirstIteration()) {
                continue;
            }

            // make sure we don't have touched the current position, but have calculated
            // the needed dependency already.
            assert((*mat)(row_node->Index(), it.Node()->Index()) == -1.0);
            assert((*mat)(row_node->Index(), it.Link()->Outer()->Node()->Index()) > -1.0);

            // the distance to the current row node is: the length of the current branch plus
            // the distance from the other end of that branch to the row node.
            (*mat)(row_node->Index(), it.Node()->Index())
                = it.Edge()->branch_length
                + (*mat)(row_node->Index(), it.Link()->Outer()->Node()->Index());
        }
    }

    return mat;
}

/**
 * @brief
 *
 * If no Node pointer is provided, the root is taken as node.
 */
template <class NDT, class EDT>
std::vector<double> Tree<NDT, EDT>::NodeDistanceVector(const NodeType* node) const
{
    if (!node) {
        node = RootNode();
    }

    std::vector<double> vec;
    vec.resize(NodeCount(), 0.0);
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    return vec;
}

/**
 * @brief Returns a vector containing the closest leaf node for each node, measured in number of
 * edges between them and its depth (number of edges between them).
 *
 * The vector is indexed using the Node()->Index() for every node. Its value contains an std::pair,
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
template <class NDT, class EDT>
typename Tree<NDT, EDT>::NodeIntVectorType Tree<NDT, EDT>::ClosestLeafDepthVector() const
{
    // prepare a result vector with the size of number of nodes.
    NodeIntVectorType vec;
    vec.resize(NodeCount(), {nullptr, 0});

    // fill the vector for every node.
    // this could be speed up by doing a postorder traversal followed by some sort of inside-out
    // traversal (preorder might do the job). but for now, this simple O(n^2) version works, too.
    for (NodeType* node : nodes_) {
        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->Index()].first == nullptr);

        // look for closest leaf node by doing a levelorder traversal.
        for (
            ConstIteratorLevelorder it = BeginLevelorder(node);
            it != EndLevelorder();
            ++it
        ) {
            // if we find a leaf, leave the loop.
            if (it.Node()->IsLeaf()) {
                vec[node->Index()].first  = it.Node();
                vec[node->Index()].second = it.Depth();
                break;
            }
        }
    }

    return vec;
}

/**
 * @brief Returns a vector containing the closest leaf node for each node, using the branch_length
 * as distance measure.
 *
 * The vector is indexed using the Node()->Index() for every node. Its value contains an std::pair,
 * where the first element is a NodeType* to the closest leaf node of the node at the index,
 * measured using the branch_length; the second element of the pair is the distance value itself.
 * Thus, leaf nodes will have a pointer to themselves and a distance value of 0.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::NodeDoubleVectorType Tree<NDT, EDT>::ClosestLeafDistanceVector() const
{
    // prepare a result vector with the size of number of nodes.
    NodeDoubleVectorType vec;
    vec.resize(NodeCount(), {nullptr, 0.0});

    // we need the pairwise distances between all nodes, so we can do quick loopups.
    Matrix<double>* node_distances = NodeDistanceMatrix();

    // fill the vector for every node.
    // there is probably a faster way of doing this: preorder traversal with pruning. but for now,
    // this simple O(n^2) version works.
    for (NodeType* node : nodes_) {
        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->Index()].first == nullptr);

        NodeType* min_node = nullptr;
        double    min_dist = 0.0;

        // try out all other nodes, and find the closest leaf.
        for (NodeType* other : nodes_) {
            if (!other->IsLeaf()) {
                continue;
            }

            double dist = (*node_distances)(node->Index(), other->Index());
            if (min_node == nullptr || dist < min_dist) {
                min_node = other;
                min_dist = dist;
            }
        }

        vec[node->Index()].first  = min_node;
        vec[node->Index()].second = min_dist;
    }

    return vec;
}

/**
 * @brief Returns the longest distance from any point in the tree (on the edges) to any leaf.
 */
template <class NDT, class EDT>
double Tree<NDT, EDT>::DeepestDistance() const
{
    double max = 0.0;

    NodeDoubleVectorType leaf_dist = ClosestLeafDistanceVector();

    for (EdgeType* e : edges_) {
        int idx_p = e->PrimaryNode()->Index();
        int idx_s = e->SecondaryNode()->Index();
        double d = (leaf_dist[idx_p].second + e->branch_length + leaf_dist[idx_s].second) / 2;

        if (d > max) {
            max = d;
        }
    }
    return max;
}

// =============================================================================
//     Comparison
// =============================================================================

/**
 * @brief Compares two trees for equality given a binary comparator functional.
 *
 * This function does a preorder traversal of both trees in parallel and calls a comparator
 * functional for each position of the iterator. It returns true iff the comparator is true for
 * every position.
 *
 * The comparator functional can be either a function pointer, function object, or lambda
 * expression.
 *
 * Furthermore, the trees are checked for equal topology: their elements (links, nodes, edges)
 * have to be equal in size and the rank of each node during the traversal has to be identical in
 * both trees. Those assumptions are made because two trees that do not have identical topology
 * are never considered equal for the purposes of this framework.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::Equal(
    const TreeType& lhs,
    const TreeType& rhs,
    const std::function<bool (TreeType::ConstIteratorPreorder&, TreeType::ConstIteratorPreorder&)>
        comparator
) {
    // check array sizes
    if (lhs.links_.size() != rhs.links_.size() ||
        lhs.nodes_.size() != rhs.nodes_.size() ||
        lhs.edges_.size() != rhs.edges_.size()
    ) {
        return false;
    }

    // do a preorder traversal on both trees in parallel
    TreeType::ConstIteratorPreorder it_l = lhs.BeginPreorder();
    TreeType::ConstIteratorPreorder it_r = rhs.BeginPreorder();
    for (
        ;
        it_l != lhs.EndPreorder() && it_r != rhs.EndPreorder();
        ++it_l, ++it_r
    ) {
        if (it_l.Node()->Rank() != it_r.Node()->Rank() || !comparator(it_l, it_r)) {
            return false;
        }
    }

    // check whether we are done with both trees
    if (it_l != lhs.EndPreorder() || it_r != rhs.EndPreorder()) {
        return false;
    }

    return true;
}

/**
 * @brief Compares the tree to another one given a binary comparator functional.
 *
 * See other Equal() function for more information.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::Equal(
    const TreeType& other,
    const std::function<bool (TreeType::ConstIteratorPreorder&, TreeType::ConstIteratorPreorder&)>
        comparator
) const {
    return Equal(*this, other, comparator);
}

// TODO make const! (need to add const versions of the tree iterators first...)
/**
 * @brief Returns true iff both trees have an identical topology.
 *
 * The topology is considered identical only if the order of edges is also the same in both trees.
 * This means, although two trees might have the same number of leaves and branches, they might
 * still be not identical (with respect to this function) when the branches appear in a different
 * order or when the root sits at a different node.
 *
 * Thus, this function is mainly intended to check whether two trees have been produced from the
 * same input, for example from the same Newick file.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::HasIdenticalTopology(const TreeType& right) const
{
    auto comparator = [] (TreeType::ConstIteratorPreorder&, TreeType::ConstIteratorPreorder&)
    {
        return true;
    };

    return Equal(right, comparator);
}

// =============================================================================
//     Dump and Debug Functions
// =============================================================================


    // TODO introduce a validate function that checks the integrity of the tree:
    // TODO are all links, edges and nodes connected corretly to each other,
    // TODO is every one of them coverd exactly once when doing a full traversal?
    // TODO do all node->link_ links point to the root? same for all edge->primary?
    // TODO all objects coupled correctly?
    // TODO also, if we introduce indices to them for faster access, are those correct?
    // TODO this function will be curtial to ensure correctness of invariants once
    // TODO we start implementing stuff that modifies a tree (add nodes, move branches...)!
    // TODO do all iterators and check consistency! eg is a round trip covering every object
    // TODO (links onces, branches twice, nodes rank many times)?
    // TODO iterator over all links and count if edges are encountered exactly twice each.


template <class NDT, class EDT>
bool Tree<NDT, EDT>::Validate() const
{
    // check that the member arrays are valid: if at least one of them is empty, the tree is not
    // fully initialized, so either it is a new tree without any data (all arrays empty, valid),
    // or some are empty, but others not (not valid).
    if (links_.empty() || nodes_.empty() || edges_.empty()) {
        bool emp = links_.empty() && nodes_.empty() && edges_.empty();
        if (emp) {
            LOG_INFO << "Tree is empty.";
        } else {
            LOG_INFO << "Tree is not empty, but one of its data members is.";
        }
        return emp;
    }

    if (links_.front()->node_ != nodes_.front()) {
        LOG_INFO << "The first link does not correspond to the first node.";
        return false;
    }

    for (size_t i = 0; i < links_.size(); ++i) {
        if (i != links_[i]->index_) {
            LOG_INFO << "Link at index " << i << " has wrong index (" << links_[i]->index_ << ").";
            return false;
        }
    }

    for (size_t i = 0; i < nodes_.size(); ++i) {
        if (i != nodes_[i]->index_) {
            LOG_INFO << "Node at index " << i << " has wrong index (" << nodes_[i]->index_ << ").";
            return false;
        }
    }

    for (size_t i = 0; i < edges_.size(); ++i) {
        if (i != edges_[i]->index_) {
            LOG_INFO << "Edge at index " << i << " has wrong index (" << edges_[i]->index_ << ").";
            return false;
        }
    }

    // if we are here, all three arrays (links, nodes, edges) contain data, so we can start a full
    // traversal along all links.
    LinkType* link = links_.front();
    do {
        link = link->next_->outer_;
    } while (link != links_.front());

    return true;
}

/**
 * @brief Returns a simple text representation of the Tree, showing all nodes, edges and links
 * with their indices.
 */
template <class NDT, class EDT>
std::string Tree<NDT, EDT>::Dump() const
{
    std::vector<int> depth = this->NodeDepthVector();
    std::vector<int> done;
    std::ostringstream out;

    // prepare link so that we point to the root link. this will ensure that the order of nodes
    // displayed by this funtion is the one expected by the user. usually, we would go into
    // the first branch immediately, but then there would be no way of first nicely displaying
    // the information about the root node. so we need to do it a bit more complex than the
    // usual iteration...
    LinkType* l = RootLink();
    while (l->Next() != RootLink()) {
        l = l->Next();
    }

    // do an euler tour traversal over all links. (we cannot use the iterator here, as
    // we need each link on its own, and not each node as the iterator gives)
    do {
        NodeType* n = l->Node();
        std::string indent = std::string(4 * depth[n->Index()], ' ');
        if (!Contains(done, n->Index())) {
            out << indent << "\033[1;31mNode " << n->Index() << ": \"" << n->name << "\"\033[0m\n";
        }
        done.push_back(n->Index());

        // dont display the next link when we are at the first iteration.
        if (l->Next() == RootLink()) {
            l = l->Next();
        } else {
            out << indent;
            out << "    \033[34mLink " << l->Index() << "\033[0m";
            l = l->Next();
            out << " \033[32m>\033[0m \033[34mLink " << l->Index() << "\033[0m\n";
        }

        out << indent;
        out << " -- \033[34mLink " << l->Index() << "\033[0m";
        out << " -- \033[36mEdge " << l->Edge()->Index() << "\033[0m";
        l = l->Outer();
        out << " --> \033[34mLink " << l->Index() << "\033[0m\n";
    } while (l->Next() != RootLink());

    // output the last next link back to the root, because we skipped this in the loop
    // (the one that was skipped in the beginning).
    out << "    \033[34mLink " << l->Index() << "\033[0m";
    l = l->Next();
    out << " \033[32m>\033[0m \033[34mLink " << l->Index() << "\033[0m\n";

    return out.str();
}

/**
 * @brief Returns lists of all nodes, edges and links including their indices and connections
 * with each other.
 */
template <class NDT, class EDT>
std::string Tree<NDT, EDT>::DumpLists() const
{
    std::ostringstream out;

    // nodes
    for (size_t i = 0; i < nodes_.size(); ++i) {
        out << "Node " << i
            << " \t Main Link: " << nodes_[i]->link_->index_
            << " \t " << nodes_[i]->Dump() << "\n";
    }
    out << "\n";

    // edges
    for (size_t i = 0; i < edges_.size(); ++i) {
        out << "Edge " << i
            << " \t Link P: " << edges_[i]->link_p_->index_
            << " \t Link S: " << edges_[i]->link_s_->index_
            << " \t " << edges_[i]->Dump() << "\n";
    }
    out << "\n";

    // links
    for (size_t i = 0; i < links_.size(); ++i) {
        out << "Link " << i
            << "  \t Next: "  << links_[i]->next_->index_
            << " \t Outer: " << links_[i]->outer_->index_
            << " \t Node: "  << links_[i]->node_->index_
            << " \t Edge: "  << links_[i]->edge_->index_
            << " \t " << links_[i]->Dump()
            << "\n";
    }

    return out.str();
}

} // namespace genesis
