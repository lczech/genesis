#ifndef GENESIS_UTILS_CORE_FS_H_
#define GENESIS_UTILS_CORE_FS_H_

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

bool        file_exists ( std::string const& filename );
std::string file_read   ( std::string const& filename );
void        file_write  ( std::string const& content, std::string const& filename );
void        file_append ( std::string const& content, std::string const& filename );

bool        dir_exists     ( std::string const& dir );
void        dir_create     ( std::string const& path );

std::vector<std::string> dir_list_files ( std::string const& dir );

// =================================================================================================
//     File Information
// =================================================================================================

std::unordered_map<std::string, std::string> file_info ( std::string const& filename );

size_t      file_size      ( std::string filename );
std::string file_path      ( std::string filename );
std::string file_basename  ( std::string filename );
std::string file_filename  ( std::string filename );
std::string file_extension ( std::string filename );

} // namespace utils
} // namespace genesis

#endif // include guard
