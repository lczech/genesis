/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Implementation of file system functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/core/fs.hpp"

#include <cctype>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <sys/stat.h>

#include "utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     File Access
// =================================================================================================

/**
 * @brief Return true iff the provided path is a file.
 *
 * Internally, this function simply return the value of file_exists(), as this already does the
 * needed check. Thus, it is an alias.
 */
bool is_file( std::string const& path )
{
    return file_exists( path );
}

/**
 * @brief Return true iff the file exists.
 */
bool file_exists( std::string const& filename )
{
    std::ifstream infile(filename);
    return infile.good();
}

/**
 * @brief Return the contents of a file as a string.
 *
 * If the file is not readable, the function throws `std::runtime_error`.
 */
std::string file_read( std::string const& filename )
{
    std::ifstream infile(filename);
    std::string   str;

    if (!infile.good()) {
        throw std::runtime_error( "Cannot read from file '" + filename + "'." );
    }

    infile.seekg(0, std::ios::end);
    str.reserve(infile.tellg());
    infile.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(infile)),
                std::istreambuf_iterator<char>());
    return str;
}

/**
 * @brief Write the content of a string to a file.
 *
 * If the file is not writable, the function throws `std::runtime_error`.
 */
void file_write( std::string const& content, std::string const& filename )
{
    // TODO check if path exists, create if not (make a function for that)
    // TODO check if file exists, trigger warning?, check if writable

    std::ofstream outfile(filename);
    if (!outfile.good()) {
        throw std::runtime_error( "Cannot write to file '" + filename + "'." );
    }
    outfile << content;
}

/**
 * @brief Append the content of a string to a file.
 *
 * If the file is not writable, the function throws `std::runtime_error`.
 */
void file_append( std::string const& content, std::string const& filename )
{
    // TODO check if path exists, create if not (make a function for that)
    // TODO check if file exists, trigger warning?, check if writable
    // TODO maybe merge with file_write and use mode as optional parameter.

    std::ofstream outfile(filename, std::ofstream::app);
    if (!outfile.good()) {
        throw std::runtime_error( "Cannot write to file '" + filename + "'." );
    }
    outfile << content;
}

// =================================================================================================
//     Directory Access
// =================================================================================================

/**
 * @brief Return true iff the provided path is a directory.
 *
 * Internally, this function simply return the value of dir_exists(), as this already does the
 * needed check. Thus, it is an alias.
 */
bool is_dir( std::string const& path )
{
    return dir_exists( path );
}

/**
 * @brief Return true iff the directory exists.
 */
bool dir_exists( std::string const& dir )
{
    struct stat info;
    if (stat (dir.c_str(), &info) != 0) {
        return false;
    }
    return info.st_mode & S_IFDIR;

    // alternative implementation:
    // DIR* dp = opendir(dir);
    // if (dp) {
    //     closedir(dir);
    //     return dp;
    // } else {
    //     return false;
    // }
}

/**
 * @brief Create a directory.
 *
 * If the directory already exists, nothing happens.
 * If the path exists, but is not a directory, a `std::runtime_error` is thrown.
 * If the creation fails for some other reason, also a `std::runtime_error` is thrown.
 */
void dir_create( std::string const& path )
{
    mode_t mode = 0775;
    struct stat info;

    if( stat (path.c_str(), &info) != 0 ) {
        if( mkdir( path.c_str(), mode ) != 0 && errno != EEXIST ) {
            throw std::runtime_error("Cannot create directory: " + path);
        }
    } else if( !S_ISDIR(info.st_mode) ) {
        throw std::runtime_error("Path exists, but is not a directory: " + path);
    }
}

/**
 * @brief Get a list of files in a directory.
 *
 * If the directory is not readable, the function throws `std::runtime_error`.
 */
std::vector<std::string> dir_list_files( std::string const& dir )
{
    std::vector<std::string> list;

    DIR*           dp;
    struct dirent* dirp;

    if( ( dp  = opendir( dir.c_str() )) == nullptr) {
        throw std::runtime_error( "Cannot open directory '" + dir + "'." );
    }
    while ((dirp = readdir(dp)) != nullptr) {
        std::string fn = std::string(dirp->d_name);
        if (fn == "." || fn == "..") {
            continue;
        }
        list.push_back(fn);
    }
    closedir(dp);
    //~ std::sort(list.begin(), list.end());
    return list;
}

// =================================================================================================
//     File Information
// =================================================================================================

/**
 * @brief Return information about a file.
 */
std::unordered_map<std::string, std::string> file_info( std::string const& filename )
{
    std::string basename = file_basename(filename);
    std::unordered_map<std::string, std::string> res;

    res["path"]      = file_path(filename);
    res["basename"]  = basename;
    res["filename"]  = file_filename(basename);
    res["extension"] = file_extension(basename);

    return res;
}

/**
 * @brief Return the size of a file.
 */
size_t file_size( std::string const& filename )
{
    auto result = filename;
    std::ifstream in(result, std::ifstream::ate | std::ifstream::binary);
    return static_cast<size_t>(in.tellg());
}

/**
 * @brief Return the path leading to a file.
 *
 * Does not resolve the path. Simply splits at the last directory separator.
 */
std::string file_path( std::string const& filename )
{
    auto result = filename;
    const size_t idx = result.find_last_of("\\/");
    if (idx != std::string::npos)
    {
        result.erase(idx);
    }
    return result;
}

/**
 * @brief Remove directory name from file name if present.
 */
std::string file_basename( std::string const& filename )
{
    auto result = filename;
    const size_t idx = result.find_last_of("\\/");
    if (idx != std::string::npos)
    {
        result.erase(0, idx + 1);
    }
    return result;
}

/**
 * @brief Remove extension if present.
 *
 * Caveat: Does not remove the path. So, if the filename itself does not contain an extension
 * separator ".", but the path does, this will yield an unwanted result. Call file_basename() first.
 */
std::string file_filename( std::string const& filename )
{
    auto result = filename;
    const size_t idx = result.rfind('.');
    if (idx != 0 && idx != std::string::npos)
    {
        result.erase(idx);
    }
    return result;
}

/**
 * @brief Return the extension name of a file.
 *
 * Also see file_filename().
 */
std::string file_extension( std::string const& filename )
{
    auto result = filename;
    const size_t idx = result.rfind('.');
    if (idx != 0 && idx != std::string::npos)
    {
        result.erase(0, idx + 1);
    }
    return result;
}

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
bool is_valid_filname( std::string const& filename )
{
    // No empty filenames.
    if( trim( filename ) == "" ) {
        return false;
    }

    // No space at beginning or end.
    if( starts_with( filename, " " ) || ends_with( filename, " " )) {
        return false;
    }

    // Check forbidden chars of Win and Unix systems.
    if( filename.find_first_of( "<>:\"\\/|?*" ) != std::string::npos ) {
        return false;
    }

    // Check for non-printable chars.
    // They might be allowed on most systems, but better be conservative here.
    for( auto c : filename ) {
        if( ! isprint( c ) ) {
            return false;
        }
    }

    return true;
}

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
std::string sanitize_filname( std::string const& filename )
{
    // Prepare result.
    std::string result = "";
    result.reserve( filename.size() );

    // Copy all printable chars, drop the others.
    for( auto c : filename ) {
        if( isprint( c ) ) {
            result += c;
        }
    }

    // No spaces around the name, and replace all forbidden chars by underscores.
    result = trim( result );
    result = replace_all_chars( result, "<>:\"\\/|?*", '_' );

    if( result == "" ) {
        throw std::runtime_error( "Invalid filename." );
    }

    return result;
}

} // namespace utils
} // namespace genesis
