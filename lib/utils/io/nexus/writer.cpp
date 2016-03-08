/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/nexus/writer.hpp"

#include "utils/io/nexus/document.hpp"

#include <ostream>

namespace genesis {
namespace utils {

// =================================================================================================
//     Nexus Writer
// =================================================================================================

void NexusWriter::to_stream( NexusDocument const& doc, std::ostream& out ) const
{
    out << "#NEXUS\n";

    for(auto& block : doc) {
        out << "\nBEGIN " << block->block_name() << ";\n";
        out << *block;
        out << "END;\n";
    }
}

} // namespace utils
} // namespace genesis
