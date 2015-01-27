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

/**
 * @brief Parser to process a Jplace document and create a Placements object from it.
 *
 * The Jplace format is described in the following publication:
 * Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 * A Format for Phylogenetic Placements.
 * PLoS ONE 7(2): e31009. doi:10.1371/journal.pone.0031009
 * http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009
 */
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
