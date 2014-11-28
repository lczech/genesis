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

    LOG_DBG << Dump();
    LOG_DBG << DumpLinks();
    LOG_DBG << DumpNodes();
    LOG_DBG << DumpBranches();
}

void Tree::xFromTreeBroker (const TreeBroker& broker)
{
    // TODO make sure assign ranks is called before!
    // TODO this function only works for bifurcating trees!

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // CAVEAT: THIS FUNCTION IS NOT WORKING!!!!!!!!
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    int leaf_count = broker.LeafCount();
    InitBifurcatingTree(leaf_count);

    int leaf_i  = 0;
    int inner_i = leaf_count;
    TreeLink* link;
    std::vector<TreeLink*> link_stack;

    for (TreeBroker::const_iterator b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        TreeBrokerNode* bnode = *b_itr;

        LOG_DBG2 << "bnode " << bnode->name << ", stack:\n";
        for (size_t i = 0; i < link_stack.size(); ++i) {
            LOG_DBG2 << "    " << LinkPointerToIndex(link_stack[i]);
        }

        if (link_stack.empty()) {
            link_stack.push_back(links_[inner_i]);
            link_stack.push_back(links_[inner_i]->next_);
            link_stack.push_back(links_[inner_i]->next_->next_);
            inner_i += 3;
            continue;
        }

        link = link_stack.back();
        link_stack.pop_back();

        if (bnode->rank() > 0) {
            link->outer_ = links_[inner_i];
            links_[inner_i]->outer_ = link;

            link_stack.push_back(links_[inner_i]->next_);
            link_stack.push_back(links_[inner_i]->next_->next_);
            inner_i += 3;
        } else {
            link->outer_ = links_[leaf_i];
            links_[leaf_i]->outer_ = link;
            ++leaf_i;
        }

        link->node_->name_ = bnode->name;
        LOG_DBG2 << "    link " << LinkPointerToIndex(link) << " points to link " << LinkPointerToIndex(link->outer_);
        LOG_DBG2 << "    and to node " << NodePointerToIndex(link->node_) << " with name " << link->node_->name_;
        LOG_DBG2 << "============================";
    }

    for (size_t i = 0; i < nodes_.size(); ++i) {
        LOG_DBG1 << nodes_[i]->name_;
    }

    LOG_DBG << "leaf count " << leaf_count << " --> inner count " << broker.InnerCount();
    LOG_DBG << "leaf i " << leaf_i << "; inner_i " << inner_i;

    LOG_DBG << DumpLinks();
    LOG_DBG << DumpNodes();
    LOG_DBG << DumpBranches();
}

void Tree::InitBifurcatingTree (const int leaf_count)
{
    int inner_count = leaf_count - 1;

    clear();
    branches_.resize(leaf_count +     inner_count - 1);
    links_.resize   (leaf_count + 3 * inner_count    );
    nodes_.resize   (leaf_count +     inner_count    );

    // TODO we allocate on instance at a time here, which might be slow. if so, it could help doing this:
    // TreeLink* arr = new TreeLink() [count];
    // for i = 0 .. count
    //     links_[i] = arr++;
    // and then just forget about arr, because all its content is pointed to by links_, so its not lost.

    // alloc and init leaf links and nodes
    for (int i = 0; i < leaf_count; ++i) {
        links_[i] = new TreeLink();
        links_[i]->next_  = links_[i];
        links_[i]->outer_ = nullptr;

        nodes_[i] = new TreeNode();
        nodes_[i]->link_ = links_[i];
        links_[i]->node_ = nodes_[i];
    }

    // alloc and init inner links and nodes
    for (int i = 0; i < inner_count; ++i) {
        int cn = leaf_count + i; // current node index
        nodes_[cn] = new TreeNode();

        // create three links per inner node and link their next_ pointers to build a circle
        TreeLink* round_link = nullptr;
        for (int j = 0; j < 3; ++j) {
            int cl = leaf_count + i * 3 + j; // current link index

            links_[cl] = new TreeLink();
            links_[cl]->next_  = round_link;
            links_[cl]->outer_ = nullptr;
            round_link = links_[cl];

            links_[cl]->node_ = nodes_[cn];
        }
        links_[leaf_count + i * 3]->next_ = round_link;

        // the node points to the first of the corresponding links
        nodes_[cn]->link_ = links_[leaf_count + i * 3];
    }

    // alloc and init all branches
    for (int i = 0; i < leaf_count + inner_count - 1; ++i) {
        branches_[i] = new TreeBranch();
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
