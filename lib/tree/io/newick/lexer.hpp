#ifndef GENESIS_TREE_IO_NEWICK_LEXER_H_
#define GENESIS_TREE_IO_NEWICK_LEXER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/io/lexer.hpp"

namespace genesis {

// =============================================================================
//     Newick Lexer
// =============================================================================

class NewickLexer : public Lexer
{
public:

    NewickLexer();

private:

    bool scan_comment() override;

    bool scan_number() override;

    bool scan_tag() override;
};

} // namespace genesis

#endif // include guard
