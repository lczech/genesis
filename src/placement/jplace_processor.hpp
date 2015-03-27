#ifndef GNS_PLACEMENT_JPLACEPROCESSOR_H_
#define GNS_PLACEMENT_JPLACEPROCESSOR_H_

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
class PlacementSet;

// =============================================================================
//     Jplace Processor
// =============================================================================

/**
 * @brief Parser and printer to process a Jplace document and create a PlacementSet object from it.
 *
 * The Jplace format is described in the following publication:
 * Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 * A Format for Phylogenetic PlacementSet.
 * PLoS ONE 7(2): e31009. doi:10.1371/journal.pone.0031009
 * http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009
 */
class JplaceProcessor
{
public:

    static std::string GetVersion   ();
    static bool        CheckVersion (const std::string version);

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    static bool FromFile     (const std::string&  fn,     PlacementSet& placements);
    static bool FromString   (const std::string&  jplace, PlacementSet& placements);
    static bool FromDocument (const JsonDocument& doc,    PlacementSet& placements);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    static bool        ToFile     (const std::string   fn,     const PlacementSet& placements);
    static void        ToString   (      std::string&  jplace, const PlacementSet& placements);
    static std::string ToString   (                            const PlacementSet& placements);
    static void        ToDocument (      JsonDocument& doc,    const PlacementSet& placements);
};

} // namespace genesis

#endif // include guard
