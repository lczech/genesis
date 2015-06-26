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
#include <iomanip>
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

bool        dir_exists     (const std::string& dir);
bool        dir_list_files (const std::string& dir, std::vector<std::string>& list);

std::unordered_map<std::string, std::string> file_info (std::string filename);

size_t      file_size      (std::string filename);
std::string file_path      (std::string filename);
std::string file_basename  (std::string filename);
std::string file_filename  (std::string filename);
std::string file_extension (std::string filename);

std::string string_escape   (const std::string& text);
std::string string_deescape (const std::string& text);
std::string string_replace_all (
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
bool contains(const C& v, const T& x)
{
    return std::end(v) != std::find(std::begin(v), std::end(v), x);
}

/**
 * @brief Returns a std::unique_ptr for a given type.
 *
 * Since this is not available in C++11, we need our own implementation.
 * It is following http://herbsutter.com/gotw/_102/
 */
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

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

// ---------------------------------------------------------
//     Strings and Chars
// ---------------------------------------------------------

/** @brief Returns whether a char is a digit (0-9). */
inline bool char_is_digit (const char c)
{
    return ('0' <= c) && (c <= '9');
}

/** @brief Returns whether a char is a sign (+-). */
inline bool char_is_sign (const char c)
{
    return ('+' == c) || ('-' == c);
}

/**
 * @brief Returns whether two chars are the same, case insensitive.
 */
inline bool char_match(const char c1, const char c2)
{
    return std::tolower(c1) == std::tolower(c2);
}

/**
 * @brief Returns a precise(er than to_string) string representation of the input value.
 */
template <typename T>
inline std::string to_string_precise (const T value, const int precision = 6)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

/**
 * @brief Returns a copy of the input string, with left trimmed white spaces.
 */
inline std::string string_trim_right (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return s.substr(0, s.find_last_not_of(delimiters) + 1);
}

/**
 * @brief Returns a copy of the input string, with right trimmed white spaces.
 */
inline std::string string_trim_left (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return s.substr(s.find_first_not_of(delimiters));
}

/**
 * @brief Returns a copy of the input string, with trimmed white spaces.
 */
inline std::string string_trim (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return string_trim_left(string_trim_right(s, delimiters), delimiters);
}

inline std::string string_unify_newlines(const std::string& s)
{
    // TODO string_unify_newlines
    return s;
}

} // namespace genesis

#endif // include guard
