/**
 * @brief Implementation of basic tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree.hh"

#include <sstream>

#include "tree/newick_parser.hh"
#include "utils/logging.hh"
#include "utils/utils.hh"

namespace genesis {

Tree::~Tree ()
{
    clear();
}

void Tree::clear()
{
    for (TreeBranch* branch : branches_) {
        delete branch;
    }
    for (TreeLink* link : links_) {
        delete link;
    }
    for (TreeNode* node : nodes_) {
        delete node;
    }

    std::vector<TreeBranch*>().swap(branches_);
    std::vector<TreeLink*>().swap(links_);
    std::vector<TreeNode*>().swap(nodes_);
}

bool Tree::FromNewickFile (const std::string& fn)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Newick file '" << fn << "' does not exist.";
        return false;
    }
    return FromNewickString(FileRead(fn));
}

bool Tree::FromNewickString (const std::string& tree)
{
    TreeBroker broker;
    if (!NewickParser::ProcessString(tree, broker)) {
        return false;
    }

    broker.AssignRanks();
    LOG_INFO << broker.Dump();
    FromTreeBroker(broker);
    return true;
}

void Tree::FromTreeBroker (const TreeBroker& broker)
{
    clear();
    std::vector<TreeLink*> link_stack;

    for (TreeBroker::const_iterator b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        TreeBrokerNode* bnode = *b_itr;

        // create the tree node for this broker node
        TreeNode* cur_node = new TreeNode();
        cur_node->name_ = bnode->name;
        nodes_.push_back(cur_node);

        // create the link that points towards the root
        TreeLink* up_link = new TreeLink();
        up_link->node_ = cur_node;
        cur_node->link_ = up_link;
        links_.push_back(up_link);

        // establish the link towards the root
        // (link_stack is only empty in the very beginning - for the root itself)

        if (link_stack.empty()) {
            up_link->outer_ = up_link;
        } else {
            up_link->outer_ = link_stack.back();
            link_stack.back()->outer_ = up_link;
            link_stack.pop_back();
        }

        if (bnode->rank() == 0) {
            up_link->next_ = up_link;
        } else {
            TreeLink* next_link = up_link;
            for (int i = 0; i < bnode->rank(); ++i) {
                TreeLink* down_link = new TreeLink();
                down_link->next_ = next_link;
                next_link = down_link;

                down_link->node_ = cur_node;
                links_.push_back(down_link);
                link_stack.push_back(down_link);
            }
            up_link->next_ = next_link;
        }
    }
}

std::string Tree::DumpBranches() const
{
    std::ostringstream out;
    for (size_t i = 0; i < branches_.size(); ++i) {
        out << "Branch " << i
            << " \t link p "  << LinkPointerToIndex(branches_[i]->link_p_)
            << " \t link q " << LinkPointerToIndex(branches_[i]->link_q_)
            << "\n";
    }
    return out.str();
}

std::string Tree::DumpLinks() const
{
    std::ostringstream out;
    for (size_t i = 0; i < links_.size(); ++i) {
        out << "Link " << i
            << " \t next "  << LinkPointerToIndex(links_[i]->next_)
            << " \t outer " << LinkPointerToIndex(links_[i]->outer_)
            << " \t node " << NodePointerToIndex(links_[i]->node_)
            << " \t branch " << BranchPointerToIndex(links_[i]->branch_)
            << "\n";
    }
    return out.str();
}

std::string Tree::DumpNodes() const
{
    std::ostringstream out;
    for (size_t i = 0; i < nodes_.size(); ++i) {
        out << "Node " << i
            << " \t name " << nodes_[i]->name_
            << " \t link "  << LinkPointerToIndex(nodes_[i]->link_)
            << "\n";
    }
    return out.str();
}

std::string Tree::DumpRound() const
{
    std::string out;
    TreeLink* link = links_.front();

    do {
        out += link->node_->name_ + "\n";
        link = link->next_;
        link = link->outer_;
    } while (link != links_.front());

    return out;
}

/**
 * @brief Returns the index of a given branch pointer within the branch pointer array branches_.
 *
 * This is useful for debugging purposes, particularly for the Dump functions.
 * Returns `-1` if the pointer was not found.
 */
int Tree::BranchPointerToIndex (TreeBranch* branch) const
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
int Tree::LinkPointerToIndex (TreeLink* link) const
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
int Tree::NodePointerToIndex (TreeNode* node) const
{
    for (size_t i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i] == node) {
            return i;
        }
    }
    return -1;
}

} // namespace genesis
