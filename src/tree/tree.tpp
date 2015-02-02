/**
 * @brief Implementation of basic tree functions.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <algorithm>
#include <assert.h>
#include <sstream>

#include "tree/newick_format.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// -------------------------------------------------------------------------
//     Construction and Destruction
// -------------------------------------------------------------------------

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
 * @brief Destructor. Calls clear() to free all memory used by the tree and its substructures.
 */
template <class NDT, class EDT>
Tree<NDT, EDT>::~Tree ()
{
    clear();
}

// -------------------------------------------------------------------------
//     Read and Write
// -------------------------------------------------------------------------

/**
 * @brief Create a Tree from a file containing a Newick tree.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::FromNewickFile (const std::string& fn)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Newick file '" << fn << "' does not exist.";
        return false;
    }
    return FromNewickString(FileRead(fn));
}

/**
 * @brief Create a Tree from a string containing a Newick tree.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::FromNewickString (const std::string& tree)
{
    TreeBroker broker;
    if (!NewickParser::ProcessString(tree, broker)) {
        return false;
    }

    FromTreeBroker(broker);
    return true;
}

/**
 * @brief Create a Tree from a TreeBroker.
 *
 * It does not take the TreeBroker by const, because AssignRanks() has to be called in order to get
 * the nesting right.
 * TODO: this could be changed by not assigning ranks to the broker but a tmp struct.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::FromTreeBroker (TreeBroker& broker)
{
    clear();
    std::vector<LinkType*> link_stack;

    // we need the ranks (number of immediate children) of all nodes
    broker.AssignRanks();

    // iterate over all nodes of the tree broker
    for (TreeBroker::const_iterator b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        TreeBrokerNode* broker_node = *b_itr;

        // create the tree node for this broker node
        NodeType* cur_node = new NodeType();
        cur_node->FromTreeBrokerNode(broker_node);
        cur_node->index_ = nodes_.size();
        nodes_.push_back(cur_node);

        // create the link that points towards the root.
        // this link is created for every node, root, inner and leaves.
        LinkType* up_link = new LinkType();
        up_link->node_ = cur_node;
        cur_node->link_ = up_link;
        up_link->index_ = links_.size();
        links_.push_back(up_link);

        // establish the link towards the root
        if (link_stack.empty()) {
            // if the link stack is empty, we are currently at the very beginning of this loop,
            // which means we are at the root itself. in this case, make the "link towards the root"
            // point to itself. this makes traversing the tree lots easier!
            up_link->outer_ = up_link;
        } else {
            // if we are however in some other node (leaf or inner, but not the root), we establish
            // the link "upwards" to the root, and back from there.
            up_link->outer_ = link_stack.back();
            link_stack.back()->outer_ = up_link;

            // also, create an edge that connects both nodes
            EdgeType* up_edge = new EdgeType();
            up_edge->link_p_         = link_stack.back();
            up_edge->link_s_         = up_link;
            up_link->edge_           = up_edge;
            link_stack.back()->edge_ = up_edge;
            up_edge->FromTreeBrokerNode(broker_node);
            up_edge->index_ = edges_.size();
            edges_.push_back(up_edge);

            // we can now delete the head of the stack, because we just estiablished its "downlink"
            // and thus are done with it
            link_stack.pop_back();
        }

        // in the following, we create the links that will connect to the nodes' children.
        // for leaf nodes, this makes the next pointer point to the node itself (the loop
        // is never executed in this case, as leaves have rank 0).
        // for inner nodes, we create as many "down" links as they have children. each of them
        // is pushed to the stack, so that for the next broker nodes they are available as
        // reciever for the "up" links.
        // in summary, make all next pointers of a node point to each other in a circle.
        LinkType* next_link = up_link;
        for (int i = 0; i < broker_node->rank(); ++i) {
            LinkType* down_link = new LinkType();
            down_link->next_ = next_link;
            next_link = down_link;

            down_link->node_ = cur_node;
            down_link->index_ = links_.size();
            links_.push_back(down_link);
            link_stack.push_back(down_link);
        }
        up_link->next_ = next_link;
    }

    // now delete the uplink of the root, in order to make the tree fully unrooted.
    // (we do that after the tree creation, as it is way easier this way)
    assert(links_.front()->outer_ == links_.front());
    LinkType* next = links_.front()->next_;
    while (next->next_ != links_.front()) {
        next = next->next_;
    }
    next->next_ = next->next_->next_;
    assert(next->next_ == links_.front()->next_);
    links_.erase(links_.begin());
    for (size_t i = 0; i < links_.size(); ++i) {
        links_[i]->index_ = i;
    }
    next->node_->link_ = next->next_;
}

template <class NDT, class EDT>
void Tree<NDT, EDT>::ToNewickFile(const std::string& fn)
{
    if (FileExists(fn)) {
        LOG_WARN << "Newick file '" << fn << "' already exist. Will not overwrite it.";
        return;
    }
}

template <class NDT, class EDT>
std::string Tree<NDT, EDT>::ToNewickString ()
{
    TreeBroker broker;
    ToTreeBroker(broker);
    return NewickPrinter::ToString(broker);
}

template <class NDT, class EDT>
void Tree<NDT, EDT>::ToTreeBroker (TreeBroker& broker)
{
    // store the distance from each node to the root. this is needed to assign levels of depth
    // to the nodes for the broker.
    std::vector<int> dist;
    dist.resize(NodesSize(), -1);
    dist[0] = 0;

    // calculate the distance vector via levelorder iteration.
    for (
        IteratorLevelorder it = BeginLevelorder();
        it != EndLevelorder();
        ++it
    ) {
        // skip the root (it is already set to 0).
        if (it.IsFirstIteration()) {
            continue;
        }

        // the distance is the distance from the "parent" node (the next one in direction towards
        // the root) plus 1.
        dist[it.Node()->Index()] = 1 + dist[it.Link()->Outer()->Node()->Index()];
    }

    broker.clear();
    for (
        IteratorPostorder it = BeginPostorder();
        it != EndPostorder();
        ++it
    ) {
        TreeBrokerNode* bn = new TreeBrokerNode();

        assert(dist[it.Node()->Index()] > -1);
        bn->depth = dist[it.Node()->Index()];

        it.Node()->data.ToTreeBrokerNode(bn);
        it.Edge()->data.ToTreeBrokerNode(bn);

        broker.PushTop(bn);
    }
    broker.AssignRanks();
    LOG_DBG << "Out: " << broker.Dump();
}

// -------------------------------------------------------------------------
//     Member Functions
// -------------------------------------------------------------------------

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
 * @brief Returns a distance matrix containing pairwise distances between all Nodes, using the
 * branch_length of the Edges as distance measurement.
 */
