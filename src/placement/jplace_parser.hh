#ifndef GNS_PLACEMENT_JPLACEPARSER_H_
#define GNS_PLACEMENT_JPLACEPARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>

#include "placement/placement_data.hh"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

class JsonDocument;
class JsonLexer;

template <class NodeDataType, class BranchDataType>
class Tree;

// =============================================================================
//     JplaceParser
// =============================================================================

class JplaceParser
{
public:
    static bool ProcessFile     (const std::string&  fn,     PlacementTree& tree);
    static bool ProcessString   (const std::string&  jplace, PlacementTree& tree);
    static bool ProcessLexer    (const JsonLexer&    lexer,  PlacementTree& tree);
    static bool ProcessDocument (const JsonDocument& doc,    PlacementTree& tree);
};

} // namespace genesis

#endif // include guard
