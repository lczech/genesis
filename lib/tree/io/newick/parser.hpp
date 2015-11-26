#ifndef GENESIS_TREE_IO_NEWICK_PARSER_H_
#define GENESIS_TREE_IO_NEWICK_PARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/io/newick/lexer.hpp"

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class  NewickBroker;

// =================================================================================================
//     Newick Parser
// =================================================================================================

bool parse_newick_tree (
          NewickLexer::iterator& ct,
    const NewickLexer::iterator& end,
          NewickBroker&          broker
);

} // namespace genesis

#endif // include guard
