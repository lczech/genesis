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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector/operators.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Operators
// =================================================================================================

// -------------------------------------------------------------------------
//     Bit Operators
// -------------------------------------------------------------------------

/**
 * @brief Local helper function to get the order of two bitvectors for bitwise operations.
 *
 * The first one returned is the vector whose size we want to end up with. The second one is then
 * the one that we use for the bitwise operation.
 */
inline std::pair<Bitvector const*, Bitvector const*> bitwise_operator_order_(
    Bitvector const& lhs, Bitvector const& rhs,
    BitwiseOperatorLengthPolicy length_policy
) {
    Bitvector const* first  = nullptr;
    Bitvector const* second = nullptr;

    switch( length_policy ) {
        case BitwiseOperatorLengthPolicy::kExpectEqual: {
            if( lhs.size() != rhs.size() ) {
                throw std::invalid_argument(
                    "Bitwise operation on bitvectors of different lengths (" +
                    std::to_string( lhs.size() ) + " and " + std::to_string( rhs.size() ) +
                    ") with BitwiseOperatorLengthPolicy::kExpectEqual"
                );
            }
            first  = &lhs;
            second = &rhs;
            break;
        }
        case BitwiseOperatorLengthPolicy::kUseShorter: {
            if( lhs.size() < rhs.size() ) {
                first  = &lhs;
                second = &rhs;
            } else {
                first  = &rhs;
                second = &lhs;
            }
            break;
        }
        case BitwiseOperatorLengthPolicy::kUseLonger: {
            if( lhs.size() < rhs.size() ) {
                first  = &rhs;
                second = &lhs;
            } else {
                first  = &lhs;
                second = &rhs;
            }
            break;
        }
        case BitwiseOperatorLengthPolicy::kUseFirst: {
            first  = &lhs;
            second = &rhs;
            break;
        }
        case BitwiseOperatorLengthPolicy::kUseSecond: {
            first  = &rhs;
            second = &lhs;
            break;
        }
        default: {
            throw std::invalid_argument(
                "Invalid BitwiseOperatorLengthPolicy in bitwise operator"
            );
        }
    }

    return std::make_pair( first, second );
}

Bitvector bitwise_and(
    Bitvector const& lhs, Bitvector const& rhs,
    BitwiseOperatorLengthPolicy length_policy
) {
    // Depending on the length policty, we want to switch between which of the two vectors
    // we use to obtain the final length of the resulting vector. The order function returns
    // the vector as first whose length we want. We use that to create the result vector with
    // that length, but using the data of the second vector to fill it. That is an easy way
    // to get a vector with the desired length that can then combined with the first vector
    // again via the operator, as now both have the same length.
    auto const order = bitwise_operator_order_( lhs, rhs, length_policy );
    auto result = Bitvector( order.first->size(), *order.second );
    result &= *order.first;
    return result;
}

Bitvector bitwise_or(
    Bitvector const& lhs, Bitvector const& rhs,
    BitwiseOperatorLengthPolicy length_policy
) {
    // See above bitwise_and() for details on how this works.
    auto const order = bitwise_operator_order_( lhs, rhs, length_policy );
    auto result = Bitvector( order.first->size(), *order.second );
    result |= *order.first;
    return result;
}

Bitvector bitwise_xor(
    Bitvector const& lhs, Bitvector const& rhs,
    BitwiseOperatorLengthPolicy length_policy
) {
    // See above bitwise_and() for details on how this works.
    auto const order = bitwise_operator_order_( lhs, rhs, length_policy );
    auto result = Bitvector( order.first->size(), *order.second );
    result ^= *order.first;
    return result;
}

// -------------------------------------------------------------------------
//     Set Operators
// -------------------------------------------------------------------------

Bitvector set_minus( Bitvector const& lhs, Bitvector const& rhs )
{
    return lhs & (~rhs);
}

Bitvector symmetric_difference( Bitvector const& lhs, Bitvector const& rhs )
{
    return (lhs | rhs) & ~(lhs & rhs);
}

bool is_strict_subset( Bitvector const& sub, Bitvector const& super )
{
    // Not really efficient. We could stop early in the comparison instead.
    // But as of now, we do not need the speed, so let's keep it simple instead.
    // Same for the other variants of this function below.
    return ((sub & super) == sub) && (sub.count() < super.count());
}

