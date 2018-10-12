/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab,  Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35,  D-69118 Heidelberg,  Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/tools/gzip.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"

#include <cassert>
#include <fstream>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     GZIP/ZLIB Functions
// ================================================================================================

bool is_gzip_compressed_file( std::string const& file_name )
{
    // Check if the file extension hints at gzip.
    auto const ext = file_extension( file_name );
    bool const ext_gz = ( ext == "gz" || ext == "gzip" );

    // Open the file in binary mode.
    std::ifstream infile;
    infile.open( file_name, std::ifstream::in | std::ifstream::binary );
    if( !infile.good() ) {
        return false;
        // throw std::runtime_error( "Cannot read from file '" + file_name + "'." );
    }

    // Get the first two characters. If this fails, the file is too short, so it is not a gzip file.
    unsigned char buffer[2];
    infile.read( reinterpret_cast<char*>( &buffer ), 2 );
    if( !infile.good() ) {
        return false;
    }
    infile.close();

    // Check if the file starts with the magic number of gz files.
    bool const magic = ( buffer[0] == 0x1f ) && ( buffer[1] == 0x8b );

    // If extension and magic number agree, we have a clear result.
    // Otherwise, issue a warning, and return the magic bit, because this is what we trust more.
    if( ext_gz && magic ) {
        return true;
    } else if( ! ext_gz && ! magic ) {
        return false;
    } else if( ext_gz && ! magic ) {
        LOG_WARN << "File name '" << file_name << "' ends in '.gz', but the file does not seem "
                 << "to contain gzip content.";
    } else if( ! ext_gz && magic ) {
        LOG_WARN << "File name '" << file_name << "' does not end in '.gz', but the file seems "
                 << "to contain gzip content.";
    }
    return magic;
}

} // namespace utils
} // namespace genesis
