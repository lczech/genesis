#include "tree.hh"
#include "../utils/log.hh"
#include "../utils/utils.hh"
#include "../utils/lexer.hh"

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
