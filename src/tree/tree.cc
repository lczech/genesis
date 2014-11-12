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

namespace genesis {

void Tree::ReadNewickFile (std::string fn)
{
    ParseNewickString(ReadFile(fn));
}

void Tree::ParseNewickString (std::string tree)
{
    NewickLexer lex;
    lex.Analyze(tree);

    LOG_BOLD << lex.Dump();
}

} // namespace genesis
