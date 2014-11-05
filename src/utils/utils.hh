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

// first: inline functions.

/**
 * @brief Returns the current date as a string in the format "2014-12-31".
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
 * @brief Returns the current time as a string in the format "13:37:42".
 */
inline std::string CurrentTime()
{
    time_t now = time(0);
    tm*    ltm = localtime(&now);

    char out[10];
    sprintf (out, "%02u:%02u:%02u", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return out;
}

/**
 * @brief Returns a copy of the input string, with left trimmed white spaces.
 */
inline std::string StringTrimRight (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return s.substr(0, s.find_last_not_of(delimiters) + 1);
}

/**
 * @brief Returns a copy of the input string, with right trimmed white spaces.
 */
inline std::string StringTrimLeft (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return s.substr(s.find_first_not_of(delimiters));
}

/**
 * @brief Returns a copy of the input string, with trimmed white spaces.
 */
inline std::string StringTrim (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return StringTrimLeft(StringTrimRight(s, delimiters), delimiters);
}

// from here on: only declaractions.

std::string ReadFile (const std::string fn);

std::string StringDeescape (const std::string text);
std::string StringReplaceAll (
    const std::string &s,
    const std::string &search,
    const std::string &replace
);

} // namespace utils
} // namespace genesis

#endif // include guard
