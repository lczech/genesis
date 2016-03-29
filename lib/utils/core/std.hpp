#ifndef GENESIS_UTILS_CORE_STD_H_
#define GENESIS_UTILS_CORE_STD_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include <algorithm>
#include <memory>
#include <string>

namespace genesis {

// =================================================================================================
//     Shortcomings of the C++ 11 STL...
// =================================================================================================

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

} // namespace genesis

#endif // include guard