template <class NDT, class EDT>
Matrix<double>* Tree<NDT, EDT>::NodeDistanceMatrix()
{
    Matrix<double>* dist = new Matrix<double>(NodesSize(), NodesSize());

    // fill every row of the matrix
    for (NodeType* row_node : nodes_) {
        // set the diagonal element of the matrix.
        (*dist)(row_node->Index(), row_node->Index()) = 0.0;

        // the columns are filled using a levelorder traversal. this makes sure that for every node
        // we know how to calculate the distance to the current row node.
        for (
            IteratorLevelorder it = BeginLevelorder(row_node->Link());
            it != EndLevelorder();
            ++it
        ) {
            // skip the diagonal of the matrix.
            if (it.IsFirstIteration()) {
                continue;
            }

            // the distance to the current row node is: the length of the current branch plus
            // the distance from the other end of that branch to the row node.
            (*dist)(row_node->Index(), it.Node()->Index())
                = it.Edge()->data.branch_length
                + (*dist)(row_node->Index(), it.Link()->Outer()->Node()->Index());
        }
    }

    return dist;
}

/**
 * @brief Returns the number of Links of the Tree.
 */
template <class NDT, class EDT>
size_t Tree<NDT, EDT>::LinksSize() const
{
    return links_.size();
}

/**
 * @brief Returns the number of Nodes of the Tree.
 */
