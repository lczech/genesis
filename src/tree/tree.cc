/**
 * @brief Implementation of basic tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "../utils/logging.hh"
#include "../utils/utils.hh"
#include "tree.hh"
#include "newick_lexer.hh"
#include "newick_parser.hh"

namespace genesis {

void Tree::FromNewickFile (std::string fn)
{
    FromNewickString(ReadFile(fn));
}

void Tree::FromNewickString (std::string tree)
{
    NewickLexer lexer;
    lexer.Process(tree);
    LOG_INFO << lexer.Dump();

    NewickParser parser;
    parser.Process(lexer);
    LOG_INFO << parser.Dump();
}

} // namespace genesis
