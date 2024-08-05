#ifndef GENESIS_UTILS_THREADING_MULTI_FUTURE_H_
#define GENESIS_UTILS_THREADING_MULTI_FUTURE_H_

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

/*
    The code in this source file is inspired by BS::thread_pool,
    see https://github.com/bshoshany/thread-pool, which we adapted it to our coding standards.

    We here hence need to include the following original license of BS::thread_pool:

    MIT License

    Copyright (c) 2022 Barak Shoshany

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/threading/thread_pool.hpp"

#include <cassert>
#include <future>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Multi Future
// =================================================================================================

/**
 * @brief Helper class to facilitate waiting for and/or getting the results of multiple futures at once.
 */
template <typename T>
class MultiFuture
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct with the given number of futures.
     */
    MultiFuture( size_t n = 0 )
        : futures_( n )
    {}

    // Copy construction deleted, as future cannot be copied
    MultiFuture( MultiFuture const& ) = delete;
    MultiFuture( MultiFuture&& )      = default;

    MultiFuture& operator= ( MultiFuture const& ) = delete;
    MultiFuture& operator= ( MultiFuture&& )      = default;

    ~MultiFuture() = default;

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    /**
     * @brief Append a future.
     */
    void push_back( ProactiveFuture<T> future )
    {
        futures_.push_back( std::move(future) );
    }

    /**
     * @brief Return whether all stored futures are valid.
     */
    bool valid() const
    {
        for( auto const& fut : futures_ ) {
            if( ! fut.valid() ) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Get the results from all the stored futures.
     *
     * This is the version for `T` being `void`, in which case the futures do not return a value.
     * In this case, this function is equivalent to calling wait().
     */
    template <typename U = T, typename std::enable_if<std::is_same<U, void>::value>::type* = nullptr>
    void get()
    {
        // Internal check to see if we got the SFINAE right...
        static_assert(
            std::is_same<T, void>::value, "Function is meant for T == void"
        );

        // Wait for all futures.
        for( size_t i = 0; i < futures_.size(); ++i ) {
            futures_[i].get();
        }
        return;
    }

    /**
     * @brief Get the results from all the stored futures.
     *
     * This is the version for `T` begin not `void`.
     * The function calls get() on all futures, and returns their values as a vector.
     * It re-throws any stored exceptions of the futures.
     */
    template <typename U = T, typename std::enable_if< !std::is_same<U, void>::value>::type* = nullptr>
    std::vector<T> get()
    {
        // Internal check to see if we got the SFINAE right...
        static_assert(
            ! std::is_same<T, void>::value, "Function is meant for T != void"
        );

        // Get all futures
        std::vector<T> result( futures_.size() );
        for( size_t i = 0; i < futures_.size(); ++i ) {
            result[i] = std::move( futures_[i].get() );
        }
        return result;
    }

    /**
     * @brief Wait for all the stored futures.
     */
    void wait() const
    {
        for( size_t i = 0; i < futures_.size(); ++i ) {
            futures_[i].wait();
        }
    }

    /**
     * @brief Get a reference to one of the stored futures.
     */
    ProactiveFuture<T>& operator[]( size_t i )
    {
        return futures_[i];
    }

    /**
     * @brief Get a const reference to one of the stored futures.
     */
    ProactiveFuture<T> const& operator[]( size_t i ) const
    {
        return futures_[i];
    }

    /**
     * @brief Get the number of stored futures.
     */
    size_t size() const
    {
        return futures_.size();
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    std::vector<ProactiveFuture<T>> futures_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
