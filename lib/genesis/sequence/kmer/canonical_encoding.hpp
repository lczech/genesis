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
    with copyright (C) 2022-2023 Roland Wittler <roland.wittler@uni-bielefeld.de>.

    The concept is described in the following excellent manuscript:

    > Wittler, Roland. 2023. “General Encoding of Canonical k-Mers.” Peer Community Journal 3 (e87).
    > https://doi.org/10.24072/pcjournal.323.

    See there and the original code repository for details. We thank Roland Wittler for the input
    on the original implementation and for helping to refine this adaptation of it!
 */

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/kmer/kmer.hpp"
#include "genesis/utils/math/common.hpp"

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
 * but modified to fit our Kmer structure and code conventions. Furthermore, we added several code
 * optimizations for speed, and replaced some pre-computed masks by direct bit operations. We have
 * also contributed these improvements back to the original repository, so that the basic code
 * matches the one here.
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
        init_();
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

        // Get the length of the symmetric prefix/suffix, in num of characters, i.e., 2x num of bits.
        // We check the special case sym==0, as calling ctz(0) is undefined behavior.
        // Then, l is the bit index of the char that is the specifying case for the k-mer.
        uint64_t const sym = kmer.value ^ kmer.rev_comp;
        uint8_t const l = ( sym == 0 ? Bitfield::BIT_WIDTH : ( __builtin_ctzll(sym) / 2 * 2 ));
        assert( l % 2 == 0 );

        // The above builtin call uses unsigned long long. Assert that this has the size we expect.
        static_assert(
            sizeof(unsigned long long) >= sizeof(uint64_t),
            "sizeof(unsigned long long) < sizeof(uint64_t)"
        );

        // Get the encoding that still includes gaps in the image space.
        // Depending on the type of the specifying pair, we need different ways for this.
        uint64_t kmerhash = 0;
        if( l < k - 1 ) {
            // Not just single character in the middle, i.e., we have a specifying pair.

            // There are 16 possible combinations of two characters from ACGT.
            // We here extract the first two asymmetric characters (the specifying pair, i.e. 2x2 bits)
            // to build a pattern for a lookup of which combination we have in the kmer.
            // This is done by shifting the relevant bits of the pair to the LSBs of the pattern.
            unsigned char pattern = 0;
            pattern |= ( kmer.value >> ( 2*k - l - 4 )) & 0x0C;
            pattern |= ( kmer.value >> (       l     )) & 0x03;
            assert( pattern < 16 );

            // Check which case we need for the initial hash, based on the pattern we found.
            if( reverse_[pattern] ) {
                kmerhash = encode_prime_( kmer.rev_comp, l );
            } else {
                kmerhash = encode_prime_( kmer.value, l );
            }

            // Set positions l+1, l+2, l+3 and l+4 according to the specifying pair pattern,
            // which is called R in the manuscript.
            // Similar to above, we can avoid any branching here by directly shifting
            // the replace_ mask bits to the needed positions. If the replace mask is 0 for the
            // given pattern, we shift a zero, which just does nothing.
            kmerhash |= ( replace_[pattern] << ( 2*k - l - 4 ));
        } else if( l == k - 1 ) {
            // Single character in the middle. Can only occurr in odd k.
            assert( k % 2 == 1 );

            // We are interested in the bits at the central character,
            // which (given that we have l == k - 1 here) are located at:
            //     2*k - l - 1 == k
            //     2*k - l - 2 == k - 1
            // Use these bits to encode A/T -> 0 and C/G -> 1:
            //     A = 00 -> 0
            //     C = 01 -> 1
            //     G = 10 -> 1
            //     T = 11 -> 0
            // Depending on the combination of those two bits, we want to set a bit in kmerhash.
            // In particular, we want to set the same bit as the second of the two above positions,
            // but only if both bit positions are different (C or G). For this, we first obtain
            // both bits of the kmer, and use XOR to see if they are different. To this end,
            // bit1 is shifted by 1 so that it is in the same positon as bit2.
            // The result of this XOR is a single bit indicating if we have C/G or A/T at the position,
            // and it is already in the correct position to be set in kmerhash.
            kmerhash = encode_prime_( kmer.value, l );
            auto const bit1 = ( kmer.value & ( 1ULL << ( k   ))) >> 1;
            auto const bit2 = ( kmer.value & ( 1ULL << ( k-1 )));
            kmerhash |= bit1 ^ bit2;
        } else {
            // Palindrome -> nothing to do. Can only occurr in even k.
            assert( k % 2 == 0 );
            assert( l >= k );

            // We use l = k here, as in a palindrome, l will overshoot due to the ctl call,
            // so we limit it here to the range that we are interested in.
            kmerhash = encode_prime_( kmer.value, k );
        }

        // Subtract gaps: 2*(k//2-l-1) ones followed by k-2 zeros
        if( l <= k - 4 ) {
            auto const ALL_1 = Bitfield::ALL_1;
            auto const shift = 2 * ( k/2 - l/2 - 1 );
            uint64_t gaps = ( shift == 0 ? ALL_1 : ( ALL_1 >> ( Bitfield::BIT_WIDTH - shift )));
            gaps <<= ( 2 * (( k+1 ) / 2 ) - 1 );
            kmerhash -= gaps;
        }

        // Subtract gaps in code due to specifying middle position (odd k).
        // We here use pre-computed powers of four for speed, as those are constant.
        assert( k <= 32 );
        if( k % 2 == 1 && kmerhash >= four_to_the_k_half_plus_one_ ) {
            kmerhash -= twice_four_to_the_k_half_;
        }

        return kmerhash;
    }

    // -----------------------------------------------------
    //     Internal Member Functions
    // -----------------------------------------------------

