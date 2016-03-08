/**
 * @brief Implementation of date and time functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/tools/date_time.hpp"

#include <cstdio>
#include <ctime>

namespace genesis {
namespace utils {

std::string current_date()
{
    time_t now = time(0);
    tm*    ltm = localtime(&now);

    char out[12];
    sprintf (out, "%u-%02u-%02u",
        ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday
    );
    return out;
}

std::string current_time()
{
    time_t now = time(0);
    tm*    ltm = localtime(&now);

    char out[10];
    sprintf (out, "%02u:%02u:%02u", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return out;
}

} // namespace utils
} // namespace genesis
