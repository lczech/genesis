#ifndef GENESIS_SEQUENCE_KMER_ALPHABET_H_
#define GENESIS_SEQUENCE_KMER_ALPHABET_H_

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
 * @ingroup sequence
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/char.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Nucleotide Alphabet
// =================================================================================================

struct KmerAlphabet
{
    // -------------------------------------------------------------------------
    //     Constants
    // -------------------------------------------------------------------------

    /**
     * @brief All characters in the alphabet.
     */
    static const std::string ALPHABET;

    /**
     * @brief Size of the alphabet, i.e., the number of different symbols in it.
     */
    static const uint8_t SIZE = 4;

    /**
     * @brief Maximum rank that the alphabet uses.
     *
     * This should be equal or smaller than the max rank of the bitmask encoding.
     */
    static const uint8_t MAX_RANK = 0x03;

    /**
     * @brief Declare that negating bits corresponds to complementing the value.
     *
     * This is mainly intended for future-proofness, so that we can staticically assert this
     * where this property is used, and detect those places later if we add alphabets
     * for which this is not true.
     */
    static const bool NEGATE_IS_COMPLEMENT = true;

    // /**
    //  * @brief Translation table from ASCII code to nucleotide rank code.
    //  *
    //  * We use A=0, C=1, G=2, T=3, N=4, and all others 8, to allow checking illegal chars with the
    //  * special case of N, which is legal, but might need to be skipped. Could be extended to allow
    //  * for all alpahbetical or all IUPAC codes, or something similar, if needed. Currently not used.
    //  */
    // static constexpr std::array<uint8_t, 256> ascii_to_nt4 = {{
    //     /*   0 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /*  16 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /*  32 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8,
    //     /*                                               -        */
    //     /*  48 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /*  64 */ 8, 0, 8, 1, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 4, 8,
    //     /*           A     C           G                    N     */
    //     /*  80 */ 8, 8, 8, 8, 3, 3, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /*                    T  U                                */
    //     /*  96 */ 8, 0, 8, 1, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 4, 8,
    //     /*           a     c           g                    n     */
    //     /* 112 */ 8, 8, 8, 8, 3, 3, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /*                    t  u                                */
    //     /* 128 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /* 144 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /* 160 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /* 176 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /* 192 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /* 208 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /* 224 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    //     /* 240 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    // }};

    // -------------------------------------------------------------------------
    //     Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Return the rank, given an ASCII char.
     *
     * If the input char is invalid, the max of uint8_t is returned.
     * The latter can then be checked against KmerBitmask::MAX_RANK to see if the rank is valid.
     */
    static inline uint8_t char_to_rank( char c ) noexcept
    {
        // Basic input checks. Checking upper and lower case in one condition is faster in our tests
        // than turning it into one case and doing fewer checks - likely due to branch prediction.
        // c = utils::to_upper( c );
        if(
            ( c != 'A' ) && ( c != 'C' ) && ( c != 'G' ) && ( c != 'T' ) &&
            ( c != 'a' ) && ( c != 'c' ) && ( c != 'g' ) && ( c != 't' )
        ) GENESIS_CPP_UNLIKELY {
            return std::numeric_limits<uint8_t>::max();
        }

        // We need ASCII for the following to work. Probably fine, but doesn't hurt to check.
        static_assert( static_cast<int>('A') == 0x41, "Non-ASCII char set" );
        static_assert( static_cast<int>('C') == 0x43, "Non-ASCII char set" );
        static_assert( static_cast<int>('G') == 0x47, "Non-ASCII char set" );
        static_assert( static_cast<int>('T') == 0x54, "Non-ASCII char set" );
        static_assert( static_cast<int>('a') == 0x61, "Non-ASCII char set" );
        static_assert( static_cast<int>('c') == 0x63, "Non-ASCII char set" );
        static_assert( static_cast<int>('g') == 0x67, "Non-ASCII char set" );
        static_assert( static_cast<int>('t') == 0x74, "Non-ASCII char set" );

        // For extra speed, we exploit the ASCII code of the characters. We already checked above
        // that we are dealing with valid ones, so this is fine. The lower halves of each byte are:
        // A 0001
        // C 0011
        // G 0111
        // T 0100
        //   -^^-
        // These have a pattern in the middle bits (marked) that we use; doing a single right shift
        // puts those into the two rightmost bits of the result. The first of them (the left one)
        // is already what we want (A=C=0 and G=T=1), but the other (the right one) is not
        // (A=T=0 and C=G=1, but we want A=G=0 and C=T=1 for that bit). We xor with the other bit
        // to get our result, as that has a 1 for the G and the T, and gives us the encoding that
        // we want. Luckily, the fourth bit is always zero here, so that it does not mess this up.
        // This works for upper and lower case, as the case bit is in the higher four bits,
        // which are ignored here anyway. In our tests, this is the fastest method.
        unsigned char const u = c;
        return ((u >> 1) ^ (u >> 2)) & 3;

        // The following is another fast solution, see https://github.com/seqan/seqan3/issues/1970
        // Alternatively, a lookup table, or a switch could be used, but those are way slower.
        // return 0 * ( c == 'A' ) + 1 * ( c == 'C' ) + 2 * ( c == 'G' ) + 3 * ( c == 'T' );
    }

    /**
     * @brief Return the ASCII representation of a given rank.
     */
    static inline char rank_to_char( uint8_t r )
    {
        switch( r ) {
            case 0x00: {
                return 'A';
            }
            case 0x01: {
                return 'C';
            }
            case 0x02: {
                return 'G';
            }
            case 0x03: {
                return 'T';
            }
        }
        throw std::invalid_argument( "Invalid nucleotide alphabet rank " + std::to_string( r ));
    }

    /**
     * @brief Get the complement rank.
     */
    static inline constexpr uint8_t complement( uint8_t rank ) noexcept
    {
        return MAX_RANK - rank;
    }

};

} // namespace sequence
} // namespace genesis

#endif // include guard
