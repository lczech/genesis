#ifndef GENESIS_UTIL_CONTAINER_BITPACKED_VECTOR_H_
#define GENESIS_UTIL_CONTAINER_BITPACKED_VECTOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2026 Lucas Czech

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
 * @ingroup util
 */

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace util {
namespace container {

// =================================================================================================
//     Bitpacked Vector
// =================================================================================================

/**
 * @brief Vector of unsigned integer values of (almost) arbitrary bit width.
 *
 * This vector wrapper packs unsigned integer values with bit widths between 1 and 64 (inclusive)
 * so that unused bits are not wasted. For instance, when requiring ints up to 127, we only need
 * 7 bits to store those, and can hence start the value of the next entry in the vector at bit 8.
 * For large vectors, this can save a considerable amount of memory.
 *
 * This comes at the cost of having to extract the value from the underlying data first. We optimize
 * so that if the same uint type is used for internal storage as for the actual data, no extraction
 * is necessary. In all other cases, we need to apply some bit magic to get the values.
 * In our tests, this leads to a ~2-3x slowdown compared to a direct access via `operator[]` on the
 * vector when querying consecutive elements (which is still in the hundreds of millions of
 * accesses per second).
 *
 * The class can be parameterized with:
 *
 *   - `U`: The underlying uint type, `uint64_t` by default. Values are packed into a vector of this
 *     type, and extracted from there. Unless another uint type is being used as the external
 *     data type `T`, this can be kept at its default. Alternatively, use another `uint` type.
 *   - `T`: The external uint type that we want to set and get. This cannot have a wider bit width
 *     than `U`, but smaller if needed (although that does not do much, except for helping with
 *     the casting for downstream).
 *
 * As mentioned, in the default case, where `T == U`, and if additionally the bit width exactly
 * matches those types, the underlying vector stores the exact type and width that we want, so then
 * we do not need to extract bits. This can yield a slight speedup.
 */
template<typename U = uint64_t, typename T = U>
class BitpackedVector
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    BitpackedVector() = default;

    /**
     * @brief Construct the vector with a given @p size (number of elements) and the needed
     * @p bit_width for each of these elements.
     */
    BitpackedVector( size_t size, size_t bit_width )
        : size_( size )
        , bit_width_( bit_width )
    {
        // Boundary checks
        if( bit_width_ == 0 || bit_width_ > STORAGE_BITS ) {
            throw std::invalid_argument("Bit width must be between 1 and the storage width.");
        }
        if( bit_width_ > VALUE_BITS ) {
            throw std::invalid_argument("Bit width exceeds the value width of T.");
        }
        if( size_ > (std::numeric_limits<size_t>::max() - (STORAGE_BITS - 1)) / bit_width_ ) {
            throw std::length_error("BitpackedVector size overflow.");
        }

        // Calculate the number of U integers needed to store the required number of elements
        size_t const vec_size = ( bit_width_ * size_ + STORAGE_BITS - 1 ) / STORAGE_BITS;
        data_.resize( vec_size, 0 );

        // Efficiency caches. The mask is only used if the underlying type does _not_
        // have the same bit width as used for storage, i.e., when the bits are not aligned.
        is_bit_aligned_ = std::is_same<T, U>::value && bit_width_ == STORAGE_BITS;
        mask_ = is_bit_aligned_ ? ~U{0} : ((U{1} << bit_width_) - 1);
    }

    ~BitpackedVector() = default;

    BitpackedVector( BitpackedVector const& ) = default;
    BitpackedVector( BitpackedVector&& )      = default;

    BitpackedVector& operator= ( BitpackedVector const& ) = default;
    BitpackedVector& operator= ( BitpackedVector&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    [[nodiscard]] inline T at( size_t index ) const
    {
        if( index >= size_ ) {
            throw std::out_of_range( "BitpackedVector::at(): index out of bounds" );
        }
        return get_unchecked_( index );
    }

    [[nodiscard]] inline T operator[]( size_t index ) const noexcept
    {
        assert( index < size_ );
        return get_unchecked_( index );
    }

    inline void set_at( size_t index, T value )
    {
        if( index >= size_ ) {
            throw std::out_of_range( "BitpackedVector::set_at(): index out of bounds" );
        }
        if( static_cast<U>( value ) > mask_ ) {
            throw std::invalid_argument(
                "Value " + std::to_string( value ) + " out of bounds for " +
                std::to_string( bit_width_ ) + " bit storage"
            );
        }
        set_unchecked_( index, value );
    }

    inline void set_unchecked( size_t index, T value ) noexcept
    {
        assert( index < size_ );
        assert( static_cast<U>( value ) <= mask_ );
        set_unchecked_( index, value );
    }

    [[nodiscard]] size_t size() const noexcept
    {
        return size_;
    }

    [[nodiscard]] size_t bit_width() const noexcept
    {
        return bit_width_;
    }

    [[nodiscard]] std::vector<U> const& data() const noexcept
    {
        return data_;
    }

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    [[nodiscard]] inline T get_unchecked_( size_t index ) const noexcept
    {
        // If the type T is the same as the storage type, return the element directly
        // for efficiency. No need for bit shifting in that case.
        if( is_bit_aligned_ ) {
            return static_cast<T>( data_[index] );
        }

        // Calculate the bit offset and unit index
        size_t const bit_index   = index * bit_width_;
        size_t const data_index  = bit_index / STORAGE_BITS;
        size_t const bit_in_word = bit_index % STORAGE_BITS;

        // Mask to extract the value bits. As this is only executed if the used bit width is smaller
        // than that of the underlying type, the shift never overflows, which we assert.
        U value = ( data_[data_index] >> bit_in_word ) & mask_;

        // Handle values that span across the boundary of two storage units.
        // This can only happen if the indexed entry is not in the last word.
        if( bit_in_word + bit_width_ > STORAGE_BITS ) {
            assert( data_index + 1 < data_.size() );
            size_t const bit_in_next_word = ( bit_in_word + bit_width_ ) - STORAGE_BITS;
            assert( bit_in_next_word < bit_width_ );
            U const next_mask = ( U{1} << bit_in_next_word ) - 1;
            assert( next_mask != 0 );
            value |= ( data_[data_index + 1] & next_mask ) << ( bit_width_ - bit_in_next_word );
        }

        assert( value <= std::numeric_limits<T>::max() );
        return static_cast<T>( value );
    }

    inline void set_unchecked_( size_t index, T value ) noexcept
    {
        U const uvalue = static_cast<U>( value );

        // If the type T is the same as the storage type, set the element directly for efficiency.
        if( is_bit_aligned_ ) {
            data_[index] = uvalue;
            return;
        }

        // Calculate the bit offset and unit index
        size_t const bit_index   = index * bit_width_;
        size_t const data_index  = bit_index / STORAGE_BITS;
        size_t const bit_in_word = bit_index % STORAGE_BITS;

        // Mask to clear and set the value. As before, we only execute this when the bid width
        // is smaller that that of U, so that the shifting does not overflow.
        data_[data_index] &= ~( mask_ << bit_in_word );
        data_[data_index] |= ( uvalue & mask_ ) << bit_in_word;
        assert( mask_ != 0 );
        assert(( static_cast<U>(value) & mask_ ) == static_cast<U>(value) );

        // Handle values that span across the boundary of two storage units.
        // This can only happen if the indexed entry is not in the last word.
        if( bit_in_word + bit_width_ > STORAGE_BITS ) {
            size_t const bit_in_next_word = ( bit_in_word + bit_width_ ) - STORAGE_BITS;
            U const next_mask = ( U{1} << bit_in_next_word ) - 1;
            assert( next_mask != 0 );
            U const shifted_value = uvalue >> ( bit_width_ - bit_in_next_word );

            assert( data_index + 1 < data_.size() );
            data_[data_index + 1] &= ~next_mask;
            data_[data_index + 1] |= shifted_value & next_mask;
        }
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    // Assert correct type usage
    static_assert(std::is_unsigned<T>::value, "T must be an unsigned integral type.");
    static_assert(std::is_unsigned<U>::value, "U must be an unsigned integral type.");
    static_assert( sizeof(T) <= sizeof(U), "sizeof(T) cannot be larger than sizeof(U)" );

    // Number of bits in the underlying type U
    // static constexpr size_t STORAGE_BITS = sizeof(U) * CHAR_BIT;
    static constexpr size_t STORAGE_BITS = std::numeric_limits<U>::digits;
    static constexpr size_t VALUE_BITS   = std::numeric_limits<T>::digits;
    static_assert( CHAR_BIT == 8, "CHAR_BIT != 8" );
    static_assert( STORAGE_BITS == 8 * sizeof(U), "STORAGE_BITS != 8 * sizeof(U)" );
    static_assert( VALUE_BITS == 8 * sizeof(T), "VALUE_BITS != 8 * sizeof(T)" );

    // Data members
    size_t size_ = 0;
    size_t bit_width_ = 0;
    std::vector<U> data_{};

    // Caches
    bool is_bit_aligned_ = false;
    U mask_ = 0;
};

} // namespace container
} // namespace util
} // namespace genesis

#endif // include guard
