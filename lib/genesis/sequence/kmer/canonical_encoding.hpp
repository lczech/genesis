#ifndef GENESIS_SEQUENCE_KMER_CANONICAL_ENCODING_H_
#define GENESIS_SEQUENCE_KMER_CANONICAL_ENCODING_H_

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

/*
    The code below is adapted from the implementation at
    https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer
    which is published under the GNU General Public License v3.0 or later,
    with copyright (C) 2022-2024 Roland Wittler <roland.wittler@uni-bielefeld.de>.

    The concept is described in the following excellent manuscript:

    > Wittler, Roland. 2023. “General Encoding of Canonical k-Mers.” Peer Community Journal 3 (e87).
    > https://doi.org/10.24072/pcjournal.323.

    See there and the original code repository for details.
 */

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/kmer/kmer.hpp"

#include <array>
#include <cassert>
#include <cctype>
#include <cmath>
#include <climits>
#include <cstdbool>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Minimal Canonical Encoding
// =================================================================================================

/**
 * @brief For a given Kmer, compute a minimal encoding to obtain a unique index for each k-mer
 * and its reverse complement.
 *
 * This follows Wittler 2023, [doi:10.24072/pcjournal.323](https://doi.org/10.24072/pcjournal.323).
 * See there for the equations implemented here. We use it for indexing k-mers to achieve a minimal
 * encoding of canonical k-mers, as explained there.
 * The code is based on https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer,
 * but modified to fit our Kmer structure and code conventions.
 */
template<typename Tag>
class MinimalCanonicalEncoding
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using Alphabet = typename Kmer<Tag>::Alphabet;
    using Bitfield = typename Kmer<Tag>::Bitfield;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    MinimalCanonicalEncoding()
    {
        if( Kmer<Tag>::k() == 0 || Kmer<Tag>::k() > 32 ) {
            throw std::invalid_argument(
                "Can only use minimal canonical encoding with k in [1,32]"
            );
        }
        init_masks_();
    }

    ~MinimalCanonicalEncoding() = default;

    MinimalCanonicalEncoding( MinimalCanonicalEncoding const& ) = default;
    MinimalCanonicalEncoding( MinimalCanonicalEncoding&& )      = default;

    MinimalCanonicalEncoding& operator= ( MinimalCanonicalEncoding const& ) = default;
    MinimalCanonicalEncoding& operator= ( MinimalCanonicalEncoding&& )      = default;

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    uint64_t encode( Kmer<Tag> const& kmer ) const
    {
        // Compute enc_r_c, called hash() in https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer
        // We renamed variables, and restructured the code.
        auto const k = Kmer<Tag>::k();

        // Check that the kmer has its rc set (if rc==0, the kmer is maxed out with Ts)
        assert( kmer.rev_comp != 0 || kmer.value == Bitfield::ones_mask[Kmer<Tag>::k()] );

        // Get length of symmetric prefix / suffix, in num of characters.
        uint64_t const sym = kmer.value ^ kmer.rev_comp;
        int l = Bitfield::BIT_WIDTH;
        if( sym != 0 ) {
            l = __builtin_ctzll(sym) / 2 * 2;
        }

        // As per the manuscript, the following holds, with l=2p in the notation from there.
        // Not quite true here due to palindromes (the 0 case above), so we skip this for now.
        // assert( l <= k / 2 );

        // Get the encoding that still includes gaps in the image space
        uint64_t kmerhash = encode_gapped_( kmer.value, kmer.rev_comp, l );

        // Subtract gaps: 2*(k//2-l-1) ones followed by k-2 zeros
        if( l <= k - 4 ) {
            uint64_t gaps = zero_mask_[Bitfield::BIT_WIDTH - (2*(k/2-l/2-1))];
            gaps <<= (2*((k+1)/2)-1);
            kmerhash -= gaps;
        }

        // Subtract gaps in code due to specifying middle position (odd k)
        if( k % 2 == 1 && kmerhash >= pow( 4, k/2+1 )){
            kmerhash -= 2*pow(4,k/2);
        }

        return kmerhash;
    }

    // -----------------------------------------------------
    //     Internal Member Functions
    // -----------------------------------------------------