template <class NDT, class EDT>
size_t Tree<NDT, EDT>::NodesSize() const
{
    return nodes_.size();
}

/**
 * @brief Returns the number of Edges of the Tree.
 */
template <class NDT, class EDT>
size_t Tree<NDT, EDT>::EdgesSize() const
{
    return edges_.size();
}

// TODO make const! (need to add const versions of the tree iterators first...)
/**
 * @brief Returns true iff both trees have an identical topology.
 *
 * The topology is considered identical only if the order of edges is also the same in both trees.
 * This means, although two trees might have the same number of tips and branches, they might still
 * be not identical (with respect to this function) when the branches appear in a different order
 * or when the root sits at a different node.
 *
 * Thus, this function is mainly intended to check whether two trees have been produced from the
 * same input, for example from the same Newick file.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::HasIdenticalTopology(TreeType& right)
{
    // check array sizes
    if (this->links_.size() != right.links_.size() ||
        this->nodes_.size() != right.nodes_.size() ||
        this->edges_.size() != right.edges_.size()
    ) {
        return false;
    }

    // do a preorder traversal on both trees in parallel
    TreeType::IteratorPreorder it_l = this->BeginPreorder();
    TreeType::IteratorPreorder it_r = right.BeginPreorder();
    for (
        ;
        it_l != this->EndPreorder() && it_r != right.EndPreorder();
        ++it_l, ++it_r
    ) {
        // if the rank differs, we have a different topology
        if (it_l.Node()->Rank() != it_r.Node()->Rank()) {
            return false;
        }
    }

    // check whether we are done with both trees
    if (it_l != this->EndPreorder() || it_r != right.EndPreorder()) {
        return false;
    }

    return true;
}

/**
 * @brief Returns true iff both trees contain identical data on all their edges.
 *
 * See HasIdenticalData() for more information.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::HasIdenticalEdgeData(TreeType& right) const
{
    // check array size
    if (this->edges_.size() != right.edges_.size()) {
        return false;
    }

    // check edge data
    for (size_t i = 0; i < this->edges_.size(); ++i) {
        if (this->edges_[i]->data != right.edges_[i]->data) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Returns true iff both trees contain identical data on all their nodes.
 *
 * See HasIdenticalData() for more information.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::HasIdenticalNodeData(TreeType& right) const
{
    // check array sizes
    if (this->nodes_.size() != right.nodes_.size()) {
        return false;
    }

    // check node data
    for (size_t i = 0; i < this->nodes_.size(); ++i) {
        if (this->nodes_[i]->data != right.nodes_[i]->data) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Returns true iff both trees contain identical data on all their nodes and edges.
 *
 * It is first checked whether both trees have the same number of nodes and edges. It is however
 * not checked whether they have an identical topology. See HasIdenticalTopology() for this.
 * As this function relies on the order of nodes and edges in memory, it is however quite
 * improbable to have two trees with identical data but not identical topology.
 *
 * Thus, this function is mainly intended to check whether two trees have been produced from the
 * same input, for example from the same Newick file.
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::HasIdenticalData(TreeType& right) const
{
    return HasIdenticalEdgeData(right) && HasIdenticalNodeData(right);
}

// -------------------------------------------------------------------------
//     Dump and Debug Functions
// -------------------------------------------------------------------------


    // TODO introduce a validate function that checks the integrity of the tree:
    // TODO are all links, edges and nodes connected corretly to each other,
    // TODO is every one of them coverd exactly once when doing a full traversal?
    // TODO do all node->link_ links point to the root? same for all edge->primary?
    // TODO all objects coupled correctly?
    // TODO also, if we introduce indices to them for faster access, are those correct?
    // TODO this function will be curtial to ensure correctness of invariants once
    // TODO we start implementing stuff that modifies a tree (add nodes, move branches...)!
    // TODO do all iterators and check consistency! eg is a round trip covering every object (links onces, branches twice, nodes rank many times)?
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
 * @brief Returns the combined dump of DumpLinks(), DumpNodes() and DumpEdges().
 */
template <class NDT, class EDT>
std::string Tree<NDT, EDT>::DumpAll() const
{
    return DumpLinks() + "\n" + DumpNodes() + "\n" + DumpEdges();
}

