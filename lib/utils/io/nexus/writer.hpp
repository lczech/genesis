#ifndef GENESIS_UTILS_IO_NEXUS_WRITER_H_
#define GENESIS_UTILS_IO_NEXUS_WRITER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <iosfwd>

namespace genesis {
namespace nexus {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Document;

// =================================================================================================
//     Nexus Writer
// =================================================================================================

/**
 * @brief
 */
class Writer
{
    // ---------------------------------------------------------------------
    //     Write Functions
    // ---------------------------------------------------------------------

public:

    void to_stream( Document const& doc, std::ostream& out );
};

} // namespace nexus
} // namespace genesis

#endif // include guard