private:

    void init_masks_()
    {
        // Offset based on k
        auto const k = Kmer<Tag>::k();
        offset_ = Bitfield::BIT_WIDTH - 2 * k;

        // Single bit set at each position
        pos_mask_[Bitfield::BIT_WIDTH] = 1;
        for( int i = Bitfield::BIT_WIDTH - 1; i >= 0; i-- ){
            pos_mask_[i] = pos_mask_[i+1] << 1;
        }

        // Leading ones up to the index: 1 .. 1 0 .. 0
        one_mask_[Bitfield::BIT_WIDTH] = Bitfield::ALL_1;
        for( int i = Bitfield::BIT_WIDTH - 1; i >= 0; i-- ){
            one_mask_[i] = one_mask_[i+1] << 1;
        }

        // Leading zeros up to the index: 0 .. 0 1 .. 1
        zero_mask_[0] = Bitfield::ALL_1;
        for( int i = 1; i <= Bitfield::BIT_WIDTH; i++ ){
            zero_mask_[i] = zero_mask_[i-1] >> 1;
        }

        // Remainder: 0 .. 0 1 .. 1 0 .. 0, depends on k, and only filled up to k.
        remainder_mask_[0] = Bitfield::ALL_1;
        for( int i = 1; i <= k; i++ ){
            remainder_mask_[i] = zero_mask_[i+offset_] & one_mask_[Bitfield::BIT_WIDTH-i];
        }
    }

    uint64_t encode_gapped_( uint64_t val_fw, uint64_t val_rc, int l ) const
    {
        // Function originally part of hash() in https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer
        auto const k = Kmer<Tag>::k();

        // Depending on the type of the specifying pair, we need different ways for the hashing
        uint64_t kmerhash = 0;
        if( l < k - 1 ) {
            // Not just single character in the middle

            // Get the first two asymmetric characters, i.e. 2x2 bits
            unsigned char pattern = 0;
            if( val_fw & pos_mask_[offset_+l+1] ) {
                pattern += 8;
            }
            if( val_fw & pos_mask_[offset_+l+2] ) {
                pattern += 4;
            }
            if( val_fw & pos_mask_[offset_+2*k-l-1] ) {
                pattern += 2;
            }
            if( val_fw & pos_mask_[offset_+2*k-l] ) {
                pattern += 1;
            }
            assert( pattern < 16 );

            // Check which case we need for the initial hash
            if( reverse_[pattern] ) {
                kmerhash = encode_prime_( val_rc, l );
            } else {
                kmerhash = encode_prime_( val_fw, l );
            }

            // Set positions l+1, l+2, l+3 and l+4 according to *-pair-encoding
            if( replace_[0][pattern] ) {
                kmerhash |= pos_mask_[offset_+l+1];
            }
            if( replace_[1][pattern] ) {
                kmerhash |= pos_mask_[offset_+l+2];
            }
            if( replace_[2][pattern] ) {
                kmerhash |= pos_mask_[offset_+l+3];
            }
            if( replace_[3][pattern] ) {
                kmerhash |= pos_mask_[offset_+l+4];
            }
        } else if( l >= k ) {
            // Palindrome -> nothing to do
            l = k;
            kmerhash = encode_prime_( val_fw, l );
        } else {
            // Single character in the middle
            kmerhash = encode_prime_( val_fw, l );

            // Set the bits accordingly
            // A = 00 -> 0
            // C = 01 -> 1
            // G = 10 -> 1
            // T = 11 -> 0
            // The following lines are a correction of the original lines of
            // https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer, as of 2024-08-20.
            // This has been fixed in the original repo now as well (pers. comm. with Roland Wittler).
            if(( val_fw & pos_mask_[offset_+l+1] ) && (( ~val_fw ) & pos_mask_[offset_+l+2] )) {
                kmerhash |= pos_mask_[offset_+l+2];
            }
            if((( ~val_fw ) & pos_mask_[offset_+l+1] ) && ( val_fw & pos_mask_[offset_+l+2] )) {
                kmerhash |= pos_mask_[offset_+l+2];
            }
        }
        return kmerhash;
    }

    uint64_t encode_prime_( uint64_t val, int l ) const
    {
        // Compute encoding where only setting the bits according to specifying case and
        // subtracting gaps is missing, i.e., enc prime.
        // Called initialhashed() in https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer
        auto const k = Kmer<Tag>::k();

        // Pick a precomputed mask consisting of l trailing 1s and 0s else
        // do an AND of that mask and the original k-mer to get the new right part
        uint64_t right = val & zero_mask_[offset_+2*k-l]; // l trailing ones

        // Complement / invert the right part by xor with zeromask
        right = right ^ zero_mask_[offset_+2*k-l];

        // No remainder left? We are done.
        if( l + 2 >= k ) {
            return right;
        }

        // Pick a precomputed mask consisting of ones in the middle to get the remainder,
        // and put it in the correct position.
        uint64_t remainder = ( val & remainder_mask_[l+2] ) >> 2;
        return remainder | right;
    }

    // -----------------------------------------------------
    //     Internal Members
    // -----------------------------------------------------

private:

    // Assumptions on the underlying representation. Needed for later
    // to avoid mistakes, if we introduce different kmer types.
    // Anything that fails here means that the class is used with
    // an incompatible bit representation or alphabet encoding.
    static_assert( Bitfield::BITS_PER_CHAR == 2, "Bitfield::BITS_PER_CHAR != 2" );
    static_assert( Alphabet::SIZE == 4, "Alphabet::SIZE != 4" );
    static_assert( Alphabet::NEGATE_IS_COMPLEMENT, "Alphabet::NEGATE_IS_COMPLEMENT != true" );

    // Replace markers for the function R of Wittler 2023:
    // * 0 A..A -> 0110
    // * 1 A..C -> 0101
    // * 2 A..G -> 0100
    // # 3 palindrome A..T
    // * 4 C..A -> 1000
    // * 5 C..C -> 0111
    // # 6 palindrome C..G
    // # 7 C..T -> A..G -> 0100
    // * 8 G..A -> 1001
    // # 9 palindrome G..C
    // # 10 G..G -> C..C -> 0111
    // # 11 G..T -> A..C -> 0101
    // # 12 palindrome T..A
    // # 13 T..C -> G..A -> 1001
    // # 14 T..G -> C..A -> 1000
    // # 15 T..T -> A..A -> 0110
    std::array<std::array<uint8_t, 16>, 4> const replace_ = {{
        {{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0 }},
        {{ 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1 }},
        {{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }},
        {{ 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0 }}
    }};

    // Reverse markers
    std::array<uint8_t, 16> const reverse_ = {{
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1
    }};

    // Masks to be set at construction. We could hard-code them here as well,
    // but stick with the original implementation for now for simplicty.
    std::array<uint64_t, Bitfield::BIT_WIDTH + 1> pos_mask_;
    std::array<uint64_t, Bitfield::BIT_WIDTH + 1> one_mask_;
    std::array<uint64_t, Bitfield::BIT_WIDTH + 1> zero_mask_;
    std::array<uint64_t, Bitfield::BIT_WIDTH + 1> remainder_mask_;

    // Data based on k
    uint8_t offset_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
