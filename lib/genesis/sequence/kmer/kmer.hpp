#ifndef GENESIS_SEQUENCE_KMER_KMER_H_
#define GENESIS_SEQUENCE_KMER_KMER_H_

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
 * @ingroup sequence
 */

#include "genesis/sequence/kmer/alphabet.hpp"
#include "genesis/sequence/kmer/bitfield.hpp"

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Kmer
// =================================================================================================

/**
 * @brief Default Tag for a Kmer, used when no other Tag is provided.
 */
struct KmerTagDefault{};

/**
 * @brief Kmer class template for representing k-mers of various sizes, currently up to k-32.
 *
 * The class is templated with a Tag parameter, which allows distinguishing instances of different
 * k-mer sizes to avoid accidentally mixing them. This also serves as a mechanism to maintain the
 * value of k for each Tag that is being used, so that it does not have to be handed over to each
 * function using the Kmer. The downside of this is that for a given Tag, only one value of k can
 * be used at a time. For normal use cases, this should be fine, as k usually is a fixed value for
 * the duration of a program. Should multiple values be needed, each needs to use their own Tag.
 */
template<typename Tag = KmerTagDefault>
class Kmer
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Alphabet = KmerAlphabet;
    using Bitfield = KmerBitfield;

    // Type checks
    static_assert(
        Alphabet::MAX_RANK <= Bitfield::MAX_RANK, "Alphabet::MAX_RANK > Bitfield::MAX_RANK"
    );

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Kmer() = default;

    // Constructor to initialize k-mer data
    Kmer( typename Bitfield::WordType data )
        : value( data )
    {
        // assert( value & Bitfield::ones_mask[k_] == value );
    }

    ~Kmer() = default;

    Kmer( Kmer const& ) = default;
    Kmer( Kmer&& )      = default;

    Kmer& operator= ( Kmer const& ) = default;
    Kmer& operator= ( Kmer&& )      = default;

    // -------------------------------------------------------------------------
    //     K
    // -------------------------------------------------------------------------

    static uint8_t k()
    {
        return k_;
    }

    /**
     * @brief Set the value of `k` for all Kmer%s of the given Tag.
     *
     * This needs to be called once for a given Tag in order to initialize the value of `k` for the
     * Tag. It can also only be done once, and will throw if used again with a different `k`.
     * When needing to change `k` for a given Tag later on, use reset_k() instead.
     */
    static void set_k( uint8_t k )
    {
        if( k_ != 0 && k != k_ ) {
            throw std::runtime_error(
                "Cannot set_k() for a given Tag multiple times. Use reset_k() instead"
            );
        }
        reset_k( k );
    }

    /**
     * @brief Re-set the value of `k` for all Kmer%s of the given Tag.
     *
     * Conceptually, this is the same as set_k(), but without checking that the value has not been
     * set already. This invalidates all Kmers of the same Tag, and hence shall only be done if
     * no instances of a Kmer with the Tag are being in use any more. Otherwise, any computations
     * using those will produce errors or meaningless results.
     */
    static void reset_k( uint8_t k )
    {
        if( k == 0 || k > Bitfield::MAX_CHARS_PER_KMER ) {
            throw std::invalid_argument(
                "Cannot use k-mer with k==" + std::to_string( k )
            );
        }
        k_ = k;
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Get the rank value of a position in the kmer.
     */
    inline uint8_t operator[] ( uint8_t position ) const
    {
        return Bitfield::extract( value, position );
    }

    /**
     * @brief Get the rank value of a position in the kmer.
     *
     * Same as operator[], but also checks the input and kmer validity.
     */
    inline uint8_t get( uint8_t position ) const
    {
        assert( validate() );
        if( position >= k_ ) {
            throw std::invalid_argument(
                "Invalid position " + std::to_string( position ) +
                " in k-mer of size " + std::to_string( k_ )
            );
        }
        return Bitfield::extract( value, position );
    }

    /**
     * @brief Set the rank value at a position in the kmer.
     *
     * Mostly meant for manual testing operations on the kmer, as we usually can be more efficient
     * when working with kmers extracted from larger sequences by operating directly on the value.
     */
    inline void set( uint8_t position, uint8_t rank )
    {
        assert( validate() );
        if( position >= k_ ) {
            throw std::invalid_argument(
                "Invalid position " + std::to_string( position ) +
                " in k-mer of size " + std::to_string( k_ )
            );
        }
        if( rank > Bitfield::MAX_RANK ) {
            throw std::invalid_argument(
                "Cannot set kmer position to rank " + std::to_string( rank ) +
                " in bit encoding with max rank " + std::to_string( Bitfield::MAX_RANK )
            );
        }
        Bitfield::implant( value, position, rank );
    }

    /**
     * @brief Operator to retrieve the kmer as its underlying int type.
     */
    inline operator typename Bitfield::WordType() const
    {
        return value;
    }

    /**
     * @brief Validate the current kmer by checking some basic properties.
     */
    bool validate( bool throw_if_invalid = false ) const
    {
        bool valid = true;

        // Check k.
        valid &= ( k_ > 0 && k_ <= Bitfield::MAX_CHARS_PER_KMER );

        // Check that only the valid bits for the given k are set.
        valid &= (( value    & Bitfield::ones_mask[k_] ) == value );
        valid &= (( rev_comp & Bitfield::ones_mask[k_] ) == rev_comp );

        // Respond to the result of the check.
        if( ! valid && throw_if_invalid ) {
            throw std::runtime_error( "Invalid kmer" );
        }
        return valid;
    }

    // -------------------------------------------------------------------------
    //     Public Data Members
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Value of the kmer.
     *
     * This is the raw value to operate on. It uses the Bitfield for setting and getting bits,
     * and the Alphabet for encoding characters as bits. It is public, as we often want to operate
     * on this value directly, for efficiency.
     */
    typename Bitfield::WordType value = 0;

    /**
     * @brief Reverse complement (RC) of the kmer.
     *
     * In many cases, we need the RC of the kmer, for instance to get the canonical representation.
     * It is cheaper to store this here and incrementally change it the same way that the kmer value
     * is changed, for instance when extracting kmers from an input sequence. Otherwise, we might
     * have to compute the RC each time from scratch.
     *
     * Note though that not all methods that create a kmer will be able to set this, in which case
     * it is left at value zero. That also happens to be the reverse complement of an all-T kmer,
     * but that's okay - that is one edge case where the reverse_complement() function will
     * unnecessarily compute it again if called, and we can live with that.
     */
    typename Bitfield::WordType rev_comp = 0;

    // size_t location = 0;
    // Bitmask::WordType invalid_chars;

    // -------------------------------------------------------------------------
    //     Private Data Members
    // -------------------------------------------------------------------------

private:

    static uint8_t k_;

};

// Initialize the static variable. Has to be out-of-line...
template<typename Tag>
uint8_t Kmer<Tag>::k_ = 0;

} // namespace sequence
} // namespace genesis

#endif // include guard
