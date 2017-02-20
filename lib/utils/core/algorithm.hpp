#ifndef GENESIS_UTILS_CORE_ALGORITHM_H_
#define GENESIS_UTILS_CORE_ALGORITHM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @brief Provides some valuable algorithms that are not part of the C++ 11 STL.
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <cassert>
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
inline bool contains(const C& v, const T& x)
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

// =================================================================================================
//     Sorting
// =================================================================================================

/**
 * @copydoc sort_indices( RandomAccessIterator, RandomAccessIterator )
 */
template <typename RandomAccessIterator, typename Comparator>
inline std::vector<size_t> sort_indices(
    RandomAccessIterator first,
    RandomAccessIterator last,
    Comparator           comparator
) {
    // Initialize original index locations with increasing numbers.
    size_t size = std::distance( first, last );
    std::vector<size_t> idx( size );
    std::iota( idx.begin(), idx.end(), 0 );

    // Sort indexes based on comparing values of the iterator range.
    std::sort( idx.begin(), idx.end(), [&] ( size_t i1, size_t i2 ) {
        assert( first + i1 < last );
        assert( first + i2 < last );
        return comparator( *(first + i1), *(first + i2) );
    });

    return idx;
}

/**
 * @brief Get the indices to the sorted order of the given range.
 *
 * This function returns a list of indices into the given range, so that their order gives the
 * sorted content of the elements in `[ first, last )`, using `std::less` for comparisons.
 *
 * For example,
 *
 *     for( auto i: sort_indices(v) ) {
 *         cout << *( first + i ) << endl;
 *     }
 *
 * outputs the elements in the range in sorted order, without actually moving any elements
 * in the range.
 * This is useful if the same sorting order needs to be applied to some other container.
 *
 * There is also an overload of this function that takes an additional parameter `comparator`.
 * It can be used to specify the function for comparing two values of the range.
 *
 * Furthermore, there are variants that use `std::stable_sort` instead, see stable_sort_indices().
 */
template <typename RandomAccessIterator>
inline std::vector<size_t> sort_indices(
    RandomAccessIterator first,
    RandomAccessIterator last
) {
    return sort_indices(
        first,
        last,
        std::less< typename std::iterator_traits<RandomAccessIterator>::value_type >()
    );
}

/**
 * @copydoc stable_sort_indices( RandomAccessIterator, RandomAccessIterator )
 */
template <typename RandomAccessIterator, typename Comparator>
inline std::vector<size_t> stable_sort_indices(
    RandomAccessIterator first,
    RandomAccessIterator last,
    Comparator           comparator
) {
    // Initialize original index locations with increasing numbers.
    size_t size = std::distance( first, last );
    std::vector<size_t> idx( size );
    std::iota( idx.begin(), idx.end(), 0 );

    // Sort indexes based on comparing values of the iterator range.
    std::stable_sort( idx.begin(), idx.end(), [&] ( size_t i1, size_t i2 ) {
        assert( first + i1 < last );
        assert( first + i2 < last );
        return comparator( *(first + i1), *(first + i2) );
    });

    return idx;
}

/**
 * @brief Get the indices to the stable sorted order of the given range.
 *
 * This function returns a list of indices into the given range, so that their order gives the
 * stable sorted content of the elements in `[ first, last )`, using `std::less` for comparisons.
 *
 * For example,
 *
 *     for( auto i: stable_sort_indices(v) ) {
 *         cout << *( first + i ) << endl;
 *     }
 *
 * outputs the elements in the range in stable sorted order, without actually moving any elements
 * in the range.
 * This is useful if the same sorting order needs to be applied to some other container.
 *
 * There is also an overload of this function that takes an additional parameter `comparator`.
 * It can be used to specify the function for comparing two values of the range.
 *
 * Furthermore, there are variants that use normal `std::sort` instead, see sort_indices().
 */
template <typename RandomAccessIterator>
inline std::vector<size_t> stable_sort_indices(
    RandomAccessIterator first,
    RandomAccessIterator last
) {
    return stable_sort_indices(
        first,
        last,
        std::less< typename std::iterator_traits<RandomAccessIterator>::value_type >()
    );
}

} // namespace utils
} // namespace genesis

#endif // include guard
