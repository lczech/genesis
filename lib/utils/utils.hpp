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


/**
 * @brief Erases all elements from the container that satisfy a given predicate.
 * An element is erased, if the predicate evaluates to true for it.
 * The predicate needs to have a signature similar to (T const&)->bool.
 *
 * @param c The container to modify.
 * @param p The predicate to satisfy.
*/
template< class Container, class UnaryPredicate >
inline void erase_if( Container &c, UnaryPredicate p )
{
	using std::begin;
	using std::end;
	using std::remove_if;

	auto old_end = end( c );
	auto new_end = remove_if( begin( c ), old_end, p );

	if ( new_end != old_end ) {
		c.erase( new_end, old_end );
	}
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

} // namespace genesis

#endif // include guard
