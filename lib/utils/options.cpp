/**
 * @brief Implementation of Options members.
 *
 * @file
 * @ingroup main
 */

#include "utils/options.hpp"

#include "utils/logging.hpp"

#ifdef PTHREADS
#    include <thread>
#endif

namespace genesis {

// =============================================================================
//     Static Data Members
// =============================================================================

unsigned int              Options::number_of_threads = 1;
std::vector<std::string>  Options::arguments;

// =============================================================================
//     Initialization
// =============================================================================

/**
 * @brief Init method that takes the program's command line arguments.
 */
void Options::init (int argc, char* argv[])
{
    // store all arguments in the array.
    for (int i = 0; i < argc; i++) {
        arguments.push_back(argv[i]);
    }

    // initialize threads with actual number of cores.
#   ifdef PTHREADS
        Options::number_of_threads = std::thread::hardware_concurrency();
#   endif
}

// =============================================================================
//     Getter for Properties
// =============================================================================

/**
 * @brief Returns an array of strings containing the program's command line arguments.
 */
std::vector<std::string> Options::get_command_line ()
{
    return Options::arguments;
}

/**
 * @brief Returns a string containing the program's command line arguments.
 */
std::string Options::get_command_line_string ()
{
    std::string ret = "";
    for (size_t i = 0; i < arguments.size(); ++i) {
        std::string a = arguments[i];
        ret += (i==0 ? "" : " ") + a;
    }
    return ret;
}

} // namespace genesis

