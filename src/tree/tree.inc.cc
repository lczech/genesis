/**
 * @brief Implementation of basic tree functions.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and BranchDataType are abbreviated NDT and BDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <algorithm>
#include <sstream>

#include "tree/newick_parser.hh"
#include "utils/logging.hh"
#include "utils/utils.hh"

namespace genesis {

// -------------------------------------------------------------------------
//     Construction and Destruction
// -------------------------------------------------------------------------

/**
 * @brief Deletes all data of the tree, including all links, nodes and branches.
 */
template <class NDT, class BDT>
void Tree<NDT, BDT>::clear()
{
    for (TreeBranch<NDT, BDT>* branch : branches_) {
        delete branch;
    }
    for (TreeLink<NDT, BDT>* link : links_) {
        delete link;
    }
    for (TreeNode<NDT, BDT>* node : nodes_) {
        delete node;
    }

    std::vector<TreeBranch<NDT, BDT>*>().swap(branches_);
    std::vector<TreeLink<NDT, BDT>*>().swap(links_);
    std::vector<TreeNode<NDT, BDT>*>().swap(nodes_);
}

/**
 * @brief Destructor. Calls clear() to free all memory used by the tree and its substructures.
 */
template <class NDT, class BDT>
Tree<NDT, BDT>::~Tree ()
{
    clear();
}

// -------------------------------------------------------------------------
//     Create Tree from different sources
// -------------------------------------------------------------------------

/**
 * @brief Create a Tree from a file containing a Newick tree.
 */
template <class NDT, class BDT>
bool Tree<NDT, BDT>::FromNewickFile (const std::string& fn)
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
template <class NDT, class BDT>
bool Tree<NDT, BDT>::FromNewickString (const std::string& tree)
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
 */
template <class NDT, class BDT>
void Tree<NDT, BDT>::FromTreeBroker (TreeBroker& broker)
{
    clear();
    std::vector<TreeLink<NDT, BDT>*> link_stack;

    // we need the ranks (number of immediate children) of all nodes
    broker.AssignRanks();

    // iterate over all nodes of the tree broker
    for (TreeBroker::const_iterator b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        TreeBrokerNode* broker_node = *b_itr;

        // create the tree node for this broker node
        TreeNode<NDT, BDT>* cur_node = new TreeNode<NDT, BDT>();
        cur_node->FromTreeBrokerNode(broker_node);
        nodes_.push_back(cur_node);

        // create the link that points towards the root.
        // this link is created for every node, root, inner and leaves.
        TreeLink<NDT, BDT>* up_link = new TreeLink<NDT, BDT>();
        up_link->node_ = cur_node;
        cur_node->link_ = up_link;
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

            // also, create a branch that connects both nodes
            TreeBranch<NDT, BDT>* up_branch = new TreeBranch<NDT, BDT>();
            up_branch->link_p_         = link_stack.back();
            up_branch->link_s_         = up_link;
            up_link->branch_           = up_branch;
            link_stack.back()->branch_ = up_branch;
            up_branch->FromTreeBrokerNode(broker_node);
            branches_.push_back(up_branch);

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
        TreeLink<NDT, BDT>* next_link = up_link;
        for (int i = 0; i < broker_node->rank(); ++i) {
            TreeLink<NDT, BDT>* down_link = new TreeLink<NDT, BDT>();
            down_link->next_ = next_link;
            next_link = down_link;

            down_link->node_ = cur_node;
            links_.push_back(down_link);
            link_stack.push_back(down_link);
        }
        up_link->next_ = next_link;
    }
}

// -------------------------------------------------------------------------
//     Member Functions
// -------------------------------------------------------------------------

template <class NDT, class BDT>
int Tree<NDT, BDT>::MaxRank() const
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

template <class NDT, class BDT>
bool Tree<NDT, BDT>::IsBifurcating() const
{
    return MaxRank() == 2;
}

