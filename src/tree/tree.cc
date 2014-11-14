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

void Tree::FromNewickFile (std::string fn)
{
    // TODO make sure file exists!
    // (readfile checks this, too, but simply returns "", which yields the wrong message when
    // running the parser...)
    FromNewickString(ReadFile(fn));
}

void Tree::FromNewickString (std::string tree)
{
    NewickLexer lexer;
    lexer.Process(tree);
    //~ LOG_INFO << lexer.Dump();

    NewickParser parser;
    parser.Process(lexer);
    LOG_INFO << parser.Dump();
    LOG_INFO << "Nodes: " << parser.nodes() << ", Leaves: " << parser.leaves();
    LOG_INFO;
}

} // namespace genesis
