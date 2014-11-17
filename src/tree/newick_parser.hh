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
#include "tree/tree_agent.hh"

namespace genesis {

class NewickParser
{
public:
    static bool Process (const std::string& tree,  TreeAgent* agent);
    static bool Process (const NewickLexer& lexer, TreeAgent* agent);

protected:
    static bool FillTreeAgent (const NewickLexer& lexer, TreeAgent* agent);
};

} // namespace genesis

#endif // include guard
