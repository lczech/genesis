#ifndef GNS_PLACEMENT_JPLACEPARSER_H_
#define GNS_PLACEMENT_JPLACEPARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

class JsonDocument;
class JsonLexer;
class Placements;

// =============================================================================
//     JplaceParser
// =============================================================================

class JplaceParser
{
public:
    static bool ProcessFile     (const std::string&  fn,     Placements& placements);
    static bool ProcessString   (const std::string&  jplace, Placements& placements);
    static bool ProcessLexer    (const JsonLexer&    lexer,  Placements& placements);
    static bool ProcessDocument (const JsonDocument& doc,    Placements& placements);

    /**
     * @brief The version of jplace format that this parser is built for.
     */
    static const int version = 3;
};

} // namespace genesis

#endif // include guard
