#ifndef GENESIS_SEQUENCE_KMER_ALPHABET_H_
#define GENESIS_SEQUENCE_KMER_ALPHABET_H_

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
     * If the input char is invalid, either an exception is thrown, or the max of uint8_t is returned.
     * The latter can then be checked against KmerBitmask::MAX_RANK to see if the rank is valid.
     */
    static inline uint8_t char_to_rank( char c, bool throw_if_invalid = false )
    {
        // Basic checks on the input.
        c = utils::to_upper( c );
        if(( c != 'A' ) && ( c != 'C' ) && ( c != 'G' ) && ( c != 'T' )) {
            if( throw_if_invalid ) {
                throw std::invalid_argument(
                    "Cannot use char " + utils::char_to_hex( c ) + " to construct ACGT k-mer"
                );
            } else {
                return std::numeric_limits<uint8_t>::max();
            }
        }

        // The following seems to be the fastest, see https://github.com/seqan/seqan3/issues/1970
        // Alternatively, the table, or a switch could be used.
        return 0 * ( c == 'A' ) + 1 * ( c == 'C' ) + 2 * ( c == 'G' ) + 3 * ( c == 'T' );
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
