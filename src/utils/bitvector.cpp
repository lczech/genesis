/**
 * @brief Implementation of bitvector functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/bitvector.hpp"

#include <algorithm>
#include <functional>

namespace genesis {

// =============================================================================
//     Declarations and Class Functions
// =============================================================================

const Bitvector::IntType Bitvector::all_0_ = 0ul;
const Bitvector::IntType Bitvector::all_1_ = (((1ul << 32) - 1) << 32)  +  ((1ul << 32) - 1);

/**
 * @brief
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
    return *this;
}

Bitvector& Bitvector::operator ^= (Bitvector const& rhs)
{
    size_t min_s = std::min(data_.size(), rhs.data_.size());
    for (size_t i = 0; i < min_s; ++i) {
        data_[i] ^= rhs.data_[i];
    }
    return *this;
}

Bitvector Bitvector::operator ~ () const
{
    Bitvector cpy = Bitvector(*this);
    cpy.Invert();
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

Bitvector Bitvector::SymmetricDifference (Bitvector const& rhs) const
{
    return SymmetricDifference(*this, rhs);
}

Bitvector Bitvector::SymmetricDifference (Bitvector const& lhs, Bitvector const& rhs)
{
    return (lhs | rhs) & ~(lhs & rhs);
}

size_t Bitvector::Count() const
{
    // TODO implement a faster way to count bits!
    size_t res = 0;
    for (size_t i = 0; i < size_; ++i) {
        if (Get(i)) {
            ++res;
        }
    }
    return res;
}

size_t Bitvector::Hash() const
{
    size_t res = 0;
    for (size_t i = 0; i < size_; ++i) {
        if (Get(i)) {
            res ^= std::hash<size_t>()(i);
        }
    }
    return res;
}

Bitvector::IntType Bitvector::XHash() const
{
    IntType res = 0;
    for (IntType e : data_) {
        res ^= e;
    }
    return res;
}

void Bitvector::Invert()
{
    // flip all bits.
    for (size_t i = 0; i < data_.size(); ++i) {
        data_[i] = ~ data_[i];
    }

    // reset the surplus bits at the end of the vector.
    for (size_t i = size_ % IntSize; i < IntSize; ++i) {
        data_.back() ^= bit_mask_[i];
    }
}

void Bitvector::Normalize()
{
    if (Get(0)) {
        Invert();
    }
}

// =============================================================================
//     Dump and Debug
// =============================================================================

std::ostream& operator << (std::ostream& s, Bitvector const& rhs)
{
    for(size_t i = 0; i < rhs.size() ; ++i) {
        if(rhs.Get(i)) {
            s << "1" ;
        } else {
            s << "0";
        }
    }
    return s;
}

std::string Bitvector::Dump() const
{
    std::string res = "[" + std::to_string(size_) + "] ";
    for (size_t i = 0; i < size_; ++i) {
        res += (*this)[i] ? "1" : "0";
        if ((i+1) % 8 == 0) {
            res += " ";
        }
    }
    return res;
}

} // namespace genesis
