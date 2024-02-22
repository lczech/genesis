#ifndef GENESIS_UTILS_CORE_THREAD_FUNCTIONS_H_
#define GENESIS_UTILS_CORE_THREAD_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/multi_future.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/thread_pool.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Critical Section
// =================================================================================================

/**
 * @brief Helper class to define a critical section.
 *
 * This is similar to `#pragma omp critical`, in order to synchronize access to a criticla section
 * across multiple threads. The class internally uses a `std::mutex` to guarantee exclusive access
 * for each thread.
 *
 * The class is templated with a `Tag`, so that mutiple independent sections can be defined, each
 * getting their own mutex. The usage for this class is hence to first define the access tag, and
 * then use it as follows:
 *
 *     // Define section tag
 *     struct MyThreadCriticalSection{};
 *
 *     // Start a critical section
 *     {
 *     ThreadCriticalSection<MyThreadCriticalSection> cs;
 *
 *     // Critical section code for MyThreadCriticalSection
 *     std::cout << "Task " << id << " is running in MyThreadCriticalSection." << std::endl;
 *     std::this_thread::sleep_for(std::chrono::milliseconds(100));
 *
 *     } // MyThreadCriticalSection mutex is automatically released here
 *
 * To simplify this, instead of having to define the tag struct each time, we also provide a
 * macro `GENESIS_THREAD_CRITICAL_SECTION(TagName)` to this end:
 *
 *     // Start a critical section
 *     {
 *     GENESIS_THREAD_CRITICAL_SECTION(MyThreadCriticalSection)
 *     //...
 *     }
 *
 * This can however not be used if multiple sections need to be synchronized with the same tag,
 * as the macro would define the same struct multiple times; depending on the scope, this leads
 * either to a multiple definition error, or to structs that are local to their scope, and hence
 * lead to sections that do not synchronize with each other. In that case, use the above instead,
 * with a tag struct that is defined in a fitting scope that is visible to all sections that need
 * to be protected by it.
 */
template<typename Tag>
class ThreadCriticalSection
{
public:

    ThreadCriticalSection()
    {
        get_static_mutex_().lock();
    }

    ~ThreadCriticalSection()
    {
        get_static_mutex_().unlock();
    }

private:

    static std::mutex& get_static_mutex_()
    {
        static std::mutex mutex_;
        return mutex_;
    }
};

// Macro to define a tag and create a ThreadCriticalSection instance
#define GENESIS_THREAD_CRITICAL_SECTION(TagName) \
    struct TagName {}; \
    ThreadCriticalSection<TagName> genesis_critical_section_##TagName;

} // namespace utils
} // namespace genesis

#endif // include guard
