#ifndef GENESIS_UTILS_CORE_ALGORITHM_H_
#define GENESIS_UTILS_CORE_ALGORITHM_H_

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
#include <cmath>
#include <functional>
#include <numeric>
#include <vector>

namespace genesis {
namespace utils {

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

/**
 * @brief Get the indices to the sorted order of a vector.
 *
 * This function returns a list of indices into the given vector, so that their order gives the
 * sorted content of `v`:
 *
 *     for (auto i: sort_indices(v)) {
 *         cout << v[i] << endl;
 *     }
 *
 * This is useful if the same sorting order needs to be applied to some other container.
 *
 * The optional parameter `comparator` can be used to specify the function for comparing two
 * values of `v`, and defaults to `std::less`.
 */
template <typename T>
std::vector<size_t> sort_indices(
    std::vector<T> const& v,
    std::function< bool( T const& lhs, T const& rhs )> comparator = std::less<T>()
) {
    // Initialize original index locations with increasing numbers.
    std::vector<size_t> idx( v.size() );
    std::iota( idx.begin(), idx.end(), 0 );

    // Sort indexes based on comparing values in v.
    std::sort( idx.begin(), idx.end(), [&] ( size_t i1, size_t i2 ) {
        return comparator( v[i1], v[i2] );
    });

    return idx;
}

} // namespace utils
} // namespace genesis

#endif // include guard
