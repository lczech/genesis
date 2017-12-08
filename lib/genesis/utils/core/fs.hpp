#ifndef GENESIS_UTILS_CORE_FS_H_
#define GENESIS_UTILS_CORE_FS_H_

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
 * @brief Provides functions for accessing the file system.
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     File Access
// =================================================================================================

bool        is_file     ( std::string const& path );
bool        file_exists ( std::string const& filename );
std::string file_read   ( std::string const& filename );
void        file_write  ( std::string const& content, std::string const& filename );
void        file_append ( std::string const& content, std::string const& filename );

bool        is_dir         ( std::string const& path );
bool        dir_exists     ( std::string const& dir );
void        dir_create     ( std::string const& path );

std::string dir_normalize_path( std::string const& path );


/**
 * @brief Get a list of files and directories in a directory.
 *
 * If @p full_path is set to `true`, the path of the provided @p dir is prepended to the resulting
 * list. If a non-empy @p regex is provided, file names are filtered by this regular expression.
 *
 * If the directory is not readable, the function throws `std::runtime_error`.
 */
std::vector<std::string> dir_list_contents(
    std::string const& dir,
    bool full_path = false,
    std::string const& regex = ""
);

/**
 * @brief Get a list of files in a directory.
 *
 * Behaves the same as dir_list_contents(), except tat only files are listed (i.e., no directories).
 * See there for an explanation of the arguments.
 */
std::vector<std::string> dir_list_files(
    std::string const& dir,
    bool full_path = false,
    std::string const& regex = ""
);

/**
 * @brief Get a list of directories in a directory.
 *
 * Behaves the same as dir_list_contents(), except tat only directories are listed (i.e., no files).
 * See there for an explanation of the arguments.
 */
std::vector<std::string> dir_list_directories(
    std::string const& dir,
    bool full_path = false,
    std::string const& regex = ""
);

// =================================================================================================
//     File Information
// =================================================================================================

std::unordered_map<std::string, std::string> file_info ( std::string const& filename );

size_t      file_size      ( std::string const& filename );
std::string file_path      ( std::string const& filename );
std::string file_basename  ( std::string const& filename );
std::string file_filename  ( std::string const& filename );
std::string file_extension ( std::string const& filename );

// =================================================================================================
//     File Names
// =================================================================================================

bool        is_valid_filname( std::string const& filename );
std::string sanitize_filname( std::string const& filename );

} // namespace utils
} // namespace genesis

#endif // include guard
