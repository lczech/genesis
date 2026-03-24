#ifndef GENESIS_UTIL_CONTAINER_BITPACKED_PAIR_VECTOR_H_
#define GENESIS_UTIL_CONTAINER_BITPACKED_PAIR_VECTOR_H_

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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "genesis/util/container/bitpacked_vector.hpp"

namespace genesis {
namespace util {
namespace container {

// =================================================================================================
//     Bitpacked Pair Vector
// =================================================================================================

/**
 * @brief Vector of pairs of unsigned integer values, packed into one bit-packed scalar record.
 *
 * This class stores two unsigned integer fields per entry, each with its own runtime bit width.
 * Both fields are packed into a single unsigned storage value `U`, and those packed records are
 * then stored in a `BitpackedVector<U, U>`.
 *
 * This design is intended for the case where both values are usually accessed together. It is
 * typically faster and simpler than maintaining two separate bit-packed vectors in that case.
 *
 * Requirements:
 *   - `A`, `B`, and `U` must be unsigned integral types.
 *   - `sizeof(A) <= sizeof(U)` and `sizeof(B) <= sizeof(U)`.
 *   - `width_a + width_b <= digits(U)`.
 *   - `width_a <= digits(A)` and `width_b <= digits(B)`.
 */
template<typename U = std::uint64_t, typename A = std::uint32_t, typename B = std::uint32_t>
class BitpackedPairVector
{
public:

    using storage_type = U;
    using first_type   = A;
    using second_type  = B;
    using value_type   = std::pair<A, B>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    BitpackedPairVector() = default;

    BitpackedPairVector( size_t size, size_t width_a, size_t width_b )
        : size_( size )
        , width_a_( width_a )
        , width_b_( width_b )
        , total_width_( width_a + width_b )
        , mask_a_( 0 )
        , mask_b_( 0 )
        , storage_()
    {
        if( width_a_ == 0 || width_a_ > FIRST_BITS ) {
            throw std::invalid_argument(
                "First field width must be between 1 and the bit width of the first type."
            );
        }
        if( width_b_ == 0 || width_b_ > SECOND_BITS ) {
            throw std::invalid_argument(
                "Second field width must be between 1 and the bit width of the second type."
            );
        }
        if( total_width_ == 0 || total_width_ > STORAGE_BITS ) {
            throw std::invalid_argument(
                "Combined bit width must be between 1 and the bit width of the storage type."
            );
        }

        mask_a_ = full_mask_( width_a_ );
        mask_b_ = full_mask_( width_b_ );
        storage_ = BitpackedVector<U, U>( size_, total_width_ );
    }

    ~BitpackedPairVector() = default;

    BitpackedPairVector( BitpackedPairVector const& ) = default;
    BitpackedPairVector( BitpackedPairVector&& )      = default;

