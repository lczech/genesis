#ifndef GENESIS_UTIL_IO_OUTPUT_STREAM_H_
#define GENESIS_UTIL_IO_OUTPUT_STREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup util
 */

#include <genesis/util/core/exception.hpp>
#include <genesis/util/core/fs.hpp>
#include <genesis/util/core/options.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace util {
namespace io {

// =================================================================================================
//     Output Stream
// =================================================================================================

/**
 * @brief Helper function to obtain an output stream to a file.
 *
 * This function is used internally by all file writers. It makes sure that the file is writable,
 * and throws an exception otherwise.
 *
 * Furthermore, the function checks whether the path already exists. If it exists and
 * @link Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink is not
 * explicitly activated, the function throws, too. This is a safty measure to ensure that users
 * cannot accidentally overwrite their files.
 *
 * @param[in]   file_name   Path to the file to write to.
 * @param[out]  out_stream  Reference to the target output stream. Unfortunately, we cannot return
 *                          streams, as they do not offer copy or move, so we have to do it this way.
 * @param[in]   mode        Mode flag as used by std::ostream.
 * @param[in]   create_dirs Create parent directories if needed.
 *
 * @see Options::allow_file_overwriting( bool )
 */
inline void file_output_stream(
    std::string const&      file_name,
    std::ofstream&          out_stream,
    std::ios_base::openmode mode = std::ios_base::out,
    bool                    create_dirs = true
) {
    using namespace genesis::util::core;

    // Make sure that we are only overwriting if we are allowed to.
    if( ! Options::get().allow_file_overwriting() && path_exists( file_name ) ) {
        throw ExistingFileError(
            "Output path '" + file_name + "' already exists. If you want to allow overwriting of "
            "existing files, activate genesis::util::core::Options::get().allow_file_overwriting() first.",
            file_name
        );
    }

    // Create all parent dirs, if needed.
    if( create_dirs ) {
        auto const path = file_path( file_name );
        dir_create( path );
    }

    // Now prepare the stream and check that this worked.
    errno = 0;
    out_stream.open( file_name, mode );
    if( !out_stream.is_open() || out_stream.fail() ) {
        throw std::runtime_error(
            "Cannot open output file '" + file_name + "': " + std::string( strerror( errno ))
        );
    }
}

/**
 * @brief Helper function to obtain a binary output FILE handle with RAII lifetime management.
 *
 * Opens the file in binary write mode ("wb"). Performs the same safety checks as
 * file_output_stream(): refuses to overwrite existing files unless explicitly allowed,
 * and optionally creates parent directories.
 *
 * The returned unique_ptr calls fclose() automatically when it goes out of scope.
 * The underlying FILE* is accessible via .get() for use with C I/O functions.
 *
 * @param[in] file_name   Path to the file to write to.
 * @param[in] create_dirs Create parent directories if needed.
 * @return A unique_ptr<FILE> that closes the file on destruction.
 *
 * @see file_output_stream(), Options::allow_file_overwriting()
 */
inline std::unique_ptr<FILE, decltype(&std::fclose)> file_output_file(
    std::string const& file_name,
    bool               create_dirs = true
) {
    using namespace genesis::util::core;

    // Make sure that we are only overwriting if we are allowed to.
    if( !Options::get().allow_file_overwriting() && path_exists( file_name ) ) {
        throw ExistingFileError(
            "Output path '" + file_name + "' already exists. If you want to allow overwriting of "
            "existing files, activate genesis::util::core::Options::get().allow_file_overwriting() first.",
            file_name
        );
    }

    // Create all parent dirs, if needed.
    if( create_dirs ) {
        auto const path = file_path( file_name );
        dir_create( path );
    }

    // Now prepare the file and check that this worked.
    errno = 0;
    FILE* fp = std::fopen( file_name.c_str(), "wb" );
    if( !fp ) {
        throw std::runtime_error(
            "Cannot open output file '" + file_name + "': " + std::string( std::strerror( errno ))
        );
    }
    return { fp, std::fclose };
}

} // namespace io
} // namespace util
} // namespace genesis

#endif // include guard
