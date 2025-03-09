#ifndef GENESIS_UTILS_MATH_BITVECTOR_H_
#define GENESIS_UTILS_MATH_BITVECTOR_H_

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

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
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

    /**
     * @brief Internally used 64 bit unsigned int type.
     */
    using IntType = uint64_t;

    /**
     * @brief Number of bits per word used for storing data.
     */
    static const size_t IntSize = sizeof(IntType) * CHAR_BIT;
    static_assert( CHAR_BIT == 8, "CHAR_BIT != 8" );

    /**
     * @brief Constant word with no bits sets.
     */
    static const IntType ALL_0;

    /**
     * @brief Constant word with all bits sets.
     */
    static const IntType ALL_1;

    /**
     * @brief Bitmasks that contains a single bit at each of the 64 positions.
     *
     * The first entry BIT_MASKS[0] has the first bit set, etc.
     */
    static const std::array<IntType, IntSize> BIT_MASKS;

    /**
     * @brief Bitmask that contains as many ones as the position in the array tells.
     *
     * The element at position `i` contains `i` many ones, starting from the right.
     *
     *     ONES_MASKS[ 0] -->  0 ones: 0000...0000
     *     ONES_MASKS[ 1] -->  1 one:  0000...0001
     *     ONES_MASKS[ 2] -->  2 ones: 0000...0011
     *     ONES_MASKS[ 3] -->  3 ones: 0000...0111
     *     ...
     *     ONES_MASKS[63] --> 63 ones: 0111...1111
     *
     * This mask is used for unsetting the padding bits in unset_padding_bits().
     */
    static const std::array<IntType, IntSize> ONES_MASKS;

    /**
     * @brief Value to indicate an invalid position in the bitvector.
     */
    static constexpr size_t npos = std::numeric_limits<size_t>::max();

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
    Bitvector( size_t size, bool initial_value = false)
        : size_(size)
    {
        // Edge case.
        if( size == 0 ) {
            return;
        }

        // reserve enough bits, and init them.
        data_.resize( get_vector_size( size ));
        set_all(initial_value);
    }

    /**
     * @brief Constructor that takes a size and a list of values (positions) to be set to true.
     */
    Bitvector( size_t size, std::initializer_list<size_t> index_list )
        : Bitvector(size, false)
    {
        for( size_t e : index_list ) {
            set(e);
        }
    }

    /**
     * @brief Constructor that takes a size and a list of values (positions) to be set to true.
     */
    Bitvector( size_t size, std::vector<size_t> index_list )
        : Bitvector(size, false)
    {
        for( size_t e : index_list ) {
            set(e);
        }
    }

    /**
     * @brief Construct a Bitvector using a range of bools.
     *
     * The given iterator pair @p first to @p last needs to dereference to values
     * that are convertible to `bool`.
     */
    template<class It>
    Bitvector( It first, It last )
        : Bitvector( std::distance( first,last ), false )
    {
        size_t i = 0;
        for( auto it = first; it != last; ++it ) {
            set( i, *it );
            ++i;
        }
    }

    /**
     * @brief Create a Bitvector of a given @p size, and copy the content of another Bitvector.
     *
     * If the @p other bitvector is smaller, the remaining bits are set to `0`.
     * If it is bigger, only the first @p size many bits of it are used.
     */
    Bitvector( size_t size, Bitvector const& other );

    /**
     * @brief Constructor that takes a `std::string` of `0`s and `1`s to build the Bitvector.
     *
     * This is for cases where some fixed Bitvector needs to be constructed (e.g., for testing
     * purposes). The constructor throws if any character in the string is not `0` or `1`.
     */
    Bitvector( std::string const& values );

    // /* *
    //  * @brief Create a Bitvector by copying the first @p max_size of another Bitvector.
    //  *
    //  * If `max_size > other.size()`, all max_size are used.
    //  */
    // Bitvector( Bitvector const& other, size_t max_size );

    ~Bitvector() = default;

    Bitvector(Bitvector const&) = default;
    Bitvector(Bitvector&&)      = default;

    Bitvector& operator= (Bitvector const&) = default;
    Bitvector& operator= (Bitvector&&)      = default;

    // ---------------------------------------------------------
    //     Bit Operations
    // ---------------------------------------------------------

    /**
     * @brief Return the value of a single bit, without boundary check.
     */
    inline bool operator [] ( size_t index ) const {
        assert( index / IntSize < data_.size() );
        assert( index % IntSize < BIT_MASKS.size() );
        return static_cast<bool> (data_[index / IntSize] & BIT_MASKS[index % IntSize]);
    }

    /**
     * @brief Return the value of a single bit, with boundary check.
     */
    inline bool get( size_t index ) const
    {
        if( index >= size_ ) {
            throw std::out_of_range(
                "Cannot access element " + std::to_string(index) + " in Bitvector of size " +
                std::to_string(size())
            );
        }

        assert( index / IntSize < data_.size() );
        assert( index % IntSize < BIT_MASKS.size() );
        return static_cast<bool> (data_[index / IntSize] & BIT_MASKS[index % IntSize]);
    }

    /**
     * @brief Set the value of a single bit to true, with boundary check.
     */
    inline void set( size_t index )
    {
        if (index >= size_) {
            throw std::out_of_range(
                "Cannot access element " + std::to_string(index) + " in Bitvector of size " +
                std::to_string(size())
            );
        }

        assert( index / IntSize < data_.size() );
        assert( index % IntSize < BIT_MASKS.size() );
        data_[index / IntSize] |= BIT_MASKS[index % IntSize];
    }

    /**
     * @brief Set the value of a single bit to a given bool value, with boundary check.
     */
    inline void set( size_t index, bool value )
    {
        if( value ) {
            set( index );
        } else {
            unset( index );
        }
    }

    /**
     * @brief Set the value of a single bit to false, with boundary check.
     */
    inline void unset( size_t index )
    {
        if( index >= size_ ) {
            throw std::out_of_range(
                "Cannot access element " + std::to_string(index) + " in Bitvector of size " +
                std::to_string(size())
            );
        }

        assert( index / IntSize < data_.size() );
        assert( index % IntSize < BIT_MASKS.size() );
        data_[index / IntSize] &= ~(BIT_MASKS[index % IntSize]);
    }

    /**
     * @brief Set the value of a contiguous range of bits in the Bitvector.
     *
     * This function takes the @p first (inclusive) and @p last (past-the-end) indices into
     * the Bitvector (with boundary check), and sets them to the given value (`true` by default).
     *
     * The range @p first to @p last is zero-based, with last being the past-the-end index.
     * Hence, this is the same as
     *
     *     for( size_t i = first; i < last; ++i ) {
     *         bitvector.set( i, value );
     *     }
     *
     * but faster for anything beyond a few bits, as we operate on whole words internally.
     */
    void set_range( size_t first, size_t last, bool value = true );

    /**
     * @brief Set all the bits to a specified @p value.
     */
    void set_all( bool value );

    /**
     * @brief Flip (negate) the value of a single bit, with boundary check.
     */
    inline void flip( size_t index )
    {
        if( index >= size_ ) {
            throw std::out_of_range(
                "Cannot access element " + std::to_string(index) + " in Bitvector of size " +
                std::to_string(size())
            );
        }

        assert( index / IntSize < data_.size() );
        assert( index % IntSize < BIT_MASKS.size() );
        data_[index / IntSize] ^= BIT_MASKS[index % IntSize];
    }

    /**
     * @brief Alias for flip(), see there for details.
     */
    inline void toggle( size_t index )
    {
        return flip( index );
    }

    /**
     * @brief Flip all bits. Alias for invert().
     */
    void negate();

    /**
     * @brief Flip all bits. Alias for negate().
     */
    void invert();

    // ---------------------------------------------------------
    //     Other Functions
    // ---------------------------------------------------------

    /**
     * @brief Return whether the Bitvector is empty, that is, has size() == 0.
     *
     * Note that this function does _not_ count the number of bits that are set to `true`.
     * It simply returns whether the Bitvector has any size (`false`),
     * or was default constructed (`true`).
     */
    inline bool empty() const
    {
        return size_ == 0;
    }

    /**
     * @brief Return the size (number of bits) of this Bitvector.
     */
    inline size_t size() const
    {
        return size_;
    }

    /**
     * @brief Get the underlying data, i.e., the vector of uints, in which the data is stored.
     *
     * Note that the last bits of the last word might be padding that does not actually belong
     * to the bitvector data. They shall be 0 at all times.
     */
    std::vector<IntType> const& data() const
    {
        return data_;
    }

    /**
     * @brief Get a non-const reference to the underlying data, i.e., the vector of units.
     *
     * This is meant to allow external functions such as serialization/deserialization to access
     * the underlying data. It is hence important that any function uses this data with care,
     * and does not break the assumptions and invariants expected in the bitvector. In particular,
     * the size of the data returned here needs to match the number of bits of the bitvector,
     * plus the needed padding to get to a full word size, and any padding bits shall be 0.
     *
     * We do not allow for resizing an existing instance. Similarly, we hence recommend to not
     * change the size of the data from the outside, and always call unset_padding_bits() should
     * any operation on the data cause the padding to be non-zero (such as inverting words).
     */
    std::vector<IntType>& data()
    {
        return data_;
    }

    /**
     * @brief Unset all bits to zero that are not actively used at the end of the data vector.
     *
     * The data buffer always contains a multiple of IntSize many bits, thus there might be surplus
     * bits at its end for padding. In case we do operations with Bitvectors of different size, or
     * when negating bits, these might be affected, so we need to reset them to zero in these cases.
     *
     * This function, similar to the non-const data() function, is meant for external operations
     * that need access to internal data. This unset function is idempotent, and it is fine to call
     * it on any bitvector.
     */
    void unset_padding_bits();

    /**
     * @brief Get the mask used for unset_padding_bits()
     *
     * This can be useful when checking certain properties, such as if all bits are set.
     * If the Bitvector has a size that exactly matches the underlying int type (64 bits typically),
     * the mask is all-zero! It shall not be applied in that case.
     */
    IntType get_padding_mask() const;

    /**
     * @brief For a given numer of bits, compute the size of the internally used vector.
     *
     * This is mostly meant as a helper for data operations such as serialization and deserialization.
     */
    static size_t get_vector_size( size_t bit_size )
    {
        return (bit_size / IntSize) + (bit_size % IntSize == 0 ? 0 : 1);
    }

    // ---------------------------------------------------------
    //     Operators
    // ---------------------------------------------------------

    Bitvector& operator &= (Bitvector const& rhs);
    Bitvector& operator |= (Bitvector const& rhs);
    Bitvector& operator ^= (Bitvector const& rhs);
    Bitvector  operator ~  () const;

    friend Bitvector operator & (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator | (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator ^ (Bitvector const& lhs, Bitvector const& rhs);

    bool operator == (const Bitvector &other) const;
    bool operator != (const Bitvector &other) const;

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

    size_t size_ = 0;
    std::vector<IntType> data_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