template <class NDT, class BDT>
bool Tree<NDT, BDT>::Validate() const
{
    // check that the member arrays are valid: if at least one of them is empty, the tree is not
    // fully initialized, so either it is a new tree without any data (all arrays empty, valid),
    // or some are empty, but others not (not valid).
    if (links_.empty() || nodes_.empty() || branches_.empty()) {
        bool emp = links_.empty() && nodes_.empty() && branches_.empty();
        if (emp) {
            LOG_INFO << "Tree is empty.";
        } else {
            LOG_INFO << "Tree is not empty, but one of its data members is.";
        }
        return emp;
    }

    // if we are here, all three arrays contain data, so we can start a full traversal along all
    // links.
    TreeLink<NDT, BDT>* link = links_.front();
    do {
        link = link->next_->outer_;
    } while (link != links_.front());

    return true;
}

// -------------------------------------------------------------------------
//     Dump and Debug Functions
// -------------------------------------------------------------------------

/**
 * @brief Returns the combined dump of DumpLinks(), DumpNodes() and DumpBranches().
 */
template <class NDT, class BDT>
std::string Tree<NDT, BDT>::DumpAll() const
{
    return DumpLinks() + "\n" + DumpNodes() + "\n" + DumpBranches();
}

/**
 * @brief Returns a list of all branches including their link numbers and branch lengths.
 */
template <class NDT, class BDT>
std::string Tree<NDT, BDT>::DumpBranches() const
{
    std::ostringstream out;
    for (size_t i = 0; i < branches_.size(); ++i) {
        out << "Branch " << i
            << " \t Link P: " << LinkPointerToIndex(branches_[i]->link_p_)
            << " \t Link Q: " << LinkPointerToIndex(branches_[i]->link_s_)
            << " \t " << branches_[i]->Dump() << "\n";
    }
    return out.str();
}

/**
 * @brief Returns a list of all links including their next and outer link numbers as well as their
 * node and branch numbers.
 */
template <class NDT, class BDT>
std::string Tree<NDT, BDT>::DumpLinks() const
{
    std::ostringstream out;
    for (size_t i = 0; i < links_.size(); ++i) {
        out << "Link " << i
            << " \t Next: "   << LinkPointerToIndex(links_[i]->next_)
            << " \t Outer: "  << LinkPointerToIndex(links_[i]->outer_)
            << " \t Node: "   << NodePointerToIndex(links_[i]->node_)
            << " \t Branch: " << BranchPointerToIndex(links_[i]->branch_)
            << " \t " << links_[i]->Dump()
            << "\n";
    }
    return out.str();
}

/**
 * @brief Returns a list of all nodes including their name and the number of one of their links.
 */
template <class NDT, class BDT>
std::string Tree<NDT, BDT>::DumpNodes() const
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
 * @brief Do a full tree traversal and return a list of all visited node names.
 *
 * Leaf nodes appear once in this list, while inner nodes appear every time the traversal visits
 * them. Thus, a node of rank 3 (meaning, it has three immediate children), is visited four times:
 * One time when coming from its parent, and then once each time the traversal returns from its
 * children.
 */
template <class NDT, class BDT>
std::string Tree<NDT, BDT>::DumpRoundtrip() const
{
    if (links_.empty()) {
        return "";
    }

    std::string out;
    TreeLink<NDT, BDT>* link = links_.front();

    do {
        out += link->node_->name_ + "\n";
        link = link->next_->outer_;
    } while (link != links_.front());

    return out;
}

/**
 * @brief Returns the index of a given branch pointer within the branch pointer array branches_.
 *
 * This is useful for debugging purposes, particularly for the Dump functions.
 * Returns `-1` if the pointer was not found.
 */
template <class NDT, class BDT>
int Tree<NDT, BDT>::BranchPointerToIndex (TreeBranch<NDT, BDT>* branch) const
{
    for (size_t i = 0; i < branches_.size(); ++i) {
        if (branches_[i] == branch) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Returns the index of a given link pointer within the link pointer array links_.
 *
 * This is useful for debugging purposes, particularly for the Dump functions.
 * Returns `-1` if the pointer was not found.
 */
template <class NDT, class BDT>
int Tree<NDT, BDT>::LinkPointerToIndex (TreeLink<NDT, BDT>* link) const
{
    for (size_t i = 0; i < links_.size(); ++i) {
        if (links_[i] == link) {
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
template <class NDT, class BDT>
int Tree<NDT, BDT>::NodePointerToIndex (TreeNode<NDT, BDT>* node) const
{
    for (size_t i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i] == node) {
            return i;
        }
    }
    return -1;
}

} // namespace genesis
