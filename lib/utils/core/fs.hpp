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
bool        file_write  ( std::string const& filename, std::string const& content );
bool        file_append ( std::string const& filename, std::string const& content );

bool        dir_exists     ( std::string const& dir );
void        dir_create     ( std::string const& path );
bool        dir_list_files ( std::string const& dir, std::vector<std::string>& list );

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
