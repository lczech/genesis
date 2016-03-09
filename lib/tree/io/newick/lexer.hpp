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
namespace tree {

// =============================================================================
//     Newick Lexer
// =============================================================================

class NewickLexer : public utils::Lexer
{
public:

    NewickLexer();

private:

    bool scan_comment() override;

    bool scan_number() override;

    bool scan_tag() override;
};

} // namespace tree
} // namespace genesis

#endif // include guard
