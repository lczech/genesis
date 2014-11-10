#ifndef GNS_TREE_NEWICKLEXER_H_
#define GNS_TREE_NEWICKLEXER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "node.hh"
#include "../utils/lexer.hh"

namespace genesis {
namespace utils {

class NewickLexer : public Lexer
{
    public:
        NewickLexer() {
            SetCharType("[]",  kComment);
            SetCharType(".",   kNumber);
            SetCharType("'",   kString);
            SetCharType(",:;", kOperator);
            SetCharType("()",  kBracket);
            SetCharType("{}",  kTag);
        }
};

} // namespace utils
} // namespace genesis

#endif // include guard
