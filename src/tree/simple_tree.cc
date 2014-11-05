#include "simple_tree.hh"
#include "../utils/log.hh"
#include "../utils/lexer.hh"

namespace genesis {

using namespace utils;

//~ void SimpleTree::ReadNewickFile (std::string fn)
//~ {
    //~ return;
//~ }

void SimpleTree::ParseNewickString (std::string tree)
{
    utils::Lexer lex;
    lex.Analyze(tree);

    LOG_BOLD << lex.Dump();
}

} // namespace genesis
