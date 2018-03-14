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
 * @brief Implementation of file system functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/fs.hpp"

#include <cctype>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <functional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <sys/stat.h>

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     File Access
// =================================================================================================

bool path_exists( std::string const& path )
{
    struct stat info;
    return ( stat( path.c_str(), &info ) == 0 );
}

bool is_file( std::string const& path )
{
    return file_exists( path );
}

bool file_exists( std::string const& filename )
{
    // There are plenty of discussions on stackoverflow on how to do this correctly,
    // e.g., https://stackoverflow.com/a/12774387
    // None of them worked for me, meaning that they also returned true for directories.
    // Thus, we use a simple approach that does a basic check, and then also tests for dir...

    std::ifstream infile(filename);
    infile.seekg( 0, std::ios::end) ;
    return infile.good() && ! is_dir( filename );
}

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

void file_write( std::string const& content, std::string const& filename )
{
    // TODO check if path exists, create if not (make a function for that)

    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    ofs << content;
}

void file_append( std::string const& content, std::string const& filename )
{
    // TODO check if path exists, create if not (make a function for that)
    // TODO maybe merge with file_write and use mode as optional parameter.

    std::ofstream out_stream( filename, std::ofstream::app );
    if( out_stream.fail() ) {
        throw std::runtime_error( "Cannot append to file '" + filename + "'." );
    }
    out_stream << content;
}

// =================================================================================================
//     Directory Access
// =================================================================================================

bool is_dir( std::string const& path )
{
    return dir_exists( path );
}

bool dir_exists( std::string const& dir )
{
    struct stat info;
    if (stat (dir.c_str(), &info) != 0) {
        return false;
    }
    return static_cast<bool>( info.st_mode & S_IFDIR );

    // alternative implementation:
    // DIR* dp = opendir(dir);
    // if (dp) {
    //     closedir(dir);
    //     return dp;
    // } else {
    //     return false;
    // }
}

void dir_create( std::string const& path, bool with_parents )
{
    mode_t mode = 0775;
    struct stat info;

    // Checks. If it is the current dir, do nothing.
    auto const path_no_bs = utils::trim_right( path, "/\\");
    if( path_no_bs.empty() ) {
        return;
    }

    // Run recursively.
    if( with_parents ) {
        if( ! dir_exists( path_no_bs ) && path_no_bs.size() > 0 ) {
            dir_create( file_path( path_no_bs ), true );
        }
    }

    // Try to make dir.
    if( stat (path.c_str(), &info) != 0 ) {
        if( mkdir( path.c_str(), mode ) != 0 && errno != EEXIST ) {
            throw std::runtime_error( "Cannot create directory: " + path );
        }
    } else if( !S_ISDIR(info.st_mode) ) {
        throw std::runtime_error( "Path exists, but is not a directory: " + path );
    }
}

std::string dir_normalize_path( std::string const& path )
{
    return utils::trim_right( path, "/") + "/";
}

std::vector<std::string> dir_list_contents_(
    std::string const& dir,
    bool full_path,
    std::string const& regex,
    std::function<bool( std::string const& )> condition
) {
    std::vector<std::string> list;
    auto const dir_path = dir_normalize_path( dir );
    std::regex pattern( regex );

    DIR*           dp;
    struct dirent* dirp;

    if( ( dp  = opendir( dir.c_str() )) == nullptr) {
        throw std::runtime_error( "Cannot open directory '" + dir + "'." );
    }
    while ((dirp = readdir(dp)) != nullptr) {
        auto const fn = std::string( dirp->d_name );

        if (fn == "." || fn == "..") {
            continue;
        }
        if( ! regex.empty() && ! regex_match( fn, pattern ) ) {
            continue;
        }
        if( ! condition( dir_path + fn ) ) {
            continue;
        }

        if( full_path ) {
            list.push_back( dir_path + fn );
        } else {
            list.push_back( fn );
        }
    }
    closedir(dp);

    //~ std::sort(list.begin(), list.end());
    return list;
}

std::vector<std::string> dir_list_contents(
    std::string const& dir,
    bool full_path,
    std::string const& regex
) {
    return dir_list_contents_(
        dir, full_path, regex,
        []( std::string const& ){ return true; }
    );
}

std::vector<std::string> dir_list_files(
    std::string const& dir,
    bool full_path,
    std::string const& regex
) {
    return dir_list_contents_(
        dir, full_path, regex, is_file
    );
}

std::vector<std::string> dir_list_directories(
    std::string const& dir,
    bool full_path,
    std::string const& regex
) {
    return dir_list_contents_(
        dir, full_path, regex, is_dir
    );
}

// =================================================================================================
//     File Information
// =================================================================================================

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

size_t file_size( std::string const& filename )
{
    auto result = filename;
    std::ifstream in(result, std::ifstream::ate | std::ifstream::binary);
    return static_cast<size_t>(in.tellg());
}

std::string file_path( std::string const& filename )
{
    auto result = filename;
    const size_t idx = result.find_last_of("\\/");
    if( idx == std::string::npos ) {
        return "";
    }

    result.erase(idx);
    return result;
}

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
