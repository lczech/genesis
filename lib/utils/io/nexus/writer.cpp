/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/nexus/writer.hpp"

#include "utils/core/fs.hpp"
#include "utils/io/nexus/document.hpp"

#include <stdexcept>
#include <fstream>
#include <ostream>
#include <sstream>

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

void NexusWriter::to_file( NexusDocument const& doc, std::string const& filename) const
{
    if( utils::file_exists(filename) ) {
        throw std::runtime_error( "Nexus file '" + filename + "' already exists." );
    }

    std::ofstream fstr( filename );
    if( !fstr ) {
        throw std::runtime_error( "Cannot write Nexus file '" + filename + "'." );
    }
    to_stream( doc, fstr );
    fstr.close();
}

void NexusWriter::to_string( NexusDocument const& doc, std::string& output) const
{
    std::stringstream sstr;
    to_stream( doc, sstr );
    output = sstr.str();
}

std::string NexusWriter::to_string( NexusDocument const& doc) const
{
    std::stringstream sstr;
    to_stream( doc, sstr );
    return sstr.str();
}

} // namespace utils
} // namespace genesis
