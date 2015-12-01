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
namespace nexus {

// =================================================================================================
//     Nexus Writer
// =================================================================================================

void Writer::to_stream( Document const& doc, std::ostream& out )
{
    out << "#NEXUS\n";

    for(auto& block : doc) {
        out << "\nBEGIN " << block->block_name() << ";\n";
        out << *block;
        out << "END;\n";
    }
}

} // namespace nexus
} // namespace genesis
