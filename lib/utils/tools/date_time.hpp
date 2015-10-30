#ifndef GENESIS_UTILS_TOOLS_DATE_TIME_H_
#define GENESIS_UTILS_TOOLS_DATE_TIME_H_

/**
 * @brief Provides functions for date and time access.
 *
 * @file
 * @ingroup utils
 */

#include <string>

namespace genesis {

/**
 * @brief Returns the current date as a string in the format "2014-12-31".
 */
std::string current_date();

/**
 * @brief Returns the current time as a string in the format "13:37:42".
 */
std::string current_time();

} // namespace genesis

#endif // include guard
