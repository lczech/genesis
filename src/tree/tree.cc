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
    //~ lexer.Dump();

    TreeBroker broker;
    NewickParser::Process(tree, broker);
    broker.Dump();
    //~ LOG_INFO << "Nodes: " << parser.nodes() << ", Leaves: " << parser.leaves();
    //~ LOG_INFO;
}

void Tree::Init (const int leaf_count)
{
    int inner_count = leaf_count - 1;

    clear();
    branches_.reserve(leaf_count +     inner_count - 1);
    links_.reserve   (leaf_count + 3 * inner_count    );
    nodes_.reserve   (leaf_count +     inner_count    );

    for (int i = 0; i < leaf_count; ++i) {
        links_[i] = new TreeLink();
        links_[i]->next_ = links_[i];
    }
    for (int i = leaf_count; i < leaf_count + inner_count; ++i) {
        //~ for (int j = 0; j < 3; ++j) {
            //~ links_[i] = new TreeLink();
        //~ }
    }
}

} // namespace genesis
