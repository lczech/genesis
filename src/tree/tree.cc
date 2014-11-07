#include "tree.hh"

namespace genesis {

void Tree::ReadNewickFile (std::string fn)
{
    ParseNewickString(utils::ReadFile(fn));
}

void Tree::ParseNewickString (std::string tree)
{
    utils::Lexer lex;
    lex.Analyze(tree);

    LOG_BOLD << lex.Dump();


}

}
