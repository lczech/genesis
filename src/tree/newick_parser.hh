#ifndef GNS_TREE_NEWICKPARSER_H_
#define GNS_TREE_NEWICKPARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/newick_lexer.hh"
#include "tree/tree_broker.hh"

namespace genesis {

class NewickParser
{
public:
    static bool Process (const std::string& tree,  TreeBroker& broker);
    static bool Process (const NewickLexer& lexer, TreeBroker& broker);
};

} // namespace genesis

#endif // include guard