bool is_strict_superset( Bitvector const& super, Bitvector const& sub )
{
    return is_strict_subset( sub, super );
}

bool is_subset( Bitvector const& sub, Bitvector const& super )
{
    return (sub == super) || is_strict_subset(sub, super);
}

bool is_superset( Bitvector const& super, Bitvector const& sub )
{
    return (super == sub) || is_strict_superset(super, sub);
}

double jaccard_similarity( Bitvector const& lhs, Bitvector const& rhs )
{
    // Compute sizes of the intersection and the union.
    // Throws an exception in the and/or operators if the bitvectors are of different length.
    auto const i = ( lhs & rhs ).count();
    auto const u = ( lhs | rhs ).count();
    assert( lhs.size() == rhs.size() );

    // Compute the index, taking care of the edge case.
    if( u == 0 ) {
        assert( i == 0 );
        return 0.0;
    }
    return static_cast<double>( i ) / static_cast<double>( u );
}

double jaccard_distance( Bitvector const& lhs, Bitvector const& rhs )
{
    return 1.0 - jaccard_similarity( lhs, rhs );
}

// -------------------------------------------------------------------------
//     Sorting
// -------------------------------------------------------------------------

// bool lexicographically_compare_helper_( Bitvector const& lhs, Bitvector const& rhs, bool on_equal )
// {
//     // Deactivated at the moment, as this does not take care of the typical little endian-ness
//     // of modern computers, and hence yields wrong results...
//
//     // Local helper function to avoid code duplication.
//     if( lhs.size() != rhs.size() ) {
//         throw std::runtime_error(
//             "Cannot use lexicographical comparison functions on Bitvectors of different size."
//         );
//     }
//     for( size_t i = 0; i < lhs.data().size(); ++i ) {
//         if( lhs.data()[i] < rhs.data()[i] ) {
//             return true;
//         } else if( lhs.data()[i] > rhs.data()[i] ) {
//             return false;
//         }
//     }
//
//     // If we are here, all of the above comparisons shows that lhs == rhs.
//     assert( lhs == rhs );
//     return on_equal;
// }
//
// bool is_lexicographically_less( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( lhs, rhs, false );
// }
//
// bool is_lexicographically_less_or_equal( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( lhs, rhs, true );
// }
//
// bool is_lexicographically_greater( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( rhs, lhs, false );
// }
//
// bool is_lexicographically_greater_or_equal( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( rhs, lhs, true );
// }

// -------------------------------------------------------------------------
//     Input and Output
// -------------------------------------------------------------------------

std::ostream& operator << (std::ostream& s, Bitvector const& bv)
{
    for( size_t i = 0; i < bv.size(); ++i ) {
        s << (bv.get(i) ? "1" : "0");
    }
    return s;
}

std::istream& operator >> ( std::istream& in, Bitvector& bv )
{
    // We need two steps, as we have to construct the bitvector with a known size.
    // First, bring the bits into string form...
    std::string str;
    auto c = in.peek();
    while( c == '0' || c == '1' ) {
        str += c;
        (void) in.get();
        c = in.peek();
    }

    // ... then, create the bitvector.
    bv = Bitvector( str.size() );
    for( size_t i = 0; i < str.size(); ++i ) {
        if( str[i] == '1' ) {
            bv.set(i);
        }
    }
    return in;
}

std::vector<bool> make_bool_vector_from_indices( std::vector<size_t> const& indices, size_t size )
{
    // Get the largest element of the vector. If it's empty, we return an all-false vector.
    auto max_it = std::max_element( indices.begin(), indices.end() );
    if( max_it == indices.end() ) {
        return std::vector<bool>( size, false );
    }
    size_t target_size = *max_it + 1;
    if( size > 0 ) {
        if( target_size > size ) {
            throw std::invalid_argument(
                "Cannot use make_bool_vector_from_indices() with size " + std::to_string( size ) +
                " that is smaller than required to include the larged index " +
                std::to_string( *max_it ) + " in the list of indices (zero-based)."
            );
        }
        target_size = size;
    }

    // Fill a bool vector, setting all positions to true
    // that are indicated by the indices, pun intended.
    auto result = std::vector<bool>( target_size, false );
    for( auto const& idx : indices ) {
        assert( idx < result.size() );
        result[idx] = true;
    }
    return result;
}

} // namespace utils
} // namespace genesis
