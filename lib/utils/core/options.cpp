/**
 * @brief Implementation of Options members.
 *
 * @file
 * @ingroup main
 */

#include "utils/core/options.hpp"

#include <chrono>

#ifdef PTHREADS
#    include <thread>
#endif

namespace genesis {

// =============================================================================
//     Initialization
// =============================================================================

/**
 * @brief Constructor, which initializes the options with reasonable defaults.
 */
Options::Options()
{
    // Initialize threads with actual number of cores.
#   ifdef PTHREADS
        number_of_threads_ = std::thread::hardware_concurrency();
#   else
        number_of_threads_ = 1;
#   endif

    // Initialize random seed with time.
    random_seed_ = std::chrono::system_clock::now().time_since_epoch().count();
    random_engine_.seed(random_seed_);
}

// =============================================================================
//     Setters for Properties
// =============================================================================

/**
 * @brief Set arguments to the program's command line options.
 *
 * If the program is run from the command line, this method has to be used to properly
 * propagate the command line options to this options class.
 */
void Options::set_command_line (const int argc, const char* argv[])
{
    // Store all arguments in the array.
    command_line_.clear();
    for (int i = 0; i < argc; i++) {
        command_line_.push_back(argv[i]);
    }
}

/**
 * @brief Overwrite the system given number of threads.
 *
 * On startup, the value is initialized with the actual number of cores available in the system
 * using std::thread::hardware_concurrency(). This method overwrites this value.
 */
void Options::set_number_of_threads (const unsigned int number)
{
    number_of_threads_ = number;
}

/**
 * @brief Set a specific seed for the random engine.
 *
 * On startup, the random engine is initialized using the current system time. This value can
 * be overwritten using this method.
 */
void Options::set_random_seed(const unsigned seed)
{
    random_seed_ = seed;
    random_engine_.seed(seed);
}

// =============================================================================
//     Getters for Properties
// =============================================================================

/**
 * @brief Returns a string containing the program's command line arguments.
 */
std::string Options::command_line_string () const
{
    std::string ret = "";
    for (size_t i = 0; i < command_line_.size(); ++i) {
        std::string a = command_line_[i];
        ret += (i==0 ? "" : " ") + a;
    }
    return ret;
}

/**
 * @brief Return a list of all options with their values.
 */
std::string Options::dump () const
{
    std::string res = "";
    res += "Command line:      " + command_line_string();
    res += "Number of threads: " + std::to_string(number_of_threads());
    res += "Random seed:       ";
    return res;
}

} // namespace genesis
