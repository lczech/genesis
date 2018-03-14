#ifndef GENESIS_UTILS_CORE_FS_H_
#define GENESIS_UTILS_CORE_FS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

/**
 * @brief Return whether a path exists, i.e., is a file or directory.
 */
bool path_exists( std::string const& path );

/**
 * @brief Return true iff the provided path is a file.
 *
 * Internally, this function simply return the value of file_exists(), as this already does the
 * needed check. Thus, it is an alias.
 */
bool is_file( std::string const& path );

/**
 * @brief Return true iff the file exists.
 */
bool file_exists( std::string const& filename );

/**
 * @brief Return the contents of a file as a string.
 *
 * If the file is not readable, the function throws `std::runtime_error`.
 */
std::string file_read( std::string const& filename );

/**
 * @brief Write the content of a string to a file.
 *
 * If the file cannot be written to, the function throws an exception. Also, by default, if the file
 * already exists, an exception is thrown.
 * See @link Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink to
 * change this behaviour.
 */
void file_write( std::string const& content, std::string const& filename );

/**
 * @brief Append the content of a string to a file.
 *
 * If the file is not writable, the function throws `std::runtime_error`.
 */
void file_append( std::string const& content, std::string const& filename );

/**
 * @brief Return true iff the provided path is a directory.
 *
 * Internally, this function simply return the value of dir_exists(), as this already does the
 * needed check. Thus, it is an alias.
 */
bool is_dir( std::string const& path );

/**
 * @brief Return true iff the directory exists.
 */
bool dir_exists( std::string const& dir );

/**
 * @brief Create a directory.
 *
 * If the directory already exists, nothing happens.
 * If the path exists, but is not a directory, a `std::runtime_error` is thrown.
 * If the creation fails for some other reason, also a `std::runtime_error` is thrown.
 */
void dir_create( std::string const& path, bool with_parents = true );

/**
 * @brief Normalize a dir name, i.e., make sure that the given path ends with exaclty one slash.
 */
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

/**
 * @brief Return information about a file.
 */
std::unordered_map<std::string, std::string> file_info ( std::string const& filename );

/**
 * @brief Return the size of a file.
 */
size_t      file_size      ( std::string const& filename );

/**
 * @brief Return the path leading to a file.
 *
 * Does not resolve the path. Simply splits at the last directory separator.
 * If there is no directory separator, it returns an empty string.
 */
std::string file_path      ( std::string const& filename );

/**
 * @brief Remove directory name from file name if present.
 */
std::string file_basename  ( std::string const& filename );

/**
 * @brief Remove extension if present.
 *
 * Caveat: Does not remove the path. So, if the filename itself does not contain an extension
 * separator ".", but the path does, this will yield an unwanted result. Call file_basename() first.
 */
std::string file_filename  ( std::string const& filename );

/**
 * @brief Return the extension name of a file.
 *
 * Also see file_filename().
 */
std::string file_extension ( std::string const& filename );

// =================================================================================================
//     File Names
// =================================================================================================

/**
 * @brief Check whether a file name is valid.
 *
 * Validating filenames depends on the operating system and file system of the disk. Thus, this is
 * usually not an easy task. This function only checks some basics and is meant to catch the most
 * common problems.
 *
 * The function is meant to be called on the file name itself, without the directory path leading
 * to it. File extensions are allowed. Thus, you might need to call file_basename() before in order
 * to get the file name without the path.
 *
 * Invalid filenames are:
 *
 *   * Those with spaces at the beginning or end, or only consisting of spaces (or empty).
 *   * Those which contain any of the chars `< > : " \ / | ? *`.
 *   * Those which contain any non-printable character, as determined via isprint().
 *
 * This might be too converative for some system, or allow too much for others. It however should
 * return true for filenames that work on most systems.
 */
bool        is_valid_filname( std::string const& filename );

/**
 * @brief Remove or replace all invalid parts of a filename.
 *
 * Similar to is_valid_filname(), this function is not meant to be an ultimate solution to valid
 * filenames. See there for details.
 *
 * The function is meant to be called on the file name itself, without the directory path leading
 * to it. File extensions are allowed. Thus, you might need to call file_basename() before in order
 * to get the file name without the path.
 *
 * This function does the following:
 *
 *   * All non-printable characters are removed.
 *   * Spaces at the beginning and end are removed.
 *   * All invalid chars are replaced by an underscore. See is_valid_filname() for a list of those
 *     chars.
 *
 * If after this procedure the filename is empty, an exception is thrown. This is meant to save the
 * user from checking this, or from running into trouble when trying to write to this "file" -
 * because an empty filename will point to a directory name.
 */
std::string sanitize_filname( std::string const& filename );

} // namespace utils
} // namespace genesis

#endif // include guard
