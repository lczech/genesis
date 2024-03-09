#ifndef GENESIS_UTILS_MATH_BITVECTOR_H_
#define GENESIS_UTILS_MATH_BITVECTOR_H_

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <array>
#include <cassert>
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

    using IntType = uint64_t;
    static const size_t IntSize = sizeof(IntType) * 8;

    /**
     * @brief Value to indicate that find_next_set() did not find any set bits.
     */
    static const size_t npos = std::numeric_limits<size_t>::max();

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
        data_.resize( (size / IntSize) + (size % IntSize == 0 ? 0 : 1) );
        set_all(initial_value);
    }

    /**
     * @brief Constructor that takes a size and a list of values (positions) to be set to true.
     */
    Bitvector( size_t size, std::initializer_list<size_t> list )
        : Bitvector(size, false)
    {
        for( size_t e : list ) {
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
    //     Single Bit Functions
    // ---------------------------------------------------------

    /**
     * @brief Return the value of a single bit, without boundary check.
     */
    inline bool operator [] ( size_t index ) const {
        assert( index / IntSize < data_.size() );
        assert( index % IntSize < bit_mask_.size() );
        return static_cast<bool> (data_[index / IntSize] & bit_mask_[index % IntSize]);
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
        assert( index % IntSize < bit_mask_.size() );
        return static_cast<bool> (data_[index / IntSize] & bit_mask_[index % IntSize]);
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
        assert( index % IntSize < bit_mask_.size() );
        data_[index / IntSize] |= bit_mask_[index % IntSize];
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
        assert( index % IntSize < bit_mask_.size() );
        data_[index / IntSize] &= ~(bit_mask_[index % IntSize]);
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
    void set( size_t first, size_t last, bool value = true );

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
        assert( index % IntSize < bit_mask_.size() );
        data_[index / IntSize] ^= bit_mask_[index % IntSize];
    }

    /**
     * @brief Alias for flip(), see there for details.
     */
    inline void toggle( size_t index )
    {
        return flip( index );
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
     * @brief Count the number of set bits in the Bitvector, that is, its Hamming weight,
     * or population count (popcnt).
     */
    size_t count() const;

    /**
     * @brief Count the number of set bits between a range of indices in the Bitvector,
     * that is, its Hamming weight, or population count (popcnt), for that range.
     *
     * The range @p first to @p last is zero-based, with last being the past-the-end index.
     * Hence, this is the same as
     *
     *     size_t count = 0;
     *     for( size_t i = first; i < last; ++i ) {
     *         if( bitvector.get( i )) {
     *             ++count;
     *         }
     *     }
     *
     * but faster, as we use whole-word counting internally.
     */
    size_t count( size_t first, size_t last ) const;

    /**
     * @brief Return if any bits are set at all.
     */
    bool any_set() const;

    /**
     * @brief Return the index of the next position in the Bitvector that is set.
     *
     * This returns the first position after @p start, including @p start itself, that is set.
     * If no such position exists (because all following bits are `false`), or if @p start is beyond
     * the length of the vector, Bitvector::npos is returned instead.
     */
    size_t find_next_set( size_t start ) const;

    /**
     * @brief Return an std::hash value for the Bitvector.
     */
    size_t hash()  const;

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

    std::vector<IntType> const& data() const
    {
        return data_;
    }

    // ---------------------------------------------------------
    //     Internal Members
    // ---------------------------------------------------------

private:

    /**
     * @brief Internal function that sets all bits to zero that are not actively used.
     *
     * The data_ buffer always contains a multiple of IntSize many bits, thus there might be surplus
     * bits at its end for padding. In case we do operations with Bitvectors of different size, or
     * when negating bits, these might be affected, so we need to reset them to zero in these cases.
     */
    void unset_padding_();

    /**
     * @brief Internal helper to count the number of bits set in a word.
     */
    static size_t count_( IntType x );

    static const IntType all_0_;
    static const IntType all_1_;

    /**
     * @brief Bitmask that contains a single bit at each of the 64 positions.
     */
    static const std::array<IntType, IntSize> bit_mask_;

    /**
     * @brief Bitmask that contains as many ones as the position in the array tells.
     *
     * The element at position `i` contains `i` many ones, starting from the right.
     *
     *     ones_mask_[ 0] -->  0 ones: 0000...0000
     *     ones_mask_[ 1] -->  1 one:  0000...0001
     *     ones_mask_[ 2] -->  2 ones: 0000...0011
     *     ones_mask_[ 3] -->  3 ones: 0000...0111
     *     ...
     *     ones_mask_[63] --> 63 ones: 0111...1111
     *
     * This mask is used for unsetting the padding bits in unset_padding_().
     */
    static const std::array<IntType, IntSize> ones_mask_;

    /**
     * @brief Special masks used for quickly counting the number of set bits.
     *
     * See count_() for our usage, and see https://en.wikipedia.org/wiki/Hamming_weight for
     * the underlying algorithm.
     */
    static const std::array<IntType, 4> count_mask_;

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

    size_t               size_ = 0;
    std::vector<IntType> data_;
};

// =============================================================================
//     Hashing
// =============================================================================

/**
 * @brief Helper structure that yields the hash of a given Bitvector.
 *
 * It is meant to be used in containers such as `unordered_set` or `unordered_map`
 * that can make use of custom hash functions for the key objects. By default,
 * these containers use a specialization of the `std::hash` template, which we also offer,
 * and that also uses the Bitvector::hash() function.
 *
 * Hence, this class here is slightly redundant, as it gives the same result as just using
 * the `std::hash` specialization. Still, it might be useful to have.
 *
 * See also BitvectorXhash for an alternative version that uses Bitvector::x_hash() instead.
 */
struct BitvectorHash
{
    std::size_t operator() ( genesis::utils::Bitvector const& value ) const
    {
        return value.hash();
    }
};

/**
 * @brief Helper structer that yields the x_hash of a given Bitvector.
 *
 * It is meant to be used in containers such as `unordered_set` or `unordered_map`
 * that can make use of custom hash functions for the key objects. Using this class instead
 * of the standard `std::hash` specialization, the Bitvector::x_hash() function is used instead
 * of the standard hash() function. It is hence faster to compute, but without avalanche effect.
 *
 * In some use cases, this might be preferrable - we however recommend to test this, in order to
 * make sure that colliding hashes do not slow down the performance in the end.
 *
 * Note that the function needs to cast from Bitvector::IntType to std::size_t.
 * On most modern systems, these are expecte to be the same, i.e., 64 bit unsigned integers.
 * However, this might cause problem on systems where this is not the case.
 */
struct BitvectorXhash
{
    std::size_t operator() ( genesis::utils::Bitvector const& value ) const
    {
        return static_cast<std::size_t>( value.x_hash() );
    }
};

} // namespace utils
} // namespace genesis

// =============================================================================
//     Namespace std extension
// =============================================================================

namespace std {

/**
 * @brief Specialization of std::hash for the Bitvector class.
 *
 * It uses Bitvector::hash() for the hashing. See also BitvectorHash for an alternative class
 * that does the same, but resides in the same namespace as Bitvector, and see BitvectorXhash
 * for a variant that uses Bitvector::x_hash() instead as the hashing function.
 */
template<>
struct hash<genesis::utils::Bitvector>
{
    std::size_t operator() ( genesis::utils::Bitvector const& value ) const
    {
        return value.hash();
    }
};

} // namespace std

#endif // include guard
