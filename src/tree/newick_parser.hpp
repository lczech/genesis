#ifndef GNS_TREE_NEWICKPARSER_H_
#define GNS_TREE_NEWICKPARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

class NewickLexer;
class TreeBroker;

// =============================================================================
//     NewickParser
// =============================================================================

class NewickParser
{
public:
    static bool ProcessFile   (const std::string& fn,    TreeBroker& broker);
    static bool ProcessString (const std::string& tree,  TreeBroker& broker);
    static bool ProcessLexer  (const NewickLexer& lexer, TreeBroker& broker);
};

} // namespace genesis

#endif // include guard
