#ifndef GENESIS_PLACEMENT_IO_JPLACE_PROCESSOR_H_
#define GENESIS_PLACEMENT_IO_JPLACE_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class JsonDocument;
}

namespace placement {
    class PlacementMap;
    class PlacementMapSet;
}

// =================================================================================================
//     Jplace Processor
// =================================================================================================

namespace placement {

/**
 * @brief Parser and printer to process a Jplace document and create a PlacementMap object from it.
 *
 * The Jplace format is described in the following publication:
 *
 *     Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 *     A Format for Phylogenetic PlacementMap.
 *     PLoS ONE 7(2): e31009. doi:10.1371/journal.pone.0031009
 *     http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009
 *
 * See PlacementMap for the data structure used to store the Pqueries.
 */
class JplaceProcessor
{
public:

    static std::string get_version   ();
    static bool        check_version (const std::string version);

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    bool from_files    (const std::vector<std::string>& fns, PlacementMapSet& set);
    bool from_strings  (const std::vector<std::string>& jps, PlacementMapSet& set);

    bool from_file     (const std::string&  fn,         PlacementMap& placements);
    bool from_string   (const std::string&  jplace,     PlacementMap& placements);
    bool from_document (const utils::JsonDocument& doc, PlacementMap& placements);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    bool report_invalid_numbers  = false;
    bool correct_invalid_numbers = false;

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    bool        to_file     (const PlacementMap& placements,  const std::string   fn);
    void        to_string   (const PlacementMap& placements,        std::string&  jplace);
    std::string to_string   (const PlacementMap& placements);
    void        to_document (const PlacementMap& placements, utils::JsonDocument& doc);
};

} // namespace placement
} // namespace genesis

#endif // include guard