/**
 * @brief Returns a list of all links including their next and outer link numbers as well as their
 * node and edge numbers.
 */
template <class NDT, class EDT>
std::string Tree<NDT, EDT>::DumpLinks() const
{
    std::ostringstream out;
    for (size_t i = 0; i < links_.size(); ++i) {
        out << "Link " << i
            << " \t Next: "  << LinkPointerToIndex(links_[i]->next_)
            << " \t Outer: " << LinkPointerToIndex(links_[i]->outer_)
            << " \t Node: "  << NodePointerToIndex(links_[i]->node_)
            << " \t Edge: "  << EdgePointerToIndex(links_[i]->edge_)
            << " \t " << links_[i]->Dump()
            << "\n";
    }
    return out.str();
}

/**
 * @brief Returns a list of all nodes including their name and the number of one of their links.
 */
template <class NDT, class EDT>
std::string Tree<NDT, EDT>::DumpNodes() const
{
    std::ostringstream out;
    for (size_t i = 0; i < nodes_.size(); ++i) {
        out << "Node " << i
            << " \t Link: " << LinkPointerToIndex(nodes_[i]->link_)
            << " \t " << nodes_[i]->Dump() << "\n";
    }
    return out.str();
}

/**
 * @brief Returns a list of all edges including their link numbers and branch lengths.
 */
template <class NDT, class EDT>
std::string Tree<NDT, EDT>::DumpEdges() const
{
    std::ostringstream out;
    for (size_t i = 0; i < edges_.size(); ++i) {
        out << "Edge " << i
            << " \t Link P: " << LinkPointerToIndex(edges_[i]->link_p_)
            << " \t Link S: " << LinkPointerToIndex(edges_[i]->link_s_)
            << " \t " << edges_[i]->Dump() << "\n";
    }
    return out.str();
}

/**
 * @brief Do a full tree traversal and return a list of all visited node names.
 *
 * Leaf nodes appear once in this list, while inner nodes appear every time the traversal visits
 * them. Thus, a node of rank 3 (meaning, it has three immediate children), is visited four times:
 * One time when coming from its parent, and then once each time the traversal returns from its
 * children.
 */
template <class NDT, class EDT>
std::string Tree<NDT, EDT>::DumpRoundtrip() const
{
    if (links_.empty()) {
        return "";
    }

    std::string out;
    LinkType* link = links_.front();

    do {
        out += link->node_->name_ + "\n";
        link = link->next_->outer_;
    } while (link != links_.front());

    return out;
}

/**
 * @brief Returns the index of a given link pointer within the link pointer array links_.
 *
 * This is useful for debugging purposes, particularly for the Dump functions.
 * Returns `-1` if the pointer was not found.
 */
template <class NDT, class EDT>
int Tree<NDT, EDT>::LinkPointerToIndex (LinkType* link) const
{
    for (size_t i = 0; i < links_.size(); ++i) {
        if (links_[i] == link) {
            assert(link->index_ == i);
            return i;
        }
    }
    return -1;
}

/**
 * @brief Returns the index of a given node pointer within the node pointer array nodes_.
 *
 * This is useful for debugging purposes, particularly for the Dump functions.
 * Returns `-1` if the pointer was not found.
 */
template <class NDT, class EDT>
int Tree<NDT, EDT>::NodePointerToIndex (NodeType* node) const
{
    for (size_t i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i] == node) {
            assert(node->index_ == i);
            return i;
        }
    }
    return -1;
}

/**
 * @brief Returns the index of a given edge pointer within the edge pointer array edges_.
 *
 * This is useful for debugging purposes, particularly for the Dump functions.
 * Returns `-1` if the pointer was not found.
 */
template <class NDT, class EDT>
int Tree<NDT, EDT>::EdgePointerToIndex (EdgeType* edge) const
{
    for (size_t i = 0; i < edges_.size(); ++i) {
        if (edges_[i] == edge) {
            assert(edge->index_ == i);
            return i;
        }
    }
    return -1;
}

} // namespace genesis
