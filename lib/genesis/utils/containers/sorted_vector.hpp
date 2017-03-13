#ifndef GENESIS_UTILS_CONTAINERS_SORTED_VECTOR_H_
#define GENESIS_UTILS_CONTAINERS_SORTED_VECTOR_H_

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Sorted Vector
// =================================================================================================

/**
 * @brief Sorted vector of unique elements.
 *
 * This class offers a thin wrapper around std::vector which ensures that the stored elements are
 * unique and sorted. It also offers to retrieve the index of a value, see index_of().
 */
template< class T, class Compare = std::less<T> >
class SortedVector
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using value_type      = T;

    using       reference =       value_type&;
    using const_reference = const value_type&;
    using       pointer   =       value_type*;
    using const_pointer   = const value_type*;

    using       iterator  = typename std::vector< value_type >::iterator;
    using const_iterator  = typename std::vector< value_type >::const_iterator;

    using size_type       = size_t;
    using value_compare   = Compare;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    SortedVector()  = default;
    ~SortedVector() = default;

    SortedVector( SortedVector const& ) = default;
    SortedVector ( SortedVector&& )      = default;

    SortedVector( std::initializer_list<value_type> il )
    {
        content_.reserve( il.size() );
        for( auto const& elem : il ) {
            insert( elem );
        }
    }

    SortedVector& operator= ( SortedVector const& ) = default;
    SortedVector& operator= ( SortedVector&& )      = default;

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin()
    {
        return content_.begin();
    }

    const_iterator begin() const
    {
        return content_.cbegin();
    }

    iterator end()
    {
        return content_.end();
    }

    const_iterator end() const
    {
        return content_.cend();
    }

    const_iterator cbegin()
    {
        return content_.cbegin();
    }

    const_iterator cend()
    {
        return content_.cend();
    }

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    size_type size() const
    {
        return content_.size();
    }

    bool empty() const
    {
        return content_.empty();
    }

    // -------------------------------------------------------------------------
    //     Find Elements
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether a certain value is present in the container.
     */
    bool contains( const_reference value ) const
    {
        return std::binary_search( content_.begin(), content_.end(), value, Compare() );
    }

    /**
     * @brief Return the index at which a certain value is stored, or size(), if it is not
     * present in the container.
     */
    size_type index_of( const_reference value ) const
    {
        auto lower = std::lower_bound( content_.begin(), content_.end(), value, Compare() );

        if( lower == content_.end() || value < *lower ) {
            return size();
        } else {
            return std::distance( content_.begin(), lower );
        }
    }

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

    reference operator[] ( size_type index )
    {
        return content_[ index ];
    }

    const_reference operator[] ( size_type index ) const
    {
        return content_[ index ];
    }

    reference at( size_type index )
    {
        return content_.at( index );
    }

    const_reference at( size_type index ) const
    {
        return content_.at( index );
    }

    reference front()
    {
        return content_.front();
    }

    const_reference front() const
    {
        return content_.front();
    }

    reference back()
    {
        return content_.back();
    }

    const_reference back() const
    {
        return content_.back();
    }

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Insert a value into the container by copying it.
     *
     * If a value comparing equal to the new one is already stored in the container, nothing happens.
     */
    void insert( const_reference value )
    {
        // Find the position where the new value belongs.
        auto range = std::equal_range( content_.begin(), content_.end(), value, Compare() );

        // If it is not yet in the list, add it.
        if( range.first == range.second ) {
            content_.insert( range.first, value );
        }
    }


    /**
    * @brief Insert a value into the container by moving it.
    *
    * If a value comparing equal to the new one is already stored in the container, nothing happens.
    */
    void insert( value_type&& value )
    {
        // Find the position where the new value belongs.
        auto range = std::equal_range( content_.begin(), content_.end(), value, Compare() );

        // If it is not yet in the list, add it.
        if( range.first == range.second ) {
            content_.insert( range.first, std::move( value ));
        }
    }

    /**
     * @brief Insert values into the container by copying from an InputIterator range.
     *
     * The function copies all values in the range `[ first, last )` into the container.
     */
    template< class InputIterator >
    void insert( InputIterator first, InputIterator last )
    {
        while( first != last ) {
            insert( *first );
            ++first;
        }
    }

    /**
     * @brief Remove a certain value from the container.
     *
     * If the value is not stored in the container, nothing happens.
     */
    void remove( const_reference value )
    {
        // Find the position of the value.
        auto range = std::equal_range( content_.begin(), content_.end(), value, Compare() );

        // If found, remove it.
        if( range.first != range.second ) {
            content_.erase( range.first, range.second );
        }
    }

    /**
     * @brief Reserve space in the unterlying vector.
     */
    void reserve( size_t n )
    {
        content_.reserve( n );
    }

    void clear()
    {
        content_.clear();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector< value_type > content_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
