#ifndef GENESIS_UTILS_IO_OUTPUT_STREAM_H_
#define GENESIS_UTILS_IO_OUTPUT_STREAM_H_

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

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/options.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Output Stream
// =================================================================================================

/**
 * @brief Helper function to obtain an output stream to a file.
 *
 * This function is used internally by all file writers. It makes sure that the file is writable,
 * and throws an exception otherwise.
 *
 * Furthermore, the function checks whether the file already exists. If it exists and
 * @link Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink is not
 * explicitly activated, the function throws, too. This is a safty measure to ensure that users
 * cannot accidentally overwrite their files.
 *
 * @param[in]   filename    Path to the file to write to.
 * @param[out]  out_stream  Reference to the target output stream. Unfortunately, we cannot return
 *                          streams, as they do not offer copy or move, so we have to do it this way.
 * @param[in]   mode        Mode flag as used by std::ostream.
 *
 * @see Options::allow_file_overwriting( bool )
 */
inline void file_output_stream(
    std::string const&      filename,
    std::ofstream&          out_stream,
    std::ios_base::openmode mode = std::ios_base::out
) {
    if( ! Options::get().allow_file_overwriting() && utils::file_exists( filename ) ) {
        throw std::runtime_error(
            "File '" + filename + "' already exists. If you want to allow overwriting of existing "
            "files, activate Options::allow_file_overwriting() first."
        );
    }

    out_stream.open( filename, mode );
    if( out_stream.fail() ) {
        throw std::runtime_error( "Cannot write to file '" + filename + "'." );
    }
}

} // namespace utils
} // namespace genesis

#endif // include guard
