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
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class NexusDocument;

// =================================================================================================
//     Nexus Writer
// =================================================================================================

/**
 * @brief
 */
class NexusWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    NexusWriter()  = default;
    ~NexusWriter() = default;

    NexusWriter( NexusWriter const& ) = default;
    NexusWriter( NexusWriter&& )      = default;

    NexusWriter& operator= ( NexusWriter const& ) = default;
    NexusWriter& operator= ( NexusWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Write Functions
    // ---------------------------------------------------------------------

    void to_stream( NexusDocument const& doc, std::ostream& out ) const;
};

} // namespace utils
} // namespace genesis

#endif // include guard
