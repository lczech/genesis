/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include <cassert>
#include <cctype>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <limits.h>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <streambuf>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/io/gzip.hpp"
#include "genesis/utils/io/gzip_stream.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/io/scanner.hpp"
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

    std::ifstream instream(filename);
    instream.seekg( 0, std::ios::end);
    return instream.good() && ! is_dir( filename );
}

std::string file_read( std::string const& filename, bool detect_compression )
{
    // Create string beforehand to enable copy elision.
    std::string str;

    if( detect_compression && is_gzip_compressed_file( filename )) {

        // Open decompressing stream
        GzipIFStream instream( filename );
        if( !instream.good() ) {
            throw std::runtime_error( "Cannot read from file '" + filename + "'." );
        }

        // Can't assess file size for compressed files, so just read.
        str.assign(
            std::istreambuf_iterator<char>(instream),
            std::istreambuf_iterator<char>()
        );
        return str;
    }

    // Open normal stream
    std::ifstream instream(filename);
    if( !instream.good() ) {
        throw std::runtime_error( "Cannot read from file '" + filename + "'." );
    }

    // Get file size, so that we do not waste time and space for string concats.
    instream.seekg(0, std::ios::end);
    str.reserve(instream.tellg());
    instream.seekg(0, std::ios::beg);

    // Read content
    str.assign(
        std::istreambuf_iterator<char>(instream),
        std::istreambuf_iterator<char>()
    );
    return str;
}

