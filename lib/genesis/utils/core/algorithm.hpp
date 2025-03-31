#ifndef GENESIS_UTILS_CORE_ALGORITHM_H_
#define GENESIS_UTILS_CORE_ALGORITHM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
#include <cstddef>
#include <functional>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Shortcomings of the C++ 11 STL...
// =================================================================================================

/**
 * @brief Binary search on a sorted/partitioned range, returns an iterator to the element if found.
 *
 * See https://en.cppreference.com/w/cpp/algorithm/lower_bound for details.
 */
template<class ForwardIt, class T, class Compare=std::less<T>>
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp={})
{
    // Note: BOTH type T and the type after ForwardIt is dereferenced
    // must be implicitly convertible to BOTH Type1 and Type2, used in Compare.
    // This is stricter than lower_bound requirement (see above)

    first = std::lower_bound(first, last, value, comp);
    return first != last && !comp(value, *first) ? first : last;
}

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
 * @brief Return whether a container contains duplicates.
 *
 * The container does not need to be sorted.
 * For speed, the function internally copies the elements of the container.
 */
template<class C>
inline bool contains_duplicates( C const& v )
{
    auto test = std::unordered_set< typename C::value_type >();
    for( auto const& e : v ) {
        if( test.count(e) > 0 ) {
            return true;
        }
        test.insert(e);
    }
    return false;
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
 * @brief Returns the lowest and the greatest of the given values, by value.
 *
 * The function differs from `std::minmax` in its return value. Instead of references,
 * it returns by value, which is useful for integers and the like.
 */
template<class T>
std::pair<T, T> minmax_value( T const& a, T const& b )
{
    return (a < b) ? std::pair<T, T>(a, b) : std::pair<T, T>(b, a);
}

/**
 * @copydoc minmax_value( T const& a, T const& b )
 */
template<class T, class Compare>
std::pair<T, T> minmax_value( T const& a, T const& b, Compare comp )
{
    return comp(a, b) ? std::pair<T, T>(a, b) : std::pair<T, T>(b, a);
}

// =================================================================================================
//     Insert Sorted
// =================================================================================================

/**
 * @brief Insert into a vector @p vec, sorted by the value of the @p item.
 * The vector must already be sorted.
 */
template< typename T >
typename std::vector<T>::iterator
insert_sorted( std::vector<T> & vec, T const& item )
{
    return vec.insert(
        std::upper_bound( vec.begin(), vec.end(), item ),
        item
    );
}

/**
 * @brief Insert into a vector @p vec, sorted by the value of the @p item, using a predicate.
 * The vector must already be sorted according to the predicate.
 */
template< typename T, typename Pred >
typename std::vector<T>::iterator
insert_sorted( std::vector<T> & vec, T const& item, Pred pred )
{
    return vec.insert(
       std::upper_bound( vec.begin(), vec.end(), item, pred ),
       item
    );
}

// =================================================================================================
//     Concatenate
// =================================================================================================

/**
 * @brief Helper functions to append one container to the result container.
 *
 * Overload for lvalue containers (copying elements).
 */
template<typename Container>
void append_container( Container& result, Container const& c )
{
    // Insert using copy iterators.
    result.insert(result.end(), c.begin(), c.end());
}

/**
 * @brief Helper functions to append one container to the result container.
 *
 * Overload for rvalue containers (moving elements).
 */
template<typename Container>
void append_container( Container& result, Container&& c )
{
    // Insert using move iterators.
    result.insert(
        result.end(),
        std::make_move_iterator( c.begin() ),
        std::make_move_iterator( c.end() )
    );
}

/**
 * @brief Concatenate elements in containers.
 *
 * This function takes two or more containers of the same type and
 * returns a new container with all elements concatenated in order.
 *
 * The return type is deduced from the decayed type of the first argument.
 * Elements are either copied or moved based on the value category of
 * each argument (lvalue => copy, rvalue => move).
 */
template<typename Container, typename... Containers>
auto concatenate(Container&& c, Containers&&... cs)
    -> typename std::decay<Container>::type
{
    // Define the type of the container that we will return.
    typedef typename std::decay<Container>::type ContainerType;
    ContainerType result;

    // Calculate the total size (assumes the container type supports size()).
    std::size_t total_size = std::forward<Container>(c).size();

    // Use an initializer list to expand the parameter pack.
    using expander = int[];
    (void) expander {0, (total_size += std::forward<Containers>(cs).size(), 0)...};

    // Reserve enough memory to avoid multiple reallocations.
    result.reserve(total_size);

    // Append the first container.
    append_container(result, std::forward<Container>(c));
    // Append the rest of the containers.
    (void) expander {0, (append_container(result, std::forward<Containers>(cs)), 0)...};

    return result;
}

// =================================================================================================
//     Sort Indices
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
    std::iota( idx.begin(), idx.end(), static_cast<size_t>(0) );

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
    std::iota( idx.begin(), idx.end(), static_cast<size_t>(0) );

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
