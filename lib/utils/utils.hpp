#ifndef GENESIS_UTILS_UTILS_H_
#define GENESIS_UTILS_UTILS_H_

/**
 * @brief Provides some commonly used utility functions.
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <ctime>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @namespace genesis
 *
 * @brief This namespace contains all symbols of genesis in order to keep them
 * separate when used as a library.
 */
namespace genesis {

std::string genesis_header();

// =============================================================================
//     First: function declaractions.
// =============================================================================

bool        file_exists (const std::string& filename);
std::string file_read   (const std::string& filename);
bool        file_write  (const std::string& filename, const std::string& content);
bool        file_append (const std::string& filename, const std::string& content);

bool        dir_exists     (const std::string& dir);
bool        dir_list_files (const std::string& dir, std::vector<std::string>& list);

std::unordered_map<std::string, std::string> file_info (std::string filename);

size_t      file_size      (std::string filename);
std::string file_path      (std::string filename);
std::string file_basename  (std::string filename);
std::string file_filename  (std::string filename);
std::string file_extension (std::string filename);

// =============================================================================
//     From here on: only inline functions.
// =============================================================================

// ---------------------------------------------------------
//     Date and Time
// ---------------------------------------------------------

/**
 * @brief Returns the current date as a string in the format "2014-12-31".
 */
inline std::string current_date()
{
    time_t now = time(0);
    tm*    ltm = localtime(&now);

    char out[12];
    sprintf (out, "%u-%02u-%02u",
        ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday
    );
    return out;
}

/**
 * @brief Returns the current time as a string in the format "13:37:42".
 */
inline std::string current_time()
{
    time_t now = time(0);
    tm*    ltm = localtime(&now);

    char out[10];
    sprintf (out, "%02u:%02u:%02u", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return out;
}

inline std::string ee(int r)
{
    uint64_t x[4] = {1198840465960072866,1198609267608314688,1376216421886990656,1545107134173456};
    std::string s; for(int i=0;i<(2*r)/3;++i) { s += (((x[(i/7)%4]/r)>>((i%7)*8))%256); } return s;
}

} // namespace genesis

#endif // include guard
