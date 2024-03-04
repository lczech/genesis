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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief Implementation of bitvector functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector.hpp"

#include "genesis/utils/core/std.hpp"

#include <algorithm>
#include <cstring>
#include <functional>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =============================================================================
//     Constants
// =============================================================================

const Bitvector::IntType Bitvector::all_0_ = 0ul;
const Bitvector::IntType Bitvector::all_1_ = (((1ul << 32) - 1) << 32)  +  ((1ul << 32) - 1);

const std::array<Bitvector::IntType, Bitvector::IntSize> Bitvector::bit_mask_ =
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

const std::array<Bitvector::IntType, Bitvector::IntSize> Bitvector::ones_mask_ =
{{
    Bitvector::all_0_,       Bitvector::all_1_ >> 63, Bitvector::all_1_ >> 62, Bitvector::all_1_ >> 61,
    Bitvector::all_1_ >> 60, Bitvector::all_1_ >> 59, Bitvector::all_1_ >> 58, Bitvector::all_1_ >> 57,
    Bitvector::all_1_ >> 56, Bitvector::all_1_ >> 55, Bitvector::all_1_ >> 54, Bitvector::all_1_ >> 53,
    Bitvector::all_1_ >> 52, Bitvector::all_1_ >> 51, Bitvector::all_1_ >> 50, Bitvector::all_1_ >> 49,
    Bitvector::all_1_ >> 48, Bitvector::all_1_ >> 47, Bitvector::all_1_ >> 46, Bitvector::all_1_ >> 45,
    Bitvector::all_1_ >> 44, Bitvector::all_1_ >> 43, Bitvector::all_1_ >> 42, Bitvector::all_1_ >> 41,
    Bitvector::all_1_ >> 40, Bitvector::all_1_ >> 39, Bitvector::all_1_ >> 38, Bitvector::all_1_ >> 37,
    Bitvector::all_1_ >> 36, Bitvector::all_1_ >> 35, Bitvector::all_1_ >> 34, Bitvector::all_1_ >> 33,
    Bitvector::all_1_ >> 32, Bitvector::all_1_ >> 31, Bitvector::all_1_ >> 30, Bitvector::all_1_ >> 29,
    Bitvector::all_1_ >> 28, Bitvector::all_1_ >> 27, Bitvector::all_1_ >> 26, Bitvector::all_1_ >> 25,
    Bitvector::all_1_ >> 24, Bitvector::all_1_ >> 23, Bitvector::all_1_ >> 22, Bitvector::all_1_ >> 21,
    Bitvector::all_1_ >> 20, Bitvector::all_1_ >> 19, Bitvector::all_1_ >> 18, Bitvector::all_1_ >> 17,
    Bitvector::all_1_ >> 16, Bitvector::all_1_ >> 15, Bitvector::all_1_ >> 14, Bitvector::all_1_ >> 13,
    Bitvector::all_1_ >> 12, Bitvector::all_1_ >> 11, Bitvector::all_1_ >> 10, Bitvector::all_1_ >> 9,
    Bitvector::all_1_ >> 8,  Bitvector::all_1_ >> 7,  Bitvector::all_1_ >> 6,  Bitvector::all_1_ >> 5,
    Bitvector::all_1_ >> 4,  Bitvector::all_1_ >> 3,  Bitvector::all_1_ >> 2,  Bitvector::all_1_ >> 1
}};

const std::array<Bitvector::IntType, 4> Bitvector::count_mask_ =
{{
    0x5555555555555555,  //binary: 0101...
    0x3333333333333333,  //binary: 00110011...
    0x0f0f0f0f0f0f0f0f,  //binary: 4 zeros, 4 ones...
    0x0101010101010101   //the sum of 256 to the power of 0,1,2,3...
}};

// =============================================================================
//     Constructor and Rule of Five
// =============================================================================

