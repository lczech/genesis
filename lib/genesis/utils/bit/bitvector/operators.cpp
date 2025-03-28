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

#include "genesis/utils/bit/bitvector/operators.hpp"
#include "genesis/utils/bit/bit.hpp"

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
//     Input and Output
// -------------------------------------------------------------------------

std::string bit_string_header( size_t n, bool with_dec_line )
{
    std::string result;
    auto const max_len = n;// std::min( n, static_cast<size_t>( 64 ));
    if( with_dec_line ) {
        for( size_t i = 0; i < max_len; ++i ) {
            result += (i % 10 == 0) ? std::to_string( (i / 10) % 10 ) : " ";
            if( i+1 < n && (i+1) % 8 == 0 ) {
                result += " ";
            }
        }
        result += "\n";
    }
    for( size_t i = 0; i < max_len; ++i ) {
        result += std::to_string( i % 10 );
        if( i+1 < n && (i+1) % 8 == 0 ) {
            result += " ";
        }
    }
    result += "\n";
    return result;
}

std::string to_bit_string( Bitvector const& bv, bool with_line_breaks, char zero, char one )
{
    // std::string res = ( with_size ? "[" + std::to_string( bv.size() ) + "]\n" : "" );
    std::string res;
    for( size_t i = 0; i < bv.size(); ++i ) {
        res += bv[i] ? one : zero;
        if( with_line_breaks && i+1 < bv.size() && (i+1) % 64 == 0 ) {
            res += "\n";
        } else if( i+1 < bv.size() && (i+1) % 8 == 0 ) {
            res += " ";
        }
    }
    return res;
}

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

Serializer& operator<<( Serializer& serializer, Bitvector const& bv )
{
    // We write the size in number of bits first.
    // Then, the data serialization will additionally store the size of the underlying vector
    // that is used in the Bitvector, which is a bit of overhead, but we live with that for now.
    serializer << bv.size();
    serializer << bv.data();
    return serializer;
}

Deserializer& operator>>( Deserializer& deserializer, Bitvector& bv )
{
    // This funciton is a friend of the Bitvector class, so that we can write to its data directly.
    // Otherwise, we'd need special constructors etc, which is a bit cumbersome.
    deserializer >> bv.size_;
    deserializer >> bv.data_;

    // Now that we have read the data, check that it is valid.
    // First, we check the sizes, and then we check that the last bits are already unset,
    // and do not contain any stray set bits that would indicate wrong usage or serialization.
    size_t const expected_size = Bitvector::get_vector_size( bv.size_ );
    if( bv.data_.size() != expected_size ) {
        throw std::invalid_argument(
            "Cannot deserialize Bitvector of expected vector size " + std::to_string( expected_size ) +
            " with actual vector size " + std::to_string( bv.data_.size() )
        );
    }
    if( bv.data_.size() > 0 ) {
        auto const back = bv.data_.back();
        bv.unset_padding_bits();
        if( bv.data_.back() != back ) {
            throw std::invalid_argument(
                "Invalid (de)serialization of Bitvector where last bits after the actual size were set"
            );
        }
    }

    return deserializer;
}

} // namespace utils
} // namespace genesis