private:

    void init_()
    {
        // We need two variants of powers of 4 depending on k, which we can pre-compute for speed,
        // as this is too expensive for the hot path.
        auto const k = Kmer<Tag>::k();
        four_to_the_k_half_plus_one_ = utils::int_pow( 4, k / 2 + 1 );
        twice_four_to_the_k_half_ = 2 * utils::int_pow( 4, k / 2 );

        // After we have identified the specifying pair of characters, we need to extract
        // the remainder, see encode_prime_(). We here precompute a mask to do that.
        // For instance, for k==7, the relevant entries are shaped like this:
        //
        //     remainder_mask_[2] == 00 .. 00 11 11 11 11 00
        //     remainder_mask_[4] == 00 .. 00 00 11 11 00 00
        //     remainder_mask_[6] == 00 .. 00 00 11 00 00 00
        //
        // We only ever need to access entries at even indices, as this is indexed per bit,
        // and we use index access to the starting bit of the characters.
        // Lastly, as explained in encode_prime_(), we also might access entries
        // beyond the given triangle of 1s, so we fill those with zeros here.
        remainder_mask_[0] = Bitfield::ALL_1;
        for( size_t i = 1; i <= k; i++ ){
            auto const zeromask = Bitfield::ALL_1 >> ( Bitfield::BIT_WIDTH - 2 * k + i );
            auto const onemask  = Bitfield::ALL_1 << i;
            remainder_mask_[i]  = zeromask & onemask;
        }
        for( size_t i = k+1; i < remainder_mask_.size(); ++i ) {
            remainder_mask_[i] = 0;
        }
    }

    inline uint64_t encode_prime_( uint64_t const val, uint8_t const l ) const
    {
        // Compute encoding where only setting the bits according to specifying case and
        // subtracting gaps is missing, i.e., enc prime.
        // Called initialhashed() in https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer
        auto const k = Kmer<Tag>::k();
        (void) k;

        // This computes a mask of the form 0..01..1 (l trailing ones), to extract
        // the relevant bits on the right, and invert (complement) them.
        // We need a special case for l==0 due to shifting being undefined behavior
        // if the shift is larger than or equal to the bit width.
        uint64_t const zeromask = ( l == 0 ? 0 : Bitfield::ALL_1 >> ( Bitfield::BIT_WIDTH - l ));
        uint64_t const right = (val & zeromask) ^ zeromask;

        // No remainder left? We could just return here, but in our tests, the introduced
        // branching is more expensive than unconditionally executing the below bit operations,
        // so we have deactivated this check here. Recommended to be tested on your hardware.
        // if( l + 2 >= k ) {
        //     return right;
        // }

        // Assert that the values are as expected.
        assert( l <= k );
        assert( l % 2 == 0 );

        // Use the remainder mask (consisting of ones in the middle) to extract the bits
        // in between the specifying pair, then shift the remainder to the correct position.
        // The mask contains 0 after index k, so that if we have l+2 >= k (no remainder),
        // we just get a zero here, which does nothing to our result.
        uint64_t const remainder = ( val & remainder_mask_[l+2] ) >> 2;
        return right | remainder;
    }

    // -----------------------------------------------------
    //     Internal Members
    // -----------------------------------------------------

private:

    // Assumptions on the underlying representation. Needed for later
    // to avoid mistakes, if we introduce different kmer types.
    // Anything that fails here means that the class is used with
    // an incompatible bit representation or alphabet encoding.
    static_assert( Bitfield::BIT_WIDTH == 64, "Bitfield::BIT_WIDTH != 64" );
    static_assert( Bitfield::BITS_PER_CHAR == 2, "Bitfield::BITS_PER_CHAR != 2" );
    static_assert( Alphabet::SIZE == 4, "Alphabet::SIZE != 4" );
    static_assert( Alphabet::NEGATE_IS_COMPLEMENT, "Alphabet::NEGATE_IS_COMPLEMENT != true" );

    // Replace markers for the function R of Wittler 2023.
    // We code those as a lookup table, where each entry is a single word
    // containing the four bits of the following list in their LSBs.
    // We store those as the type of our underlying data, so that we can
    // directly shift those values to the position where they are needed.
    // *  0 A..A            -> 0110
    // *  1 A..C            -> 0101
    // *  2 A..G            -> 0100
    // #  3 A..T palindrome -> 0000
    // *  4 C..A            -> 1000
    // *  5 C..C            -> 0111
    // #  6 C..G palindrome -> 0000
    // #  7 C..T -> A..G    -> 0100
    // *  8 G..A            -> 1001
    // #  9 G..C palindrome -> 0000
    // # 10 G..G -> C..C    -> 0111
    // # 11 G..T -> A..C    -> 0101
    // # 12 T..A palindrome -> 0000
    // # 13 T..C -> G..A    -> 1001
    // # 14 T..G -> C..A    -> 1000
    // # 15 T..T -> A..A    -> 0110
    std::array<typename Bitfield::WordType, 16> const replace_ = {{
        0x06, 0x05, 0x04, 0x00, 0x08, 0x07, 0x00, 0x04, 0x09, 0x00, 0x07, 0x05, 0x00, 0x09, 0x08, 0x06
    }};

    // Markers to check if we need to encode the forward or the reverse complement.
    std::array<uint8_t, 16> const reverse_ = {{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1 }};

    // Mask to extract the remainder after having found the specifying pair. Depends on k,
    // and is created on construction. We might access positions up to k+2, hence the max size here.
    std::array<typename Bitfield::WordType, Bitfield::MAX_CHARS_PER_KMER + 2> remainder_mask_;

    // Powers are expensive to compute, but these here only depend on k, so we can pre-compute them.
    uint64_t four_to_the_k_half_plus_one_;
    uint64_t twice_four_to_the_k_half_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
