#ifndef GENESIS_UTILS_MATH_BITVECTOR_H_
#define GENESIS_UTILS_MATH_BITVECTOR_H_

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

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Bitvector;

Bitvector operator & (Bitvector const& lhs, Bitvector const& rhs);
Bitvector operator | (Bitvector const& lhs, Bitvector const& rhs);
Bitvector operator ^ (Bitvector const& lhs, Bitvector const& rhs);
Bitvector operator - (Bitvector const& lhs, Bitvector const& rhs);
std::ostream& operator << (std::ostream& out, Bitvector const& rhs);

// =================================================================================================
//     Bitvector
// =================================================================================================

class Bitvector
{
public:

    // ---------------------------------------------------------
    //     Declarations and Class Functions
    // ---------------------------------------------------------

    typedef uint64_t    IntType;
    static const size_t IntSize = sizeof(IntType) * 8;

    friend Bitvector operator & (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator | (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator ^ (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator - (Bitvector const& lhs, Bitvector const& rhs);

    friend std::ostream& operator << (std::ostream& out, Bitvector const& rhs);

    /**
     * @brief Default constructor. Creates an empty Bitvector of size 0.
     */
    Bitvector() = default;

    /**
     * @brief Constructor that takes a size and an optional bool value to initialize the Bitvector,
     * false by default.
     */
    Bitvector (const size_t size, const bool init = false) : size_(size)
    {
        // reserve enough bits, and init them.
        data_.resize( (size / IntSize) + (size % IntSize == 0 ? 0 : 1) );
        reset(init);
    }

    /**
     * @brief Constructor that takes a size and a list of values (positions) to be set to true.
     */
    Bitvector (const size_t size, const std::initializer_list<int> list) : Bitvector(size, false)
    {
        for (int e : list) {
            set(e);
        }
    }

    /**
     * @brief Returns the size (number of total bits) of this Bitvector.
     */
    inline size_t size() const
    {
        return size_;
    }

    // ---------------------------------------------------------
    //     Single Bit Functions
    // ---------------------------------------------------------

    /**
     * @brief Returns the value of a single bit, without boundary check.
     */
    inline bool operator [] (size_t index) const {
        return static_cast<bool> (data_[index / IntSize] & bit_mask_[index % IntSize]);
    }

    /**
     * @brief Returns the value of a single bit, with boundary check.
     */
    inline bool get (size_t index) const
    {
        if (index >= size_) {
            return false;
        }
        return static_cast<bool> (data_[index / IntSize] & bit_mask_[index % IntSize]);
    }

    /**
     * @brief Sets the value of a single bit to true, with boundary check.
     */
    inline void set (size_t index)
    {
        if (index >= size_) {
            return;
        }
        data_[index / IntSize] |= bit_mask_[index % IntSize];
    }

    /**
     * @brief Sets the value of a single bit to false, with boundary check.
     */
    inline void unset (size_t index)
    {
        if (index >= size_) {
            return;
        }
        data_[index / IntSize] &= ~(bit_mask_[index % IntSize]);
    }

    /**
     * @brief Sets the value of a single bit to a given bool value, with boundary check.
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
     * @brief Flips (inverts) the value of a single bit, with boundary check.
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
    bool operator != (const Bitvector &other) const
    {
        return !(*this == other);
    }

    /**
     * @brief Strict subset.
     */
    inline bool operator <  (Bitvector const& rhs) const
    {
        return ((*this & rhs) == *this) && (count() < rhs.count());
    }

    /**
     * @brief Strict superset.
     */
    inline bool operator >  (Bitvector const& rhs) const
    {
        return rhs < *this;
    }

    /**
     * @brief Subset or equal.
     */
    inline bool operator <= (Bitvector const& rhs) const
    {
        return (*this == rhs) || (*this < rhs);
    }

    /**
     * @brief Superset or equal.
     */
    inline bool operator >= (Bitvector const& rhs) const
    {
        return (*this == rhs) || (*this > rhs);
    }

    // ---------------------------------------------------------
    //     Other Functions
    // ---------------------------------------------------------

           Bitvector symmetric_difference (                      Bitvector const& rhs) const;
    static Bitvector symmetric_difference (Bitvector const& lhs, Bitvector const& rhs);

    size_t  count() const;
    size_t  hash()  const;
    IntType x_hash() const;

    void    invert();
    void    normalize();
    void    reset(const bool value = false);

    std::string dump() const;
    std::string dump_int(IntType x) const;

    // ---------------------------------------------------------
    //     Internal Members
    // ---------------------------------------------------------

protected:

    void unset_padding();

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
