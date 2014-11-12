/**
 * @brief Implementation of basic tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree.hh"
#include "../utils/log.hh"
#include "../utils/utils.hh"
#include "newick_lexer.hh"

namespace genesis {
namespace tree {

void Tree::ReadNewickFile (std::string fn)
{
    ParseNewickString(utils::ReadFile(fn));
}

void Tree::ParseNewickString (std::string tree)
{
    utils::NewickLexer lex;
    lex.Analyze(tree);

    LOG_BOLD << lex.Dump();
}

} // namespace tree
} // namespace genesis