    BitpackedPairVector& operator=( BitpackedPairVector const& ) = default;
    BitpackedPairVector& operator=( BitpackedPairVector&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    [[nodiscard]] inline value_type at( size_t index ) const
    {
        return unpack_( storage_.at( index ));
    }

    [[nodiscard]] inline value_type operator[]( size_t index ) const noexcept
    {
        assert( index < size_ );
        return unpack_( storage_[index] );
    }

    [[nodiscard]] inline A first_at( size_t index ) const
    {
        return unpack_first_( storage_.at( index ));
    }

    [[nodiscard]] inline B second_at( size_t index ) const
    {
        return unpack_second_( storage_.at( index ));
    }

    [[nodiscard]] inline A first_unchecked( size_t index ) const noexcept
    {
        assert( index < size_ );
        return unpack_first_( storage_[index] );
    }

    [[nodiscard]] inline B second_unchecked( size_t index ) const noexcept
    {
        assert( index < size_ );
        return unpack_second_( storage_[index] );
    }

    inline void set_at( size_t index, A first, B second )
    {
        if( index >= size_ ) {
            throw std::out_of_range( "BitpackedPairVector::set_at(): index out of bounds" );
        }

        U const ua = static_cast<U>( first );
        U const ub = static_cast<U>( second );

        if( ua > mask_a_ ) {
            throw std::invalid_argument(
                "First value " + std::to_string( first ) + " out of bounds for " +
                std::to_string( width_a_ ) + " bit storage"
            );
        }
        if( ub > mask_b_ ) {
            throw std::invalid_argument(
                "Second value " + std::to_string( second ) + " out of bounds for " +
                std::to_string( width_b_ ) + " bit storage"
            );
        }

        storage_.set_unchecked( index, pack_( ua, ub ));
    }

    inline void set_unchecked( size_t index, A first, B second ) noexcept
    {
        assert( index < size_ );

        U const ua = static_cast<U>( first );
        U const ub = static_cast<U>( second );

        assert( ua <= mask_a_ );
        assert( ub <= mask_b_ );

        storage_.set_unchecked( index, pack_( ua, ub ));
    }

    [[nodiscard]] inline U packed_at( size_t index ) const
    {
        return storage_.at( index );
    }

    [[nodiscard]] inline U packed_unchecked( size_t index ) const noexcept
    {
        assert( index < size_ );
        return storage_[index];
    }

    inline void set_packed_at( size_t index, U packed )
    {
        if( index >= size_ ) {
            throw std::out_of_range( "BitpackedPairVector::set_packed_at(): index out of bounds" );
        }
        if( total_width_ < STORAGE_BITS && packed > full_mask_( total_width_ )) {
            throw std::invalid_argument(
                "Packed value out of bounds for combined bit width of " +
                std::to_string( total_width_ )
            );
        }
        storage_.set_unchecked( index, packed );
    }

    inline void set_packed_unchecked( size_t index, U packed ) noexcept
    {
        assert( index < size_ );
        assert( total_width_ == STORAGE_BITS || packed <= full_mask_( total_width_ ));
        storage_.set_unchecked( index, packed );
    }

    [[nodiscard]] inline size_t size() const noexcept
    {
        return size_;
    }

    [[nodiscard]] inline size_t first_bit_width() const noexcept
    {
        return width_a_;
    }

    [[nodiscard]] inline size_t second_bit_width() const noexcept
    {
        return width_b_;
    }

    [[nodiscard]] inline size_t total_bit_width() const noexcept
    {
        return total_width_;
    }

    [[nodiscard]] inline BitpackedVector<U, U> const& storage() const noexcept
    {
        return storage_;
    }

    // -------------------------------------------------------------------------
    //     Private Helpers
    // -------------------------------------------------------------------------

private:

    static_assert( std::is_unsigned<A>::value, "A must be an unsigned integral type." );
    static_assert( std::is_unsigned<B>::value, "B must be an unsigned integral type." );
    static_assert( std::is_unsigned<U>::value, "U must be an unsigned integral type." );
    static_assert( sizeof(A) <= sizeof(U), "sizeof(A) cannot be larger than sizeof(U)" );
    static_assert( sizeof(B) <= sizeof(U), "sizeof(B) cannot be larger than sizeof(U)" );

    static constexpr size_t FIRST_BITS   = std::numeric_limits<A>::digits;
    static constexpr size_t SECOND_BITS  = std::numeric_limits<B>::digits;
    static constexpr size_t STORAGE_BITS = std::numeric_limits<U>::digits;

    [[nodiscard]] static inline U full_mask_( size_t bits ) noexcept
    {
        assert( bits > 0 );
        assert( bits <= STORAGE_BITS );
        return bits == STORAGE_BITS ? ~U{0} : (( U{1} << bits ) - 1 );
    }

    [[nodiscard]] inline U pack_( U first, U second ) const noexcept
    {
        assert( first <= mask_a_ );
        assert( second <= mask_b_ );

        // width_a_ is always < STORAGE_BITS because width_b_ > 0 and
        // width_a_ + width_b_ <= STORAGE_BITS.
        return ( first & mask_a_ ) | (( second & mask_b_ ) << width_a_ );
    }

    [[nodiscard]] inline value_type unpack_( U packed ) const noexcept
    {
        return value_type{
            static_cast<A>( packed & mask_a_ ),
            static_cast<B>(( packed >> width_a_ ) & mask_b_ )
        };
    }

    [[nodiscard]] inline A unpack_first_( U packed ) const noexcept
    {
        return static_cast<A>( packed & mask_a_ );
    }

    [[nodiscard]] inline B unpack_second_( U packed ) const noexcept
    {
        return static_cast<B>(( packed >> width_a_ ) & mask_b_ );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

    size_t size_        = 0;
    size_t width_a_     = 0;
    size_t width_b_     = 0;
    size_t total_width_ = 0;

    U mask_a_ = 0;
    U mask_b_ = 0;

    BitpackedVector<U, U> storage_{};
};

} // namespace container
} // namespace util
} // namespace genesis

#endif // GENESIS_UTIL_CONTAINER_BITPACKED_PAIR_VECTOR_H_
