#ifndef GENESIS_UTILS_UTILS_H_
#define GENESIS_UTILS_UTILS_H_

/**
 * @brief Provides some commonly used utility functions.
 *
 * @file
 * @ingroup utils
 */

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

/**
 * @namespace genesis
 *
 * @brief This namespace contains all symbols of genesis in order to keep them
 * separate when used as a library.
 */
namespace genesis {

// =============================================================================
//     First: function declaractions.
// =============================================================================

bool        FileExists (const std::string& fn);
std::string FileRead   (const std::string& fn);
bool        FileWrite  (const std::string& fn, const std::string& content);

bool        DirListFiles (const std::string& dir, std::vector<std::string>& list);

std::string StringEscape   (const std::string& text);
std::string StringDeescape (const std::string& text);
std::string StringReplaceAll (
    const std::string& text,
    const std::string& search,
    const std::string& replace
);

// =============================================================================
//     From here on: only inline functions.
// =============================================================================

// ---------------------------------------------------------
//     Shortcomings of the C++ 11 STL...
// ---------------------------------------------------------

/**
 * @brief Returns whether a container object has a certain element.
 *
 * The usage of std::find just to check for presence of a certain item is a bit cumbersome.
 * This template simply takes any container and a value and returns true iff the value is
 * present in the container.
 */
template<class C, class T>
bool Contains(const C& v, const T& x)
{
    return std::end(v) != std::find(std::begin(v), std::end(v), x);
}

// ---------------------------------------------------------
//     Date and Time
// ---------------------------------------------------------

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

inline std::string ee(int r)
{
    uint64_t x[4] = {1198840465960072866,1198609267608314688,1376216421886990656,1545107134173456};
    std::string s; for(int i=0;i<(2*r)/3;++i) { s += (((x[(i/7)%4]/r)>>((i%7)*8))%256); } return s;
}

// ---------------------------------------------------------
//     Strings and Chars
// ---------------------------------------------------------

/** @brief Returns whether a char is a digit (0-9). */
inline bool CharIsDigit (const char c)
{
    return ('0' <= c) && (c <= '9');
}

/** @brief Returns whether a char is a sign (+-). */
inline bool CharIsSign (const char c)
{
    return ('+' == c) || ('-' == c);
}

/**
 * @brief Returns whether two chars are the same, case insensitive.
 */
inline bool CharMatch(const char c1, const char c2)
{
    return std::tolower(c1) == std::tolower(c2);
}

/**
 * @brief Returns a precise(er than to_string) string representation of the input value.
 */
template <typename T>
inline std::string ToStringPrecise (const T value, const int precision = 6)
{
    std::ostringstream out;
    out << std::setprecision(precision) << value;
    return out.str();
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

inline std::string StringUnifyNewlines(const std::string& s)
{
    // TODO StringUnifyNewlines
    return s;
}

} // namespace genesis

#endif // include guard
