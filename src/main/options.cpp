/**
 * @brief Implementation of Options members.
 *
 * @file
 * @ingroup main
 */

#include "main/options.hpp"

#include "utils/logging.hpp"

#ifdef PTHREADS
#    include <thread>
#endif

namespace genesis {

/**
 * @brief
 */
Options::Options()
{
    // initialize threads with actual number of cores
#   ifdef PTHREADS
        number_of_threads = std::thread::hardware_concurrency();
#   endif
}

/**
 * @brief
 */
bool Options::SetCommandLine (int argc, char* argv[])
{
    for (int i = 0; i < argc; i++) {
        arguments.push_back(argv[i]);
    }
    return true;
}

/**
 * @brief
 */
std::vector<std::string> Options::GetCommandLine ()
{
    return arguments;
}

/**
 * @brief
 */
std::string Options::GetCommandLineString ()
{
    std::string ret = "";
    for (size_t i = 0; i < arguments.size(); ++i) {
        std::string a = arguments[i];
        ret += (i==0 ? "" : " ") + a;
    }
    return ret;
}

} // namespace genesis

