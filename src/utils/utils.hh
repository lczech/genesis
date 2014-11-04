#ifndef GNS_UTILS_UTILS_H_
#define GNS_UTILS_UTILS_H_

/**
 * @brief Provides some commonly used functions.
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <ctime>

namespace genesis {
namespace utils {

/**
 * Returns the current date as a string in the format "2014-12-31".
 */
inline std::string CurrentDate()
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
 * Returns the current time as a string in the format "13:37:42".
 */
inline std::string CurrentTime()
{
    time_t now = time(0);
    tm*    ltm = localtime(&now);

    char out[10];
    sprintf (out, "%02u:%02u:%02u", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return out;
}

std::string ReadFile (const std::string fn);

} // namespace utils
} // namespace genesis

#endif // include guard
