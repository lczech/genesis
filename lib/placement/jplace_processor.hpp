#ifndef GENESIS_PLACEMENT_JPLACE_PROCESSOR_H_
#define GENESIS_PLACEMENT_JPLACE_PROCESSOR_H_

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
class PlacementMap;

// =============================================================================
//     Jplace Processor
// =============================================================================

/**
 * @brief Parser and printer to process a Jplace document and create a PlacementMap object from it.
 *
 * The Jplace format is described in the following publication:
 * Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 * A Format for Phylogenetic PlacementMap.
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

    static bool report_invalid_numbers;
    static bool correct_invalid_numbers;

    static bool FromFile     (const std::string&  fn,     PlacementMap& placements);
    static bool FromString   (const std::string&  jplace, PlacementMap& placements);
    static bool FromDocument (const JsonDocument& doc,    PlacementMap& placements);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    static bool        ToFile     (const std::string   fn,     const PlacementMap& placements);
    static void        ToString   (      std::string&  jplace, const PlacementMap& placements);
    static std::string ToString   (                            const PlacementMap& placements);
    static void        ToDocument (      JsonDocument& doc,    const PlacementMap& placements);
};

} // namespace genesis

#endif // include guard
