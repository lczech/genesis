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
 * @brief Kmer struct for representing k-mers of various sizes, currently up to k-32.
 *
 * Note that we do store the value of `k` here for simplicity, as well as other properties such
 * as the reverse complement, location in the genome, etc. These are helpful in many situations,
 * but is of course wasteful when we just want the value of the k-mer itself.
 * Hence, when larger collections of Kmer instances need to be stored, it might make sense
 * to just store the numerical value instead, and discard the extra information kept here.
 *
 * Most operations on the Kmer are actually done byt the KmerBitfield and KmerAlphabet classes.
 * This struct here is mostly meant as an (almost) POD to store the values of a k-mer.
 */
struct Kmer
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Alphabet = KmerAlphabet;
    using Bitfield = KmerBitfield;

    using WordType = typename Bitfield::WordType;

    // Type checks
    static_assert(
        Alphabet::MAX_RANK <= Bitfield::MAX_RANK, "Alphabet::MAX_RANK > Bitfield::MAX_RANK"
    );

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Kmer() = default;

    /**
     * @brief Constructor to initialize k-mer data
     */
    explicit Kmer( uint8_t k, WordType data )
        : value( data )
        , k_( k )
    {
        // assert( value & Bitfield::ones_mask[k_] == value );
        if( k == 0 || k > Bitfield::MAX_CHARS_PER_KMER ) {
            throw std::invalid_argument(
                "Cannot construct kmers with k=" + std::to_string( k )
            );
        }
    }

    ~Kmer() = default;

    Kmer( Kmer const& ) = default;
    Kmer( Kmer&& )      = default;

    Kmer& operator= ( Kmer const& ) = default;
    Kmer& operator= ( Kmer&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    uint8_t k() const
    {
        return k_;
    }

    /**
     * @brief Get the rank value of a position in the kmer.
     */
    inline uint8_t operator[] ( uint8_t position ) const
    {
        return Bitfield::extract( k_, value, position );
    }

    /**
     * @brief Get the rank value of a position in the kmer.
     *
     * Same as operator[], but also checks the input and kmer validity.
     */
    inline uint8_t get( uint8_t position ) const
    {
        if( position >= k_ ) {
            throw std::invalid_argument(
                "Invalid position " + std::to_string( position ) +
                " in k-mer of size " + std::to_string( k_ )
            );
        }
        return Bitfield::extract( k_, value, position );
    }

    /**
     * @brief Set the rank value at a position in the kmer.
     *
     * Mostly meant for manual testing operations on the kmer, as we usually can be more efficient
     * when working with kmers extracted from larger sequences by operating directly on the value.
     */
    inline void set( uint8_t position, uint8_t rank )
    {
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
        Bitfield::implant( k_, value, position, rank );
    }

    /**
     * @brief Operator to retrieve the kmer as its underlying int type.
     */
    inline operator typename Bitfield::WordType() const
    {
        return value;
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
    WordType value = 0;

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
    WordType rev_comp = 0;

    /**
     * @brief Location of the kmer in the original input sequence.
     *
     * This is the position (0-based) of the first character of the kmer in the sequence.
     * Might not be set by all methods. Defaults to max() to indicate that it is not valid.
     */
    size_t location = std::numeric_limits<size_t>::max();

    // Bitmask::WordType invalid_chars;

    // -------------------------------------------------------------------------
    //     Private Data Members
    // -------------------------------------------------------------------------

private:

    uint8_t k_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
