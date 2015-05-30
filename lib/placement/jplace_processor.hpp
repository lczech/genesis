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

    static std::string get_version   ();
    static bool        check_version (const std::string version);

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    static bool report_invalid_numbers;
    static bool correct_invalid_numbers;

    static bool from_file     (const std::string&  fn,     PlacementMap& placements);
    static bool from_string   (const std::string&  jplace, PlacementMap& placements);
    static bool from_document (const JsonDocument& doc,    PlacementMap& placements);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    static bool        to_file     (const PlacementMap& placements, const std::string   fn);
    static void        to_string   (const PlacementMap& placements,       std::string&  jplace);
    static std::string to_string   (const PlacementMap& placements);
    static void        to_document (const PlacementMap& placements,       JsonDocument& doc);
};

} // namespace genesis

#endif // include guard
