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
 * optimizations for speed, and replaced the pre-computed masks by direct bit operations.
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

        // Get length of symmetric prefix / suffix, in num of characters, i.e., 2x num of bits.
        // Compared to the original implementation, we check the special case sym==0,
        // as calling ctz(0) is undefined behavior, which we avoid here.
        // Then, l is the bit index of the char that is the specifying case for the k-mer.
        uint64_t const sym = kmer.value ^ kmer.rev_comp;
        uint8_t const l = ( sym == 0 ? Bitfield::BIT_WIDTH : ( __builtin_ctzll(sym) / 2 * 2 ));
        assert( l % 2 == 0 );

        // The above builtin call uses unsigned long long. Assert that this has the size we expect.
        static_assert(
            sizeof(unsigned long long) >= sizeof(uint64_t),
            "sizeof(unsigned long long) < sizeof(uint64_t)"
        );

        // Get the encoding that still includes gaps in the image space
        uint64_t kmerhash = encode_gapped_( kmer.value, kmer.rev_comp, l );

        // Subtract gaps: 2*(k//2-l-1) ones followed by k-2 zeros
        if( k >= 4 && l <= k - 4 ) {
            // The original code uses a pre-computed bit mask here to get the value we need:
            // uint64_t gaps = zeromasks[64 - (2*(k/2-l/2-1))];
            // We here instead compute that value directly, so that we do not need the mask.
            // See below encode_prime_() for an explanation of the zeromasks bit masks.
            auto const ALL_1 = Bitfield::ALL_1;
            auto const shift = 2 * ( k/2 - l/2 - 1 );
            uint64_t gaps = ( shift == 0 ? ALL_1 : ( ALL_1 >> ( Bitfield::BIT_WIDTH - shift )));
            gaps <<= ( 2 * (( k+1 ) / 2 ) - 1 );
            kmerhash -= gaps;
        }

        // Subtract gaps in code due to specifying middle position (odd k).
        // Compared to the original code, we use int exponantiation here instead of the slow powl().
        assert( k <= 32 );
        if( k % 2 == 1 && kmerhash >= utils::int_pow( 4, k / 2 + 1 )) {
            kmerhash -= 2 * utils::int_pow( 4, k / 2 );
        }

        return kmerhash;
    }

    // -----------------------------------------------------
    //     Internal Member Functions
    // -----------------------------------------------------