std::vector<std::string> file_read_lines( std::string const& filename, bool detect_compression )
{
    std::vector<std::string> result;
    utils::InputStream it( from_file( filename, detect_compression ));
    while( it ) {
        result.push_back( read_to_end_of_line( it ));
        assert( !it || *it == '\n'  );
        ++it;
    }
    return result;
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

static std::vector<std::string> dir_list_contents_(
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
//     Path Information
// =================================================================================================

static std::string current_path_getcwd_()
{
    // Wrapper for the standard getcwd() function to bring it into C++
    // Adapted from https://stackoverflow.com/a/2203853/4184258

    // The local buffer of size PATH_MAX is fine for Windows, but pretty much no other OS.
    char temp[ PATH_MAX ];
    errno = 0;
    if( getcwd( temp, PATH_MAX ) != nullptr ) {
        return dir_normalize_path( std::string( temp ));
    }

    int error = errno;
    switch( error ) {
        case EACCES:
            throw std::runtime_error( "Cannot read current directory. Access denied." );

        case ENOMEM:
            throw std::runtime_error( "Cannot read current directory. Insufficient storage." );

        default: {
            std::ostringstream str;
            str << "Cannot read current directory. Unrecognised error: " << error;
            throw std::runtime_error( str.str() );
        }
    }

    // Make compilers happy.
    return "";
}

static std::string current_path_unix_()
{
    // Code adapted from http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
    // Does not work on Windows systems.

    std::string path;
    typedef std::pair<dev_t, ino_t> file_id;

    // Keep track of start directory, so can jump back to it later
    int start_fd = open(".", O_RDONLY);
    if( start_fd == -1 ) {
        throw std::runtime_error( "Cannot read current directory." );
    }

    struct stat sb;
    if( fstat(start_fd, &sb) ) {
        throw std::runtime_error( "Cannot read current directory." );
    }

    // Get info for root directory, so we can determine when we hit it
    file_id current_id(sb.st_dev, sb.st_ino);
    if( stat( "/", &sb )) {
        throw std::runtime_error( "Cannot read current directory." );
    }

    std::vector<std::string> path_components;
    file_id root_id(sb.st_dev, sb.st_ino);

    // If they're equal, we've obtained enough info to build the path
    while( current_id != root_id ) {
        bool pushed = false;

        // Keep recursing towards root each iteration
        if( ! chdir("..") ) {
            DIR *dir = opendir(".");
            if( dir ) {
                dirent *entry;

                // We loop through each entry trying to find where we came from
                while(( entry = readdir(dir) )) {
                    if(
                        strcmp(entry->d_name, ".")
                        && strcmp(entry->d_name, "..")
                        && !lstat(entry->d_name, &sb)
                    ) {
                        file_id child_id(sb.st_dev, sb.st_ino);

                        // We found where we came from, add its name to the list
                        if( child_id == current_id ) {
                            path_components.push_back(entry->d_name);
                            pushed = true;
                            break;
                        }
                    }
                }
                closedir(dir);

                // If we have a reason to continue, we update the current dir id
                if( pushed && !stat(".", &sb) ) {
                    current_id = file_id(sb.st_dev, sb.st_ino);
                }
                // Else, Uh oh, can't read information at this level
            }
        }

        // If we didn't obtain any info this pass, no reason to continue
        if( ! pushed ) {
            break;
        }
    }

    // Unless they're equal, we failed above
    if( current_id == root_id )  {

        // Built the path, will always end with a slash
        path = "/";
        for( auto i = path_components.rbegin(); i != path_components.rend(); ++i ) {
            path += *i + "/";
        }
    } else {
        throw std::runtime_error( "Cannot read current directory." );
    }

    if( fchdir(start_fd) ) {
        throw std::runtime_error( "Cannot change directory." );
    }
    close(start_fd);

    return path;
}

std::string current_path()
{
    // We use our own implementations here, which depend on the OS.
    #if defined( _WIN32 ) || defined(  _WIN64  )
        return current_path_getcwd_();
    #else
        return current_path_unix_();
    #endif

    // We use conditional compilation. Silence compiler warnings about unused functions.
    (void) current_path_getcwd_;
    (void) current_path_unix_;
}

// The following set of helper functions is adapted from
// http://insanecoding.blogspot.com/2007/11/implementing-realpath-in-c.html
// See there for details.

static void relative_dir_base_split_( std::string const& path, std::string& dir, std::string& base )
{
    // Find the last slash
    auto slash_pos = path.rfind("/");

    // If there is a slash
    if (slash_pos != std::string::npos) {
        slash_pos++;

        // Directory is before slash
        dir = path.substr(0, slash_pos);

        // And obviously, the file is after
        base = path.substr(slash_pos);
    } else {
        // Otherwise, there is no directory present
        dir.clear();
        base = path;
    }
}

static std::string chdir_getcwd_( std::string const& dir )
{
    // Open current directory so we can save a handle to it
    int start_fd = open(".", O_RDONLY);
    if (start_fd == -1) {
        throw std::runtime_error( "Cannot open current directory." );
    }

    // Change to directory
    if( chdir(dir.c_str()) ) {
        throw std::runtime_error( "Cannot change directory." );
    }

    // And get its path
    auto const path = current_path();

    // And change back of course
    if( fchdir(start_fd) ) {
        throw std::runtime_error( "Cannot change directory." );
    }
    close(start_fd);

    return path;
}

static std::string realpath_file_(const std::string& path )
{
    std::string dir;
    std::string base;
    relative_dir_base_split_(path, dir, base);

    // If there is a directory, get the realpath() for it, otherwise the current directory
    auto resolved_path = dir.size() ? chdir_getcwd_(dir) : current_path();
    return resolved_path + base;
}

static bool readlink_internal_(const std::string& path, std::string& buffer, ssize_t length)
{
    bool success = false;
    if (length > 0) {
        // Room for Null
        char *buf = new(std::nothrow) char[length+1];

        if (buf) {
            // Give room for failure
            ssize_t amount = ::readlink(path.c_str(), buf, length+1);

            // If > length, it was modified mid check
            if ((amount > 0) && (amount <= length)) {
                buf[amount] = 0;
                buffer = buf;
                success = true;
            }
            delete[] buf;
        }
    }

    return success;
}

static void build_path_base_swap_(std::string &path, const std::string& newbase)
{
    std::string dir;
    std::string base;
    relative_dir_base_split_(path, dir, base);

    if (dir.size()) {
        path = dir + newbase;
    } else {
        path = newbase;
    }
}

static bool symlink_resolve_( const std::string& start, std::string& end )
{
    typedef std::pair<dev_t, ino_t> file_id;

    bool success = false;
    if (start.size()) {
        // Need a modifyable copy
        std::string path = start;
        struct stat sb;
        std::set<file_id> seen_links;

        bool resolved_link;
        // The symlink resolve loop
        do {
            resolved_link = false;
            if (!lstat(path.c_str(), &sb)) {
                file_id current_id(sb.st_dev, sb.st_ino);
                // Not a link we've seen
                if (seen_links.find(current_id) == seen_links.end()) {
                    // Add to our set
                    seen_links.insert(current_id);

                    // Another link
                    if (S_ISLNK(sb.st_mode)) {
                        std::string newpath;
                        if (readlink_internal_(path, newpath, sb.st_size)) {
                            // Absolute
                            if (newpath[0] == '/') {
                                path = newpath;
                            } else {
                                // We need to calculate the relative path in relation to the current
                                build_path_base_swap_(path, newpath);
                            }
                            resolved_link = true;
                            // Else, Link can't be read, time to quit
                        }
                    } else {
                        // Yay, it's not a link! got to the last part finally!
                        end = realpath_file_( path );
                        success = true;
                    }
                    // Else, Nice try, someone linked a link back into a previous link
                    // during the scan to try to trick us into an infinite loop.
                }
                // Else, Dangling link, can't resolve
            }
        } while (resolved_link);
    }
    return(success);
}

static std::string real_path_unix_( std::string const& path, bool resolve_link )
{
    // Custom implementation, using the above helper functions. Based on
    // http://insanecoding.blogspot.com/2007/11/implementing-realpath-in-c.html
    // See there for details.

    // Edge case.
    if( ! path.size()) {
        return path;
    }

    struct stat sb;
    if( stat(path.c_str(), &sb) ) {
        throw std::runtime_error( "Cannot read path." );
    }

    if( S_ISDIR( sb.st_mode )) {
        return chdir_getcwd_( path );
    } else {
        if( resolve_link ) {
            std::string result;
            if( ! symlink_resolve_( path, result ) ) {
                throw std::runtime_error( "Cannot determine real path." );
            }
            return result;
        } else {
            return realpath_file_( path );
        }
    }

    // Make compilers happy.
    return path;
}

static std::string real_path_realpath_( std::string const& path, bool resolve_link )
{
    // Wrapper for the standard realpath() function to bring it into C++
    // Works on Windows.

    // Not supported in this function...
    (void) resolve_link;

    errno = 0;
    char resolved_path[PATH_MAX];
    auto ptr = realpath( path.c_str(), resolved_path );
    if( errno != 0 ) {
        throw std::runtime_error( "Cannot determine real path." );
    }
    return std::string( ptr );
}

std::string real_path( std::string const& path, bool resolve_link )
{
    // We use our own implementations here, which depend on the OS.
    #if defined( _WIN32 ) || defined(  _WIN64  )
        return real_path_realpath_( path, resolve_link );
    #else
        return real_path_unix_( path, resolve_link );
    #endif

    // We use conditional compilation. Silence compiler warnings about unused functions.
    (void) real_path_realpath_;
    (void) real_path_unix_;
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

bool is_valid_filename( std::string const& filename )
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

std::string sanitize_filename( std::string const& filename )
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
