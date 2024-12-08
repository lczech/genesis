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
 * @brief Implementation of bitvector functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector.hpp"

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/bit.hpp"

#include <algorithm>
#include <cstring>
#include <functional>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace genesis {
namespace utils {

// =============================================================================
//     Constants
// =============================================================================

const Bitvector::IntType Bitvector::ALL_0 = 0ul;
const Bitvector::IntType Bitvector::ALL_1 = (((1ul << 32) - 1) << 32)  +  ((1ul << 32) - 1);

const std::array<Bitvector::IntType, Bitvector::IntSize> Bitvector::BIT_MASKS =
{{
    1ul << 0,   1ul << 1,   1ul << 2,   1ul << 3,   1ul << 4,   1ul << 5,   1ul << 6,   1ul << 7,
    1ul << 8,   1ul << 9,   1ul << 10,  1ul << 11,  1ul << 12,  1ul << 13,  1ul << 14,  1ul << 15,
    1ul << 16,  1ul << 17,  1ul << 18,  1ul << 19,  1ul << 20,  1ul << 21,  1ul << 22,  1ul << 23,
    1ul << 24,  1ul << 25,  1ul << 26,  1ul << 27,  1ul << 28,  1ul << 29,  1ul << 30,  1ul << 31,
    1ul << 32,  1ul << 33,  1ul << 34,  1ul << 35,  1ul << 36,  1ul << 37,  1ul << 38,  1ul << 39,
    1ul << 40,  1ul << 41,  1ul << 42,  1ul << 43,  1ul << 44,  1ul << 45,  1ul << 46,  1ul << 47,
    1ul << 48,  1ul << 49,  1ul << 50,  1ul << 51,  1ul << 52,  1ul << 53,  1ul << 54,  1ul << 55,
    1ul << 56,  1ul << 57,  1ul << 58,  1ul << 59,  1ul << 60,  1ul << 61,  1ul << 62,  1ul << 63
}};

const std::array<Bitvector::IntType, Bitvector::IntSize> Bitvector::ONES_MASKS =
{{
    Bitvector::ALL_0,       Bitvector::ALL_1 >> 63, Bitvector::ALL_1 >> 62, Bitvector::ALL_1 >> 61,
    Bitvector::ALL_1 >> 60, Bitvector::ALL_1 >> 59, Bitvector::ALL_1 >> 58, Bitvector::ALL_1 >> 57,
    Bitvector::ALL_1 >> 56, Bitvector::ALL_1 >> 55, Bitvector::ALL_1 >> 54, Bitvector::ALL_1 >> 53,
    Bitvector::ALL_1 >> 52, Bitvector::ALL_1 >> 51, Bitvector::ALL_1 >> 50, Bitvector::ALL_1 >> 49,
    Bitvector::ALL_1 >> 48, Bitvector::ALL_1 >> 47, Bitvector::ALL_1 >> 46, Bitvector::ALL_1 >> 45,
    Bitvector::ALL_1 >> 44, Bitvector::ALL_1 >> 43, Bitvector::ALL_1 >> 42, Bitvector::ALL_1 >> 41,
    Bitvector::ALL_1 >> 40, Bitvector::ALL_1 >> 39, Bitvector::ALL_1 >> 38, Bitvector::ALL_1 >> 37,
    Bitvector::ALL_1 >> 36, Bitvector::ALL_1 >> 35, Bitvector::ALL_1 >> 34, Bitvector::ALL_1 >> 33,
    Bitvector::ALL_1 >> 32, Bitvector::ALL_1 >> 31, Bitvector::ALL_1 >> 30, Bitvector::ALL_1 >> 29,
    Bitvector::ALL_1 >> 28, Bitvector::ALL_1 >> 27, Bitvector::ALL_1 >> 26, Bitvector::ALL_1 >> 25,
    Bitvector::ALL_1 >> 24, Bitvector::ALL_1 >> 23, Bitvector::ALL_1 >> 22, Bitvector::ALL_1 >> 21,
    Bitvector::ALL_1 >> 20, Bitvector::ALL_1 >> 19, Bitvector::ALL_1 >> 18, Bitvector::ALL_1 >> 17,
    Bitvector::ALL_1 >> 16, Bitvector::ALL_1 >> 15, Bitvector::ALL_1 >> 14, Bitvector::ALL_1 >> 13,
    Bitvector::ALL_1 >> 12, Bitvector::ALL_1 >> 11, Bitvector::ALL_1 >> 10, Bitvector::ALL_1 >> 9,
    Bitvector::ALL_1 >> 8,  Bitvector::ALL_1 >> 7,  Bitvector::ALL_1 >> 6,  Bitvector::ALL_1 >> 5,
    Bitvector::ALL_1 >> 4,  Bitvector::ALL_1 >> 3,  Bitvector::ALL_1 >> 2,  Bitvector::ALL_1 >> 1
}};

// =============================================================================
//     Constructor and Rule of Five
// =============================================================================

Bitvector::Bitvector( size_t size, Bitvector const& other )
    : Bitvector::Bitvector( size, false )
{
    // Static test, needs to be here, as the constant is private.
    static_assert(
        Bitvector::ALL_1 == static_cast<Bitvector::IntType>(0) - 1,
        "Bitvector::ALL_1 is not all one"
    );

    if( &other == this ) {
        throw std::runtime_error(
            "In Bitvector::Bitvector( size_t, Bitvector const& ): Cannot self assign."
        );
    }

    // Copy over all data, making sure to not go past the end of either vector.
    // If other is smaller than the size we are creating here, we are technically copying
    // is padding bits as well, but those are false anyway, so that's okay.
    auto const n = std::min( data_.size(), other.data_.size() );
    for( size_t i = 0; i < n; ++i ) {
        data_[i] = other.data_[i];
    }
    unset_padding_bits();
}

Bitvector::Bitvector( std::string const& values )
    : Bitvector::Bitvector( values.size(), false )
{
    for( size_t i = 0; i < values.size(); ++i ) {
        switch( values[i] ) {
            case '0':
                break;
            case '1':
                set(i);
                break;
            default:
                throw std::invalid_argument(
                    "Cannot construct Bitvector from std::string that contains characters "
                    "other than 0 and 1."
                );
        }
    }
}

// Bitvector::Bitvector( Bitvector const& other, size_t max_size )
// {
//     if( max_size > other.size() ) {
//         max_size = other.size();
//     }
//     size_ = max_size;
//     auto const ds = (size_ / IntSize) + (size_ % IntSize == 0 ? 0 : 1);
//     assert( ds <= other.data_.size() );
//     data_ = std::vector<IntType>( other.data_.begin(), other.data_.begin() + ds );
//     unset_padding_bits();
// }

// =============================================================================
//     Bit Functions
// =============================================================================

void Bitvector::set_range( size_t first, size_t last, bool value )
{
    // Boundary checks
    if( first >= size_ || last > size_ || first > last ) {
        throw std::out_of_range(
            "Cannot access invalid range [" + std::to_string(first) + ", " + std::to_string(last) +
            ") in Bitvector of size " + std::to_string(size_)
        );
    }
    assert( first <  size_ );
    assert( last  <= size_ );
    assert( first <= last );

    // Check special case, as we might otherwise access invalid data at the boundaries.
    if( first == last ) {
        return;
    }
    assert( last > 0 );

    // Get word indices, and bit position indices within those words.
    // The last word is the one where the bit before `last` is, as `last` is past-the-end.
    // However, the bit index is still needs to be past-the-end, to use the proper mask.
    auto const f_wrd_idx = first / IntSize;
    auto const l_wrd_idx = (last - 1) / IntSize;
    auto const f_bit_idx = first % IntSize;
    auto const l_bit_idx = last % IntSize;
    assert( f_wrd_idx < data_.size() );
    assert( l_wrd_idx < data_.size() );
    assert( f_bit_idx < ONES_MASKS.size() );
    assert( l_bit_idx < ONES_MASKS.size() );

    // Get the two words at the boundary. We later check if they are the same,
    // so we do not repeat the code here, and treat the special case later.
    auto const f_mask = ~ONES_MASKS[ f_bit_idx ];
    auto const l_mask = l_bit_idx == 0 ? ALL_1 : ONES_MASKS[ l_bit_idx ];

    // Now set the bits as needed for the range.
    if( f_wrd_idx == l_wrd_idx ) {
        if( value ) {
            data_[ f_wrd_idx ] |=  ( f_mask & l_mask );
        } else {
            data_[ f_wrd_idx ] &= ~( f_mask & l_mask );
        }
    } else {
        if( value ) {
            data_[ f_wrd_idx ] |= f_mask;
            for( size_t i = f_wrd_idx + 1; i < l_wrd_idx; ++i ) {
                data_[i] = ALL_1;
            }
            data_[ l_wrd_idx ] |= l_mask;
        } else {
            data_[ f_wrd_idx ] &= ~f_mask;
            for( size_t i = f_wrd_idx + 1; i < l_wrd_idx; ++i ) {
                data_[i] = ALL_0;
            }
            data_[ l_wrd_idx ] &= ~l_mask;
        }
    }
}

void Bitvector::set_all( bool value )
{
    // set according to flag.
    const auto v = value ? ALL_1 : ALL_0;
    for( size_t i = 0; i < data_.size(); ++i ) {
        data_[i] = v;
    }

    // if we initialized with true, we need to unset the surplus bits at the end!
    if( value ) {
        unset_padding_bits();
    }
}

void Bitvector::unset_padding_bits()
{
    // Only apply if there are actual padding bits.
    if(( size_ % IntSize ) == 0 ) {
        assert( size_ / IntSize == data_.size() );
        return;
    }

    // In the other cases, unset the padding.
    assert( size_ / IntSize + 1 == data_.size() );
    assert( size_ % IntSize < ONES_MASKS.size() );
    data_.back() &= ONES_MASKS[ size_ % IntSize ];

    // other versions that might be helpful if i messed up with this little/big endian stuff...
    // first one is slow but definitely works, second one is fast, but might have the same
    // issue as the used version above (which currently works perfectly).
    //~ for( size_t i = size_ % IntSize; i < IntSize; ++i ) {
        //~ data_.back() &= ~BIT_MASKS[i];
    //~ }
    //~ data_.back() &= BIT_MASKS[size_ % IntSize] - 1;
}

void Bitvector::negate()
{
    // flip all bits.
    for( size_t i = 0; i < data_.size(); ++i ) {
        data_[i] = ~ data_[i];
    }

    // reset the surplus bits at the end of the vector.
    unset_padding_bits();
}

void Bitvector::invert()
{
    return negate();
}

// =============================================================================
//     Operators
// =============================================================================

Bitvector& Bitvector::operator &= (Bitvector const& rhs)
{
    if( size_ != rhs.size_ ) {
        throw std::runtime_error(
            "Cannot use operator `&` or `&=` on Bitvectors of different size. "
            "Use bitwise_and() instead."
        );
    }

    for( size_t i = 0; i < data_.size(); ++i ) {
        data_[i] &= rhs.data_[i];
    }
    return *this;
}

Bitvector& Bitvector::operator |= (Bitvector const& rhs)
{
    if( size_ != rhs.size_ ) {
        throw std::runtime_error(
            "Cannot use operator `|` or `|=` on Bitvectors of different size. "
            "Use bitwise_or() instead."
        );
    }

    for( size_t i = 0; i < data_.size(); ++i ) {
        data_[i] |= rhs.data_[i];
    }
    return *this;
}

Bitvector& Bitvector::operator ^= (Bitvector const& rhs)
{
    if( size_ != rhs.size_ ) {
        throw std::runtime_error(
            "Cannot use operator `^` or `^=` on Bitvectors of different size. "
            "Use bitwise_xor() instead."
        );
    }

    for( size_t i = 0; i < data_.size(); ++i ) {
        data_[i] ^= rhs.data_[i];
    }
    return *this;
}

Bitvector Bitvector::operator ~ () const
{
    Bitvector cpy = Bitvector(*this);
    cpy.negate();
    return cpy;
}

Bitvector operator & (Bitvector const& lhs, Bitvector const& rhs)
{
    Bitvector result = Bitvector(lhs);
    result &= rhs;
    return result;
}

Bitvector operator | (Bitvector const& lhs, Bitvector const& rhs)
{
    Bitvector result = Bitvector(lhs);
    result |= rhs;
    return result;
}

Bitvector operator ^ (Bitvector const& lhs, Bitvector const& rhs)
{
    Bitvector result = Bitvector(lhs);
    result ^= rhs;
    return result;
}

bool Bitvector::operator == (const Bitvector &other) const
{
    if (size_ != other.size_) {
        return false;
    }
    for( size_t i = 0; i < data_.size(); ++i ) {
        if (data_[i] != other.data_[i]) {
            return false;
        }
    }
    return true;
}

bool Bitvector::operator != (const Bitvector &other) const
{
    return !(*this == other);
}

} // namespace utils
} // namespace genesis
