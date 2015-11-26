#ifndef GENESIS_TREE_IO_NEWICK_PARSER_H_
#define GENESIS_TREE_IO_NEWICK_PARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/io/newick/lexer.hpp"

#include <string>

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class  NewickBroker;

// =================================================================================================
//     Newick Parser and Generator
// =================================================================================================

bool parse_newick_tree (
    NewickLexer::iterator      & ct,
    NewickLexer::iterator const& end,
    NewickBroker&                broker
);

std::string generate_newick_tree (
    NewickBroker const& broker
);

} // namespace genesis

#endif // include guard
