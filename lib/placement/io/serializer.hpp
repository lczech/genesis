#ifndef GENESIS_PLACEMENT_IO_SERIALIZER_H_
#define GENESIS_PLACEMENT_IO_SERIALIZER_H_

/**
 * @brief Header of PlacementMapSerializer class.
 *
 * @file
 * @ingroup placement
 */

#include <string>

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class PlacementMap;

// =================================================================================================
//     PlacementMapSerializer
// =================================================================================================

/**
 * @brief
 */
class PlacementMapSerializer
{
public:

    static void save (const PlacementMap& map, const std::string& file_name);
    static void load (const std::string& file_name, PlacementMap& map);

    static unsigned char version;

};

} // namespace placement
} // namespace genesis

#endif // include guard
