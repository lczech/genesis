#ifndef GENESIS_UTIL_IO_FILE_HANDLE_H_
#define GENESIS_UTIL_IO_FILE_HANDLE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2026 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief RAII wrappers for C FILE* handles (binary input and output).
 *
 * Provides file_input_file() and file_output_file(), which open a file and return a
 * `unique_ptr<FILE, int(*)(FILE*)>` that automatically calls fclose() on destruction.
 * Both functions throw `std::runtime_error` on failure.
 *
 * These helpers are intentionally low-level (C FILE*) rather than using C++ streams,
 * which makes them suitable for code that needs fseek/pread/fread on binary blobs.
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
#include <memory>
#include <stdexcept>
#include <string>

namespace genesis {
namespace util {
namespace io {

// =================================================================================================
//     C FILE* RAII Handles
// =================================================================================================

/**
 * @brief Helper function to obtain a binary output FILE handle with RAII lifetime management.
 *
 * Opens the file in binary write mode ("wb"). Refuses to overwrite existing files unless
 * genesis::util::core::Options::get().allow_file_overwriting() is set, and optionally creates
 * parent directories. Throws std::runtime_error if the file cannot be opened.
 *
 * The underlying FILE* is accessible via .get() for use with C I/O functions.
 *
 * @param[in] file_name   Path to the file to write to.
 * @param[in] create_dirs Create parent directories if needed.
 * @return A unique_ptr<FILE> that closes the file on destruction.
 *
 * @see file_input_file(), file_output_stream()
 */
inline std::unique_ptr<std::FILE, int(*)(std::FILE*)> file_output_file(
    std::string const& file_name,
    bool               create_dirs = true
) {
    using namespace genesis::util::core;

    // Refuse to overwrite unless explicitly allowed.
    if( !Options::get().allow_file_overwriting() && path_exists( file_name ) ) {
        throw ExistingFileError(
            "Output path '" + file_name + "' already exists. If you want to allow overwriting of "
            "existing files, activate genesis::util::core::Options::get().allow_file_overwriting() first.",
            file_name
        );
    }

    // Create parent directories if requested.
    if( create_dirs ) {
        auto const path = file_path( file_name );
        dir_create( path );
    }

    // Open and return the file handle.
    errno = 0;
    std::FILE* fp = std::fopen( file_name.c_str(), "wb" );
    if( !fp ) {
        throw std::runtime_error(
            "Cannot open output file '" + file_name + "': " + std::string( std::strerror( errno ))
        );
    }
    return { fp, std::fclose };
}

/**
 * @brief Helper function to obtain a binary input FILE handle with RAII lifetime management.
 *
 * Opens the file in binary read mode ("rb") and returns a unique_ptr that calls fclose()
 * automatically when it goes out of scope. Throws std::runtime_error if the file cannot be opened.
 *
 * The underlying FILE* is accessible via .get() for use with C I/O functions such as
 * fread(), fseeko(), and ftello().
 *
 * @param[in] file_name Path to the file to read from.
 * @return A unique_ptr<FILE> that closes the file on destruction.
 *
 * @see file_output_file()
 */
inline std::unique_ptr<std::FILE, int(*)(std::FILE*)> file_input_file(
    std::string const& file_name
) {
    errno = 0;
    std::FILE* fp = std::fopen( file_name.c_str(), "rb" );
    if( !fp ) {
        throw std::runtime_error(
            "Cannot open input file '" + file_name + "': " + std::string( std::strerror( errno ))
        );
    }
    return { fp, std::fclose };
}

} // namespace io
} // namespace util
} // namespace genesis

#endif // GENESIS_UTIL_IO_FILE_HANDLE_H_
