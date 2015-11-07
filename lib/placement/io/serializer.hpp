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

    static bool save (const PlacementMap& map, const std::string& file_name);
    static bool load (const std::string& file_name, PlacementMap& map);

    static unsigned char version;

};

} // namespace genesis

#endif // include guard