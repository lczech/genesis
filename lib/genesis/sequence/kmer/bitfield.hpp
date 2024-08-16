#ifndef GENESIS_SEQUENCE_KMER_BITFIELD_H_
#define GENESIS_SEQUENCE_KMER_BITFIELD_H_

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

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Bitfield
// =================================================================================================

/**
 * @brief Class to encapsulate the bit information needed to work with Kmers.
 *
 * This is mainly meant to keep the bitmasks etc all in one place, so that it's easier in the future
 * if we need to expand from 2-bit encoding to some other bit masks (e.g., for protein sequences).
 */
struct KmerBitfield
{
    // -------------------------------------------------------------------------
    //     Constants and Masks
    // -------------------------------------------------------------------------

    /**
     * @brief Underlying integer type used to store the k-mer.
     */
    using WordType = uint64_t;

    /**
     * @brief Number of bits in the underlying integer type used to store the k-mer.
     */
    static const uint8_t BIT_WIDTH = sizeof(WordType) * CHAR_BIT;
    static_assert( CHAR_BIT == 8, "CHAR_BIT != 8" );

    /**
     * @brief Number of bits needed to store a character of input data.
     *
     * We currently only offer k-mers for DNA, where we use two bits per nucleotide,
     * using `A == 0b00`, `C == 0b01`, `G == 0b10`, `T == 0b11`. This is the main part
     * that alternative versions of this class would need to change, along with the masks.
     */
    static const uint8_t BITS_PER_CHAR = 2;

    /**
     * @brief Highest rank (bit representation) for a value that can be packed into a word.
     */
    static const uint8_t MAX_RANK = ( static_cast<WordType>( 1 ) << BITS_PER_CHAR ) - 1;
    static_assert( MAX_RANK == 3, "MAX_RANK != 3" );

    /**
     * @brief Maximum number of characters (values) that can be packed into a word.
     */
    static const uint8_t MAX_CHARS_PER_KMER = BIT_WIDTH / BITS_PER_CHAR;
    static_assert( BIT_WIDTH % BITS_PER_CHAR == 0, "BIT_WIDTH % BITS_PER_CHAR != 0" );

    static constexpr WordType ALL_0 = 0ul;
    static constexpr WordType ALL_1 = (((1ul << 32) - 1) << 32)  +  ((1ul << 32) - 1);
    static_assert( ALL_1 == std::numeric_limits<WordType>::max(), "KmerBitfield::ALL_1 != max()" );

    /**
     * @brief Bitmask that has the bits set for each char position in the word.
     *
     * The values are
     *
     *     char_mask[0]  == 00 00 ... 00 11
     *     char_mask[1]  == 00 00 ... 11 00
     *     ...
     *     char_mask[31] == 11 00 ... 00 00
     *
     * This is useful for setting or unsetting single values in a word.
     */
    static constexpr std::array<WordType, MAX_CHARS_PER_KMER> char_mask = {{
        3ul << 0,   3ul << 2,   3ul << 4,   3ul << 6,   3ul << 8,   3ul << 10,  3ul << 12,  3ul << 14,
        3ul << 16,  3ul << 18,  3ul << 20,  3ul << 22,  3ul << 24,  3ul << 26,  3ul << 28,  3ul << 30,
        3ul << 32,  3ul << 34,  3ul << 36,  3ul << 38,  3ul << 40,  3ul << 42,  3ul << 44,  3ul << 46,
        3ul << 48,  3ul << 50,  3ul << 52,  3ul << 54,  3ul << 56,  3ul << 58,  3ul << 60,  3ul << 62
    }};
    static_assert( char_mask[ MAX_CHARS_PER_KMER - 1 ] != 0, "char_mask == 0" );

    /**
     * @brief Bitmask that holds as many consecutive all-one values
     * as the position in the array tells.
     *
     * The element at position `i` in this mask contains `i` many all-one values, starting from
     * the right. (An all-one value for two bit values is 11.)
     *
     *     ones_mask[0]  == 00 00 ... 00 00
     *     ones_mask[1]  == 00 00 ... 00 11
     *     ones_mask[2]  == 00 00 ... 11 11
     *     ...
     *     ones_mask[31] == 00 11 ... 11 11
     *     ones_mask[32] == 11 11 ... 11 11
     *
     * This mask is used for unsetting or extracting remainders of words
     * (all values left or right of a certain position).
     */
    static constexpr std::array<WordType, MAX_CHARS_PER_KMER + 1> ones_mask = {{
        ALL_0,       ALL_1 >> 62, ALL_1 >> 60, ALL_1 >> 58,
        ALL_1 >> 56, ALL_1 >> 54, ALL_1 >> 52, ALL_1 >> 50,
        ALL_1 >> 48, ALL_1 >> 46, ALL_1 >> 44, ALL_1 >> 42,
        ALL_1 >> 40, ALL_1 >> 38, ALL_1 >> 36, ALL_1 >> 34,
        ALL_1 >> 32, ALL_1 >> 30, ALL_1 >> 28, ALL_1 >> 26,
        ALL_1 >> 24, ALL_1 >> 22, ALL_1 >> 20, ALL_1 >> 18,
        ALL_1 >> 16, ALL_1 >> 14, ALL_1 >> 12, ALL_1 >> 10,
        ALL_1 >> 8,  ALL_1 >> 6,  ALL_1 >> 4,  ALL_1 >> 2,
        ALL_1
    }};

    // -------------------------------------------------------------------------
    //     Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Extract the rank value at a position for a given word.
     */
    static inline uint8_t extract( WordType const& word, uint8_t position )
    {
        assert( position <= MAX_CHARS_PER_KMER );
        return ( word & char_mask[position] ) >> ( position * BITS_PER_CHAR );
    }

    /**
     * @brief Implant (set) the rank value at a position for a given word.
     */
    static inline void implant( WordType& word, uint8_t position, uint8_t rank )
    {
        assert( position <= MAX_CHARS_PER_KMER );
        assert( rank <= MAX_RANK );
        word &= ~char_mask[position];
        word |= static_cast<WordType>( rank ) << ( position * BITS_PER_CHAR );
    }

};

} // namespace sequence
} // namespace genesis

#endif // include guard
