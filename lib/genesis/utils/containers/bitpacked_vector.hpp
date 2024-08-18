#ifndef GENESIS_UTILS_CONTAINERS_BITPACKED_VECTOR_H_
#define GENESIS_UTILS_CONTAINERS_BITPACKED_VECTOR_H_

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

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace utils {

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
 *     data type `T`, this can be kept at its default.
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
        if( bit_width_ == 0 || bit_width_ > STORAGE_BITS ) {
            throw std::invalid_argument(
                "Bit width must be between 1 and the bit width of the storage type."
            );
        }

        // Calculate the number of U integers needed to store the required number of elements
        size_t const vec_size = ( bit_width_ * size_ + STORAGE_BITS - 1 ) / STORAGE_BITS;
        data_.resize( vec_size, 0 );
    }

    ~BitpackedVector() = default;

    BitpackedVector( BitpackedVector const& ) = default;
    BitpackedVector( BitpackedVector&& )      = default;

    BitpackedVector& operator= ( BitpackedVector const& ) = default;
    BitpackedVector& operator= ( BitpackedVector&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Retrieve an element from the vector.
     */
    inline T get( size_t index ) const
    {
        if( index >= size_ ) {
            throw std::out_of_range( "Index out of bounds" );
        }

        // If the type T is the same as the storage type, return the element directly for efficiency.
        // No need for bit shifting in that case.
        if( std::is_same<T, U>::value && bit_width_ == STORAGE_BITS ) {
            return data_[index];
        }

        // Calculate the bit offset and unit index
        size_t const bit_index   = index * bit_width_;
        size_t const data_index  = bit_index / STORAGE_BITS;
        size_t const bit_in_word = bit_index % STORAGE_BITS;

        // Mask to extract the value bits. As this is only executed if the used bit width is smaller
        // than that of the underlying type, the shift never overflows, which we assert.
        U const mask  = ( U{1} << bit_width_ ) - 1;
        U value = ( data_[data_index] >> bit_in_word ) & mask;
        assert( mask != 0 );

        // Handle values that span across the boundary of two storage units
        if( bit_in_word + bit_width_ > STORAGE_BITS ) {
            size_t const bit_in_next_word = ( bit_in_word + bit_width_ ) - STORAGE_BITS;
            assert( bit_in_next_word < bit_width_ );
            U const next_mask = (( U{1} << bit_in_next_word ) - 1 );
            assert( next_mask != 0 );
            value |= ( data_[data_index + 1] & next_mask ) << ( bit_width_ - bit_in_next_word );
        }

        assert( value <= std::numeric_limits<T>::max() );
        return static_cast<T>(value);
    }

    /**
     * @brief Set an element in the vector.
     */
    inline void set( size_t index, T value )
    {
        if( index >= size_ ) {
            throw std::out_of_range( "Index out of bounds" );
        }

        // If the type T is the same as the storage type, set the element directly for efficiency.
        if( std::is_same<T, U>::value && bit_width_ == STORAGE_BITS ) {
            data_[index] = static_cast<U>(value);
            return;
        }

        // Check that the value fits into the bit width. We do this here, where we know
        // that we are not using the full width of U, so that the bit shift never overflows.
        if( value >= ( U{1} << bit_width_ )) {
            throw std::invalid_argument(
                "Value " + std::to_string( value ) + " out of bounds for " +
                std::to_string( STORAGE_BITS ) + " bit storage"
            );
        }

        // Calculate the bit offset and unit index
        size_t const bit_index   = index * bit_width_;
        size_t const data_index  = bit_index / STORAGE_BITS;
        size_t const bit_in_word = bit_index % STORAGE_BITS;

        // Mask to clear and set the value. As before, we only execute this when the bid width
        // is smaller that that of U, so that the shifting does not overflow.
        U const mask = ( U{1} << bit_width_ ) - 1;
        data_[data_index] &= ~( mask << bit_in_word );
        data_[data_index] |= ( static_cast<U>(value) & mask ) << bit_in_word;
        assert( mask != 0 );
        assert(( static_cast<U>(value) & mask ) == static_cast<U>(value) );

        // Handle values that span across the boundary of two storage units
        if( bit_in_word + bit_width_ > STORAGE_BITS ) {
            size_t const bit_in_next_word = (bit_in_word + bit_width_) - STORAGE_BITS;
            U const next_mask = (( U{1} << bit_in_next_word ) - 1 );
            assert( next_mask != 0 );
            U const shifted_value = (static_cast<U>(value) >> (bit_width_ - bit_in_next_word));

            data_[data_index + 1] &= ~(( U{1} << bit_in_next_word ) - 1 );
            data_[data_index + 1] |= shifted_value & next_mask;
        }
    }

    size_t size() const
    {
        return size_;
    }

    size_t bit_width() const
    {
        return bit_width_;
    }

    std::vector<U> const& data() const
    {
        return data_;
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
    static constexpr size_t STORAGE_BITS = sizeof(U) * CHAR_BIT;
    static_assert( CHAR_BIT == 8, "CHAR_BIT != 8" );

    // Data members
    size_t size_;
    size_t bit_width_;
    std::vector<U> data_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
