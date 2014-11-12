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

void Tree::FromNewickFile (std::string fn)
{
    FromNewickString(ReadFile(fn));
}

void Tree::FromNewickString (std::string tree)
{
    NewickLexer lex;
    lex.Process(tree);

    LOG_BOLD << lex.Dump();
}

} // namespace genesis
