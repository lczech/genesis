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

// =================================================================================================
//     File Access
// =================================================================================================

/**
 * @brief Returns true iff the file exists.
 */
bool        file_exists (const std::string& filename);

/**
 * @brief Returns the contents of a file as a string.
 *
 * If the file does not exist, a warning is triggered and an emtpty string returned.
 */
std::string file_read   (const std::string& filename);

/**
 * @brief Writes the content of a string to a file.
 */
bool        file_write  (const std::string& filename, const std::string& content);

/**
 * @brief Appends the content of a string to a file.
 */
bool        file_append (const std::string& filename, const std::string& content);

/**
 * @brief Return true iff the directory exists.
 */
bool        dir_exists     (const std::string& dir);

/**
 * @brief Get a list of files in a directory.
 */
bool        dir_list_files (const std::string& dir, std::vector<std::string>& list);

// =================================================================================================
//     File Information
// =================================================================================================

/**
 * @brief Returns information about a file.
 */
std::unordered_map<std::string, std::string> file_info (std::string filename);

/**
 * @brief Return the size of a file.
 */
size_t      file_size      (std::string filename);

/**
 * @brief Returns the path leading to a file.
 *
 * Does not resolve the path. Simply splits at the last directory separator.
 */
std::string file_path      (std::string filename);

/**
 * @brief Remove directory name from file name if present.
 */
std::string file_basename  (std::string filename);

/**
 * @brief Remove extension if present.
 *
 * Caveat: Does not remove the path. So, if the filename itself does not contain an extension
 * separator ".", but the path does, this will yield an unwanted result. Call file_basename() first.
 */
std::string file_filename  (std::string filename);

/**
 * @brief Returns the extension name of a file.
 *
 * Also see file_filename().
 */
std::string file_extension (std::string filename);

} // namespace genesis

#endif // include guard
