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
 * @brief Implementation of bitvector functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector.hpp"

#include <algorithm>
#include <assert.h>
#include <functional>

namespace genesis {
namespace utils {

// =============================================================================
//     Declarations and Class Functions
// =============================================================================

const Bitvector::IntType Bitvector::all_0_ = 0ul;
const Bitvector::IntType Bitvector::all_1_ = (((1ul << 32) - 1) << 32)  +  ((1ul << 32) - 1);

/**
 * @brief Contains a single bit at each of the 64 positions.
 */
const Bitvector::IntType Bitvector::bit_mask_[Bitvector::IntSize] =
{
    1ul << 0,   1ul << 1,   1ul << 2,   1ul << 3,   1ul << 4,   1ul << 5,   1ul << 6,   1ul << 7,
    1ul << 8,   1ul << 9,   1ul << 10,  1ul << 11,  1ul << 12,  1ul << 13,  1ul << 14,  1ul << 15,
    1ul << 16,  1ul << 17,  1ul << 18,  1ul << 19,  1ul << 20,  1ul << 21,  1ul << 22,  1ul << 23,
    1ul << 24,  1ul << 25,  1ul << 26,  1ul << 27,  1ul << 28,  1ul << 29,  1ul << 30,  1ul << 31,
    1ul << 32,  1ul << 33,  1ul << 34,  1ul << 35,  1ul << 36,  1ul << 37,  1ul << 38,  1ul << 39,
    1ul << 40,  1ul << 41,  1ul << 42,  1ul << 43,  1ul << 44,  1ul << 45,  1ul << 46,  1ul << 47,
    1ul << 48,  1ul << 49,  1ul << 50,  1ul << 51,  1ul << 52,  1ul << 53,  1ul << 54,  1ul << 55,
    1ul << 56,  1ul << 57,  1ul << 58,  1ul << 59,  1ul << 60,  1ul << 61,  1ul << 62,  1ul << 63
};

/**
 * @brief Contains as many ones as the position in the array tells.
 *
 * The element at position `i` contains `i` many ones, starting from the right:
 *
 *     ones_mask_[0] --> 0 ones: 0000...0000
 *     ones_mask_[1] --> 1 one:  0000...0001
 *     ones_mask_[2] --> 2 ones: 0000...0011
 *     ...
 *
 * This mask is used for unsetting the padding bits in unset_padding().
 */
const Bitvector::IntType Bitvector::ones_mask_[Bitvector::IntSize] =
{
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
};

/**
 * @brief Mask used for quickly counting the number of set bits, see count().
 */
const Bitvector::IntType Bitvector::count_mask_[4] =
{
    0x5555555555555555,  //binary: 0101...
    0x3333333333333333,  //binary: 00110011...
    0x0f0f0f0f0f0f0f0f,  //binary: 4 zeros, 4 ones...
    0x0101010101010101   //the sum of 256 to the power of 0,1,2,3...
};

Bitvector operator & (Bitvector const& lhs, Bitvector const& rhs)
{
    // make a copy.
    Bitvector result = Bitvector(lhs);

    // check for self-and.
    if (&lhs == &rhs) {
        return result;
    }

    // if not, return and with right hand side.
    result &= rhs;
    return result;
}

Bitvector operator | (Bitvector const& lhs, Bitvector const& rhs)
{
    // make a copy.
    Bitvector result = Bitvector(lhs);

    // check for self-or.
    if (&lhs == &rhs) {
        return result;
    }

    // if not, return or with right hand side.
    result |= rhs;
    return result;
}

Bitvector operator ^ (Bitvector const& lhs, Bitvector const& rhs)
{
    // check for self-xor. if so, return zero vector of same size.
    if (&lhs == &rhs) {
        return Bitvector(lhs.size(), false);
    }

    // otherwise, make a copy and xor it.
    Bitvector result = Bitvector(lhs);
    result ^= rhs;
    return result;
}

/**
 * @brief Set-minus of two Bitvectors.
 */
Bitvector operator - (Bitvector const& lhs, Bitvector const& rhs)
{
    return lhs & (~rhs);
}

// =============================================================================
//     Operators
// =============================================================================

Bitvector& Bitvector::operator &= (Bitvector const& rhs)
{
    size_t min_s = std::min(data_.size(), rhs.data_.size());
    for (size_t i = 0; i < min_s; ++i) {
        data_[i] &= rhs.data_[i];
    }
    return *this;
}

Bitvector& Bitvector::operator |= (Bitvector const& rhs)
{
    size_t min_s = std::min(data_.size(), rhs.data_.size());
    for (size_t i = 0; i < min_s; ++i) {
        data_[i] |= rhs.data_[i];
    }
    unset_padding();
    return *this;
}

Bitvector& Bitvector::operator ^= (Bitvector const& rhs)
{
    size_t min_s = std::min(data_.size(), rhs.data_.size());
    for (size_t i = 0; i < min_s; ++i) {
        data_[i] ^= rhs.data_[i];
    }
    unset_padding();
    return *this;
}

Bitvector Bitvector::operator ~ () const
{
    Bitvector cpy = Bitvector(*this);
    cpy.invert();
    return cpy;
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

// =============================================================================
//     Other Functions
// =============================================================================

Bitvector Bitvector::symmetric_difference (Bitvector const& rhs) const
{
    return symmetric_difference(*this, rhs);
}

Bitvector Bitvector::symmetric_difference (Bitvector const& lhs, Bitvector const& rhs)
{
    return (lhs | rhs) & ~(lhs & rhs);
}

/**
 * @brief Counts the number of set bits in the Bitvector.
 */
size_t Bitvector::count() const
{
    size_t res = 0;
    for (IntType x : data_) {
        // put count of each 2 bits into those 2 bits
        x -= (x >> 1) & count_mask_[0];

        // put count of each 4 bits into those 4 bits
        x = (x & count_mask_[1]) + ((x >> 2) & count_mask_[1]);

        // put count of each 8 bits into those 8 bits
        x = (x + (x >> 4)) & count_mask_[2];

        // take left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
        res += (x * count_mask_[3]) >> 56;
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

/**
 * @brief Returns an std::hash value for the Bitvector.
 */
size_t Bitvector::hash() const
{
    // TODO this might be a poor hash function. check what kind of value a hash fct needs to return and decide whether xhash is a good choice instead!
    size_t res = 0;
    for (size_t i = 0; i < size_; ++i) {
        if (get(i)) {
            res ^= std::hash<size_t>()(i);
        }
    }
    return res;
}

/**
 * @brief Returns a hash value of type IntType, that is quicker to calculate than hash(), and thus
 * can be used where the std::hash is not needed.
 */
Bitvector::IntType Bitvector::x_hash() const
{
    IntType res = 0;
    for (IntType e : data_) {
        res ^= e;
    }
    return res;
}

/**
 * @brief Flip all bits.
 */
void Bitvector::invert()
{
    // flip all bits.
    for (size_t i = 0; i < data_.size(); ++i) {
        data_[i] = ~ data_[i];
    }

    // reset the surplus bits at the end of the vector.
    unset_padding();
}

/**
 * @brief Brings the Bitvector in a normalized form, where the first bit is always zero.
 *
 * If the first bit is zero, nothing happens. However, if is is one, the whole Bitvector is flipped
 * using invert().
 */
void Bitvector::normalize()
{
    if (size_ > 0 && get(0)) {
        invert();
    }
}

/**
 * @brief Reset all the bits to false. If provided with parameter `true`, sets all bits to true.
 */
void Bitvector::reset(const bool value)
{
    // set according to flag.
    const auto v = value ? all_1_ : all_0_;
    for (size_t i = 0; i < data_.size(); ++i) {
        data_[i] = v;
    }

    // if we initialized with true, we need to unset the surplus bits at the end!
    if (value) {
        unset_padding();
    }
}

/**
 * @brief Internal function that sets all bits to zero that are not actively used.
 *
 * The data_ buffer always contains a multiple of IntSize many bits, thus there might be surplus
 * bits at its end for padding. In case we do operations with Bitvectors of different size, these
 * might be affected, so we need to reset them to zero sometimes.
 */
void Bitvector::unset_padding()
{
    if (size_ % IntSize == 0) {
        return;
    }
    data_.back() &= ones_mask_[size_ % IntSize];

    // other versions that might be helpful if i messed up with this little/big endian stuff...
    // first one is slow but definitely works, second one is fast, but might have the same
    // issue as the used version above (which currently works perfectly).
    //~ for (size_t i = size_ % IntSize; i < IntSize; ++i) {
        //~ data_.back() &= ~bit_mask_[i];
    //~ }
    //~ data_.back() &= bit_mask_[size_ % IntSize] - 1;
}

// =============================================================================
//     Dump and Debug
// =============================================================================

std::ostream& operator << (std::ostream& s, Bitvector const& rhs)
{
    for(size_t i = 0; i < rhs.size() ; ++i) {
        s << (rhs.get(i) ? "1" : "0");
    }
    return s;
}

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
