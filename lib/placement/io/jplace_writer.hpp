#ifndef GENESIS_PLACEMENT_IO_JPLACE_WRITER_H_
#define GENESIS_PLACEMENT_IO_JPLACE_WRITER_H_

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
//     Jplace Writer
// =================================================================================================

namespace placement {

class JplaceWriter
{
public:

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