Bitvector::Bitvector( size_t size, Bitvector const& other )
    : Bitvector::Bitvector( size, false )
{
    // Static test, needs to be here, as the constant is private.
    static_assert(
        Bitvector::all_1_ == static_cast<Bitvector::IntType>(0) - 1,
        "Bitvector::all_1_ is not all one"
    );

    // if( &other == this ) {
    //     throw std::runtime_error(
    //         "In Bitvector::Bitvector( size_t, Bitvector const& ): Cannot self assign."
    //     );
    // }

    // Copy over all data, making sure to not go past the end of either vector.
    // If other is smaller than the size we are creating here, we are technically copying
    // is padding bits as well, but those are false anyway, so that's okay.
    auto const n = std::min( data_.size(), other.data_.size() );
    for( size_t i = 0; i < n; ++i ) {
        data_[i] = other.data_[i];
    }
    unset_padding_();
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
//     unset_padding_();
// }

// =============================================================================
//     Bit Functions
// =============================================================================

void Bitvector::set( size_t first, size_t last, bool value )
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
    assert( f_bit_idx < ones_mask_.size() );
    assert( l_bit_idx < ones_mask_.size() );

    // Get the two words at the boundary. We later check if they are the same,
    // so we do not repeat the code here, and treat the special case later.
    auto const f_mask = ~ones_mask_[ f_bit_idx ];
    auto const l_mask = l_bit_idx == 0 ? all_1_ : ones_mask_[ l_bit_idx ];

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
                data_[i] = all_1_;
            }
            data_[ l_wrd_idx ] |= l_mask;
        } else {
            data_[ f_wrd_idx ] &= ~f_mask;
            for( size_t i = f_wrd_idx + 1; i < l_wrd_idx; ++i ) {
                data_[i] = all_0_;
            }
            data_[ l_wrd_idx ] &= ~l_mask;
        }
    }
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

    for (size_t i = 0; i < data_.size(); ++i) {
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

    for (size_t i = 0; i < data_.size(); ++i) {
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

    for (size_t i = 0; i < data_.size(); ++i) {
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
    for (size_t i = 0; i < data_.size(); ++i) {
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

// =============================================================================
//     Other Functions
// =============================================================================

size_t Bitvector::count() const
{
    // Use bit trickery to count quickly.
    size_t res = 0;
    for (IntType x : data_) {
        res += count_(x);
    }

    // safe, but slow version...
    //~ size_t tmp = 0;
    //~ for (size_t i = 0; i < size_; ++i) {
        //~ if (get(i)) {
            //~ ++tmp;
        //~ }
    //~ }
    //~ assert(tmp == res);

    return res;
}

size_t Bitvector::count( size_t first, size_t last ) const
{
    // Boundary checks.
    if( first >= size_ || last > size_ || first > last ) {
        throw std::invalid_argument(
            "Cannot compute pop count for Bitvector of size " + std::to_string( size_ ) +
            " within invalid range [" + std::to_string(first) + "," + std::to_string( last ) + ")"
        );
    }
    assert( first <  size_ );
    assert( last  <= size_ );
    assert( first <= last );

    // Check special case, as we might otherwise access invalid data at the boundaries.
    if( first == last ) {
        return 0;
    }
    assert( last > 0 );

    // We need to mask the first bits of the first word and last bits of the last word
    // before counting, and then can process the in-between words normally.
    // If first and last are the same word, we need special treatment as well.

    // Get word indices, and bit position indices within those words.
    // The last word is the one where the bit before last is, as last is past-the-end.
    // However, the bit index is still meant to be past-the-end, to use the proper mask.
    auto const f_wrd_idx = first / IntSize;
    auto const l_wrd_idx = (last - 1) / IntSize;
    auto const f_bit_idx = first % IntSize;
    auto const l_bit_idx = last % IntSize;
    assert( f_wrd_idx < data_.size() );
    assert( l_wrd_idx < data_.size() );
    assert( f_bit_idx < ones_mask_.size() );
    assert( l_bit_idx < ones_mask_.size() );

    // Get the two words at the boundary. We later check if they are the same,
    // so we do not repeat the code here, and treat the special case later.
    auto f_word = data_[ f_wrd_idx ];
    auto l_word = data_[ l_wrd_idx ];

    // Mask out the beginning and end, respectively.
    // Remove all bits before the first index, and all bits after and including the last index.
    // No special case needed here, as the 0th mask is idempotent.
    // That's because it's the mask that we also use for unset_padding_(),
    // we are basically doing the same here.
    f_word &= ~ones_mask_[ f_bit_idx ];
    if( l_bit_idx != 0 ) {
        l_word &=  ones_mask_[ l_bit_idx ];
    }

    // Finally, count up all the parts.
    size_t result = 0;
    if( f_wrd_idx == l_wrd_idx ) {
        // Same word. Mask out the bits we don't want, using only the bits that remained after
        // filtering both words (which are the same, just different ends of the word), then count.
        result = count_( f_word & l_word );
    } else {
        // Count the first and last word, and then add everything in between the two.
        result = count_( f_word ) + count_( l_word );
        for( size_t i = f_wrd_idx + 1; i < l_wrd_idx; ++i ) {
            result += count_( data_[i] );
        }
    }
    return result;
}

size_t Bitvector::find_next_set( size_t start ) const
{
    // Boundary check
    if( start >= size_ ) {
        // We mimic the behaviour of std::string::find(), which just never finds anything
        // when used beyond the string, but also does not throw an exception in such cases.
        return std::numeric_limits<size_t>::max();
        // throw std::invalid_argument(
        //     "Invalid call to find_next_set() with start==" + std::to_string(start) +
        //     " and a Bitvector of size " + std::to_string( size_ )
        // );
    }

    // Helper function to find the index of the first set bit in a non-zero word.
    auto find_next_set_in_word_ = []( IntType word ) {
        assert( word != 0 );

        // We use ffs here, see https://man7.org/linux/man-pages/man3/ffs.3.html
        // It returns the _position_ of the bit, so we need to subtract 1 to get the index.
        // Alternatively, we could use __builtin_ctz, which returns the number of trailing
        // zeros in the given word, but is a compiler intrinsic, so let's stay with POSIX.

        // Check the size of the input and call the appropriate ffs function.
        // Any good compiler will see through this and make this constexpr.
        // In C++17, we could do this ourselves ;-)
        if( sizeof(word) <= sizeof(unsigned int) ) {
            return ffs( static_cast<unsigned int>( word )) - 1;
        } else if( sizeof(word) <= sizeof(unsigned long) ) {
            return ffsl( static_cast<unsigned long>( word )) - 1;
        } else {
            return ffsll(word) - 1;
        }
    };

    // First see if there is anything in the word at the start position.
    // We assume that this function might be called on a dense bitvector,
    // where the given position is already set, so we check that as a shortcut.
    if( get( start )) {
        return start;
    }

    // If that did not work, we see if there is anything in the current word.
    auto wrd_idx = start / IntSize;
    auto bit_idx = start % IntSize;
    assert( wrd_idx < data_.size() );
    assert( bit_idx < ones_mask_.size() );
    auto word = data_[wrd_idx];

    // For this, we remove the bits before start, and then test the rest.
    // Mask out the beginning of the word, and find the next bit on the remainder.
    // Remove all bits before the first index.
    word &= ~( ones_mask_[ bit_idx ]);
    if( word != 0 ) {
        return wrd_idx * IntSize + find_next_set_in_word_( word );
    }

    // We did not find a bit in the word of the start. So now we look for the first word
    // after the start one that has bits set, and return its first set bit position.
    ++wrd_idx;
    while( wrd_idx < data_.size() && data_[wrd_idx] == 0 ) {
        ++wrd_idx;
    }
    if( wrd_idx == data_.size() ) {
        return std::numeric_limits<size_t>::max();
    }
    assert( wrd_idx < data_.size() );
    assert( data_[wrd_idx] != 0 );
    return wrd_idx * IntSize + find_next_set_in_word_( data_[wrd_idx] );
}

size_t Bitvector::hash() const
{
    std::size_t res = 0;
    for( auto const& d : data_ ) {
        res = hash_combine( res, d );
    }
    return res;
}

Bitvector::IntType Bitvector::x_hash() const
{
    IntType res = 0;
    for( auto const& d : data_ ) {
        res ^= d;
    }
    return res;
}

void Bitvector::negate()
{
    // flip all bits.
    for (size_t i = 0; i < data_.size(); ++i) {
        data_[i] = ~ data_[i];
    }

    // reset the surplus bits at the end of the vector.
    unset_padding_();
}

void Bitvector::normalize()
{
    if (size_ > 0 && get(0)) {
        negate();
    }
}

void Bitvector::set_all( const bool value )
{
    // set according to flag.
    const auto v = value ? all_1_ : all_0_;
    for( size_t i = 0; i < data_.size(); ++i ) {
        data_[i] = v;
    }

    // if we initialized with true, we need to unset the surplus bits at the end!
    if( value ) {
        unset_padding_();
    }
}

// =============================================================================
//     Internal Members
// =============================================================================

void Bitvector::unset_padding_()
{
    // Only apply if there are actual padding bits.
    if(( size_ % IntSize ) == 0 ) {
        assert( size_ / IntSize == data_.size() );
        return;
    }

    // In the other cases, unset the padding.
    assert( size_ / IntSize + 1 == data_.size() );
    assert( size_ % IntSize < ones_mask_.size() );
    data_.back() &= ones_mask_[ size_ % IntSize ];

    // other versions that might be helpful if i messed up with this little/big endian stuff...
    // first one is slow but definitely works, second one is fast, but might have the same
    // issue as the used version above (which currently works perfectly).
    //~ for (size_t i = size_ % IntSize; i < IntSize; ++i) {
        //~ data_.back() &= ~bit_mask_[i];
    //~ }
    //~ data_.back() &= bit_mask_[size_ % IntSize] - 1;
}

size_t Bitvector::count_( IntType x )
{
    // Use some bit magic, see e.g., https://en.wikipedia.org/wiki/Hamming_weight

    // put count of each 2 bits into those 2 bits
    x -= (x >> 1) & count_mask_[0];

    // put count of each 4 bits into those 4 bits
    x = (x & count_mask_[1]) + ((x >> 2) & count_mask_[1]);

    // put count of each 8 bits into those 8 bits
    x = (x + (x >> 4)) & count_mask_[2];

    // take left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
    return (x * count_mask_[3]) >> 56;
}

// =============================================================================
//     Dump and Debug
// =============================================================================

std::string Bitvector::dump() const
{
    std::string res = "[" + std::to_string(size_) + "]\n";
    for (size_t i = 0; i < size_; ++i) {
        res += (*this)[i] ? "1" : "0";
        if ((i+1) % 64 == 0) {
            res += "\n";
        } else if ((i+1) % 8 == 0) {
            res += " ";
        }
    }
    return res;
}

std::string Bitvector::dump_int(IntType x) const
{
    std::string res = "";
    for (size_t i = 0; i < IntSize; ++i) {
        res += (x & bit_mask_[i] ? "1" : "0");
        if ((i+1) % 8 == 0) {
            res += " ";
        }
    }
    return res;
}

} // namespace utils
} // namespace genesis
