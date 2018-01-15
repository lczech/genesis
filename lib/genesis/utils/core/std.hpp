#ifndef GENESIS_UTILS_CORE_STD_H_
#define GENESIS_UTILS_CORE_STD_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Provides some valuable additions to STD.
 *
 * @file
 * @ingroup utils
 */

#include <cstdint>
#include <memory>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Expection Handling
// =================================================================================================

// Try to find a macro that expands to the current function name.
#ifdef __cplusplus
#    define GENESIS_FUNC __PRETTY_FUNCTION__
#else
#    if defined __STDC_VERSION__
#        define GENESIS_FUNC __func__
#    else
#        define GENESIS_FUNC __FUNCTION__
// #        define GENESIS_FUNC ((const char *) 0)
#    endif
#endif

// =================================================================================================
//     Shortcomings of the C++ 11 STL...
// =================================================================================================

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
 * @brief Proxy class to hold an element accessible via arrow operator.
 *
 * This is useful for implementing an iterator whose arrow operator would return an r-value.
 * Inspired by https://stackoverflow.com/a/26496041/4184258
 */
template <typename T>
class ArrowOperatorProxy {
public:

    ArrowOperatorProxy( T const& t )
        : t(t)
    {}

    T* operator ->() const {
        return &t;
    }

private:

    T t;
};

} // namespace utils
} // namespace genesis

#endif // include guard
