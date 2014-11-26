/**
 * @brief Implementation of basic tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree.hh"

#include "tree/newick_lexer.hh"
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

void Tree::FromNewickFile (const std::string& fn)
{
    // TODO make sure file exists!
    // (readfile checks this, too, but simply returns "", which yields the wrong message when
    // running the parser...)
    FromNewickString(FileRead(fn));
}

void Tree::FromNewickString (const std::string& tree)
{
    //~ NewickLexer lexer;
    //~ lexer.Process(tree);
    //~ LOG_INFO << lexer.Dump();

    TreeBroker broker;
    NewickParser::Process(tree, broker);
    LOG_INFO << broker.Dump();
    //~ LOG_INFO << "Nodes: " << parser.nodes() << ", Leaves: " << parser.leaves();
    //~ LOG_INFO;

    FromTreeBroker(broker);
}

void Tree::FromTreeBroker (const TreeBroker& broker)
{
    InitBifurcatingTree(broker.LeafCount());
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

} // namespace genesis
