#ifndef GENESIS_PLACEMENT_IO_SERIALIZER_H_
#define GENESIS_PLACEMENT_IO_SERIALIZER_H_

/**
 * @brief Header of SampleSerializer class.
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

class Sample;

// =================================================================================================
//     SampleSerializer
// =================================================================================================

/**
 * @brief
 */
class SampleSerializer
{
public:

    static void save (const Sample& map, const std::string& file_name);
    static void load (const std::string& file_name, Sample& map);

    static unsigned char version;

};

} // namespace placement
} // namespace genesis

#endif // include guard
