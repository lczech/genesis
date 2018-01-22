#ifndef GENESIS_UTILS_MATH_BITVECTOR_H_
#define GENESIS_UTILS_MATH_BITVECTOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector
// =================================================================================================

class Bitvector
{
public:

    // ---------------------------------------------------------
    //     Typedefs, Enums, Constants
    // ---------------------------------------------------------

    typedef uint64_t    IntType;
    static const size_t IntSize = sizeof(IntType) * 8;

    // ---------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------

    /**
     * @brief Default constructor. Creates an empty Bitvector of size 0.
     */
    Bitvector() = default;

    /**
     * @brief Constructor that takes a size and an optional bool value to initialize the Bitvector,
     * false by default.
     */
    Bitvector (const size_t size, const bool initial_value = false)
        : size_(size)
    {
        // reserve enough bits, and init them.
        data_.resize( (size / IntSize) + (size % IntSize == 0 ? 0 : 1) );
        set_all(initial_value);
    }

    /**
     * @brief Constructor that takes a size and a list of values (positions) to be set to true.
     */
    Bitvector (const size_t size, const std::initializer_list<int> list)
        : Bitvector(size, false)
    {
        for (int e : list) {
            set(e);
        }
    }

    /**
     * @brief Create a Bitvector by copying the first @p bits of another Bitvector.
     *
     * If `bits > other.size()`, all bits are used.
     */
    Bitvector( Bitvector const& other, size_t bits )
    {
        if( bits > other.size() ) {
            bits = other.size();
        }
        size_ = bits;
        auto const ds = (size_ / IntSize) + (size_ % IntSize == 0 ? 0 : 1);
        assert( ds <= other.data_.size() );
        data_ = std::vector<IntType>( other.data_.begin(), other.data_.begin() + ds );
        unset_padding_();
    }

    ~Bitvector() = default;

    Bitvector(Bitvector const&) = default;
    Bitvector(Bitvector&&)      = default;

    Bitvector& operator= (Bitvector const&) = default;
    Bitvector& operator= (Bitvector&&)      = default;

    // ---------------------------------------------------------
    //     Single Bit Functions
    // ---------------------------------------------------------

    /**
     * @brief Return the value of a single bit, without boundary check.
     */
    inline bool operator [] (size_t index) const {
        return static_cast<bool> (data_[index / IntSize] & bit_mask_[index % IntSize]);
    }

    /**
     * @brief Return the value of a single bit, with boundary check.
     */
    inline bool get (size_t index) const
    {
        if (index >= size_) {
            return false;
        }
        return static_cast<bool> (data_[index / IntSize] & bit_mask_[index % IntSize]);
    }

    /**
     * @brief Set the value of a single bit to true, with boundary check.
     */
    inline void set (size_t index)
    {
        if (index >= size_) {
            return;
        }
        data_[index / IntSize] |= bit_mask_[index % IntSize];
    }

    /**
     * @brief Set the value of a single bit to false, with boundary check.
     */
    inline void unset (size_t index)
    {
        if (index >= size_) {
            return;
        }
        data_[index / IntSize] &= ~(bit_mask_[index % IntSize]);
    }

    /**
     * @brief Set the value of a single bit to a given bool value, with boundary check.
     */
    inline void set (size_t index, bool value)
    {
        if (value) {
            set(index);
        } else {
            unset(index);
        }
    }

    /**
     * @brief Flip (negate) the value of a single bit, with boundary check.
     */
    inline void flip (size_t index)
    {
        if (index >= size_) {
            return;
        }
        data_[index / IntSize] ^= bit_mask_[index % IntSize];
    }

    // ---------------------------------------------------------
    //     Operators
    // ---------------------------------------------------------

    Bitvector& operator &= (Bitvector const& rhs);
    Bitvector& operator |= (Bitvector const& rhs);
    Bitvector& operator ^= (Bitvector const& rhs);
    Bitvector  operator ~  () const;

    bool operator == (const Bitvector &other) const;
    bool operator != (const Bitvector &other) const;

    // ---------------------------------------------------------
    //     Other Functions
    // ---------------------------------------------------------

    /**
     * @brief Return the size (number of bits) of this Bitvector.
     */
    inline size_t size() const
    {
        return size_;
    }

    /**
     * @brief Count the number of set bits in the Bitvector, that is, its Hamming weight.
     */
    size_t  count() const;

    /**
     * @brief Return an std::hash value for the Bitvector.
     */
    size_t  hash()  const;

    /**
     * @brief Return a hash value of type IntType that is quicker to calculate than hash().
     *
     * This can be used for obtaining a simple hash using xor of the words.
     * The avalanche effect is of course not present, but for many applications, this hash is
     * good enough and quite useful.
     */
    IntType x_hash() const;

    /**
     * @brief Flip all bits.
     */
    void negate();

    /**
     * @brief Bring the Bitvector in a normalized form, where the first bit is always zero.
     *
     * If the first bit is zero, nothing happens. However, if is is one, the whole Bitvector is flipped
     * using negate().
     */
    void normalize();

    /**
     * @brief Set all the bits to a specified @p value.
     */
    void set_all(const bool value = false);

    std::string dump() const;
    std::string dump_int(IntType x) const;

    // ---------------------------------------------------------
    //     Internal Members
    // ---------------------------------------------------------

private:

    /**
     * @brief Internal function that sets all bits to zero that are not actively used.
     *
     * The data_ buffer always contains a multiple of IntSize many bits, thus there might be surplus
     * bits at its end for padding. In case we do operations with Bitvectors of different size, these
     * might be affected, so we need to reset them to zero sometimes.
     */
    void unset_padding_();

    static const IntType all_0_;
    static const IntType all_1_;

    static const IntType bit_mask_[IntSize];
    static const IntType ones_mask_[IntSize];

    static const IntType count_mask_[4];

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

    size_t               size_ = 0;
    std::vector<IntType> data_;
};

} // namespace utils
} // namespace genesis

// =============================================================================
//     Namespace std extension
// =============================================================================

/*
namespace std {

template<>
struct hash<genesis::utils::Bitvector>
{
    size_t operator() (genesis::utils::Bitvector const &rhs) const
    {
      return rhs.hash();
    }
};

} // namespace std
*/

#endif // include guard
