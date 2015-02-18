/**
 * @brief Implementation of Settings members.
 *
 * @file
 * @ingroup main
 */

#include "main/settings.hpp"

#ifdef PTHREADS
#    include <thread>
#endif

namespace genesis {

Settings::Settings()
{

    // initialize threads with actual number of cores
#ifdef PTHREADS
    number_of_threads = std::thread::hardware_concurrency();
#endif

}

} // namespace genesis

