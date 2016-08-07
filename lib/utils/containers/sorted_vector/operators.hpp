#ifndef GENESIS_UTILS_CONTAINERS_SORTED_VECTOR_OPERATORS_H_
#define GENESIS_UTILS_CONTAINERS_SORTED_VECTOR_OPERATORS_H_

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/containers/sorted_vector.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Sorted Vector
// =================================================================================================

template< class T>
bool operator == ( SortedVector<T> const& lhs, SortedVector<T> const& rhs )
{
    if( lhs.size() != rhs.size() ) {
        return false;
    }
    for( size_t i = 0; i < lhs.size(); ++i ) {
        if( lhs[i] != rhs[i] ) {
            return false;
        }
    }
    return true;
}

template< class T>
bool operator != ( SortedVector<T> const& lhs, SortedVector<T> const& rhs )
{
    return !( lhs == rhs );
}

template< class T>
bool equals( SortedVector<T> const& lhs, SortedVector<T> const& rhs )
{
    return lhs == rhs;
}

template< class T>
bool is_subset_of( SortedVector<T> const& subset, SortedVector<T> const& superset )
{
    if( subset.size() >= superset.size() ) {
        return false;
    }

    // Simple, but slow.
    // for( auto const& elem : subset ) {
    //     if( ! superset.contains( elem )) {
    //         return false;
    //     }
    // }

    // Iterate the subset values, and in parallel, iterate the superset to check if all values are
    // contained in it.
    auto it_sup = superset.begin();
    for( auto sub_elem : subset ) {
        while( it_sup != superset.end() && *it_sup != sub_elem ) {
            ++it_sup;
        }
        if( it_sup == superset.end() ) {
            return false;
        }
    }
    return true;
}

template< class T>
bool is_subset_or_equal( SortedVector<T> const& subset, SortedVector<T> const& superset )
{
    if( subset.size() == superset.size() ) {
        return ( subset == superset );
    } else {
        return is_subset_of( subset, superset );
    }
}

} // namespace utils
} // namespace genesis

#endif // include guard
