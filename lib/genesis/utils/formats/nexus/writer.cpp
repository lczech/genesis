/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/formats/nexus/writer.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/formats/nexus/document.hpp"
#include "genesis/utils/io/output_stream.hpp"

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
    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    to_stream( doc, ofs );
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