private:

    inline uint64_t encode_gapped_( uint64_t const val_km, uint64_t const val_rc, uint8_t const l ) const
    {
        // Function originally part of hash() in https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer
        auto const k = Kmer<Tag>::k();

        // The original code uses pre-computed masks with a single bit set to extract the bits
        // of the k-mer to check their values, and then sets values in the pattern variable
        // to indicate the case we are in (needed for the R function of the manuscript).
        // The mask is accessed via an offset depending on k: offset = 64 - 2 * k
        //
        // posmasks[x]  ==  1ULL << (64 - x);
        // posmasks[0]  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        // posmasks[1]  10000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        // ...
        // posmasks[63] 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000010
        // posmasks[64] 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001
        //
        // Combined with the offset, this gives us:
        // posmasks[offset+x]  ==  1ULL << (2*k - x);
        //
        // We here instead shift the bits of the k-mer directly to the position where they are
        // needed to build the pattern, and extract them there, in order to avoid branching and
        // repeated accesses to the masks. This is significantly faster, and works without masks.
        // This mask is used throughout the encode functions called here.

        // Depending on the type of the specifying pair, we need different ways for the hashing
        uint64_t kmerhash = 0;
        if( l < k - 1 ) {
            kmerhash = encode_gapped_specifying_pair_( val_km, val_rc , l );
        } else if( l == k - 1 ) {
            kmerhash = encode_gapped_single_character_( val_km , l );
        } else {
            kmerhash = encode_gapped_palindrome_( val_km , l );
        }
        return kmerhash;
    }

    inline uint64_t encode_gapped_specifying_pair_(
        uint64_t const val_km, uint64_t const val_rc, uint8_t const l
    ) const {
        // Not just single character in the middle, i.e., we have a specifying pair.
        auto const k = Kmer<Tag>::k();
        uint64_t kmerhash = 0;

        // Get the first two asymmetric characters, i.e. 2x2 bits.
        // Original code for reference:
        // if( val_km & posmasks[offset+l+1]     ){ pattern += 8; }
        // if( val_km & posmasks[offset+l+2]     ){ pattern += 4; }
        // if( val_km & posmasks[offset+2*k-l-1] ){ pattern += 2; }
        // if( val_km & posmasks[offset+2*k-l]   ){ pattern += 1; }

        // Instead of checking individual bits and then setting individual bits again,
        // we can directly shift the needed bit in the kmer to the position we need,
        // extract that single bit right there, and use that for amending the pattern.
        // We use use equality posmasks[offset+x] from above.
        // With that, we can turn the above accesses to posmasks into a direct bit shift:
        //     posmasks[offset+l+1]     == (1ULL << ( 2*k-l-1 ))
        //     posmasks[offset+l+2]     == (1ULL << ( 2*k-l-2 ))
        //     posmasks[offset+2*k-l-1] == (1ULL << ( l+1 ))
        //     posmasks[offset+2*k-l]   == (1ULL << ( l ))
        // Lastly, we want to use the result of those bits to create a pattern of four bits.
        // We can directly adjust the shifting so that the relevant bit of the kmer
        // ends up exctly where we need it to build the pattern, and then use a mask of that
        // bit to exact it in the correct position:
        unsigned char pattern = 0;
        pattern |= ( val_km >> ( 2*k - l - 1 - 3 )) & 0x08;
        pattern |= ( val_km >> ( 2*k - l - 2 - 2 )) & 0x04;
        pattern |= ( val_km >> (       l + 1 - 1 )) & 0x02;
        pattern |= ( val_km >> (       l     - 0 )) & 0x01;
        assert( pattern < 16 );

        // Check which case we need for the initial hash
        if( reverse_[pattern] ) {
            kmerhash = encode_prime_( val_rc, l );
        } else {
            kmerhash = encode_prime_( val_km, l );
        }

        // Set positions l+1, l+2, l+3 and l+4 according to *-pair-encoding
        // Previous code for reference:
        // if( replace1[pattern] ){ kmerhash |= posmasks[offset+l+1]; }
        // if( replace2[pattern] ){ kmerhash |= posmasks[offset+l+2]; }
        // if( replace3[pattern] ){ kmerhash |= posmasks[offset+l+3]; }
        // if( replace4[pattern] ){ kmerhash |= posmasks[offset+l+4]; }

        // Similar to the trick above, we can avoid the branching here by directly shifting
        // the replace mask bits to the needed positions. If the replace mask is 0 for the
        // given pattern, we shift a 0, which just does nothing, so that's fine.
        kmerhash |= ( replace1_[pattern] << ( 2*k - l - 1 ));
        kmerhash |= ( replace2_[pattern] << ( 2*k - l - 2 ));
        kmerhash |= ( replace3_[pattern] << ( 2*k - l - 3 ));
        kmerhash |= ( replace4_[pattern] << ( 2*k - l - 4 ));
        return kmerhash;
    }

    inline uint64_t encode_gapped_single_character_(
        uint64_t const val_km, uint8_t const l
    ) const {
        // Single character in the middle. Can only occurr in odd k.
        auto const k = Kmer<Tag>::k();
        assert( k % 2 == 1 );
        uint64_t kmerhash = encode_prime_( val_km, l );

        // Set the bits accordingly
        // A = 00 -> 0
        // C = 01 -> 1
        // G = 10 -> 1
        // T = 11 -> 0

        // Original code for reference. Note that the original implementation had a bug
        // where instead of the above assignment, A and G would be combined, as well as C and T.
        // This code here already fixes this, so that we have a correct reference. This has also
        // been fixed in the original code now as well (pers. comm. with Roland Wittler).
        // if((  val_km & posmasks[offset+l+1] ) && ( ~val_km & posmasks[offset+l+2] )) {
        //     kmerhash |= posmasks[offset+l+2];
        // }
        // if(( ~val_km & posmasks[offset+l+1] ) && (  val_km & posmasks[offset+l+2] )) {
        //     kmerhash |= posmasks[offset+l+2];
        // }

        // We here extend on the above bit hacking ideas. We are interested in kmer bits
        //     posmasks[offset+l+1] == (1ULL << (2*k-l-1))
        //     posmasks[offset+l+2] == (1ULL << (2*k-l-2))
        // and depending on their state, want to set a bit in kmerhash. In particular,
        // we want to set the same bit as the second of the two above positions, but only
        // if both bit positions are different (C or G). For this, we first obtain both bits
        // of the kmer, as before. We can hence use XOR on those two bits to see if they are
        // different. To this end, bit1 is shifted by 1 so that it is in the same positon
        // as bit2. The result of this XOR is a single bit in the correct position, indicating
        // if we have C/G or A/T at the position, and it is already in the correct position
        // to be set in kmerhash.
        auto const bit1 = ( val_km & ( 1ULL << ( 2*k-l-1 ))) >> 1;
        auto const bit2 = ( val_km & ( 1ULL << ( 2*k-l-2 )));
        kmerhash |= bit1 ^ bit2;
        return kmerhash;
    }

    inline uint64_t encode_gapped_palindrome_(
        uint64_t const val_km, uint8_t const l
    ) const {
        // Palindrome -> nothing to do.
        // Can only occurr in even k.
        auto const k = Kmer<Tag>::k();
        assert( k % 2 == 0 );
        assert( l >= k );

        // We use l = k here, as l might overshoot.
        return encode_prime_( val_km, k );
    }

    inline uint64_t encode_prime_( uint64_t const val, uint8_t const l ) const
    {
        // Compute encoding where only setting the bits according to specifying case and
        // subtracting gaps is missing, i.e., enc prime.
        // Called initialhashed() in https://gitlab.ub.uni-bielefeld.de/gi/MinEncCanKmer
        auto const k = Kmer<Tag>::k();

        // In the original code, several pre-computed masks are used,
        // all accessed via an offset depending on k:
        // offset = 64 - 2 * k;
        //
        // Furthermore, the original code uses pre-computed masks to extract bits. However,
        // it is equally as fast if not faster to simply use bit shifts here to create the masks
        // on the fly. The two general masks are zeromasks and onemasks, as follows.
        //
        // zeromasks[x]  ==  ALL_1 >> x;
        // zeromasks[0]  11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        // zeromasks[1]  01111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        // ...
        // zeromasks[63] 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001
        // zeromasks[64] 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        //
        // onemasks[x]   ==  ALL_1 << (64 - x);
        // onemasks[0]   00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        // onemasks[1]   10000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        // ...
        // onemasks[63]  11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111110
        // onemasks[64]  11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111

        // This computes zeromasks[offset+2*k-l]. We need a special case for l==0 due to shifting
        // being undefined behavior if the shift is larger than then bit width...
        uint64_t const zeromask = ( l == 0 ? 0 : Bitfield::ALL_1 >> ( Bitfield::BIT_WIDTH - l ));

        // Use the mask with l trailing 1s and 0s else to extract the new right part.
        // Then, complement / invert the right part by xor with zeromask.
        uint64_t const right = (val & zeromask) ^ zeromask;

        // No remainder left? We are done.
        // When compiled with gcc 7, we here get a compiler warning (that is then turned into an
        // error due to our pedantic warning policy) for the code path where this function here is
        // called from encode_gapped_palindrome_().
        // In that case, we have l==k, meaning that the condition here is always true, and the
        // compiler will tell us, see https://stackoverflow.com/a/17205195/4184258
        // That is an absolutely unnecessary warning, so we here just silence it here.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wstrict-overflow"
        if( l + 2 >= k ) {
            return right;
        }
        #pragma GCC diagnostic pop

        // Assert that the values are as expected. The last assertion is needed to avoid
        // bit shifting by the bit width, which again would be undefined behavior.
        assert( l + 2 < k );
        assert( l % 2 == 0 );

        // At this point, the original code uses a remainder mask to extract the bits in between
        // the specifying pair. Depending on k, it uses a triangular bit matrix, of which only
        // ever the entries at even indices are used. For instance for k==5:
        //
        // remaindermasks[2] 00000000 00000000 00000000 00000000 00000000 00000000 00000000 11111100
        // remaindermasks[4] 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00110000
        //
        // We need remaindermasks[l+2], which can be constructed as in the original pre-computation:
        // remaindermasks[x] = zeromasks[x+offset] & onemasks[64-x];
        // Following the replacement logic of the above masks, this can be done directly:
        uint64_t const remaindermask_z = Bitfield::ALL_1 >> ( Bitfield::BIT_WIDTH - 2*k + l + 2 );
        uint64_t const remaindermask_o = Bitfield::ALL_1 << ( l + 2 );
        uint64_t const remaindermask = remaindermask_z & remaindermask_o;

        // Use the mask consisting of ones in the middle to extrat the remainder,
        // then shift the remainder to the correct position.
        uint64_t const remainder = ( val & remaindermask ) >> 2;
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
    std::array<uint64_t, 16> const replace1_ = {{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0 }};
    std::array<uint64_t, 16> const replace2_ = {{ 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1 }};
    std::array<uint64_t, 16> const replace3_ = {{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }};
    std::array<uint64_t, 16> const replace4_ = {{ 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0 }};

    // Reverse markers
    std::array<uint8_t, 16> const reverse_ = {{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1 }};

};

} // namespace sequence
} // namespace genesis

#endif // include guard
