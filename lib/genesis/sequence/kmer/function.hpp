#ifndef GENESIS_SEQUENCE_KMER_FUNCTION_H_
#define GENESIS_SEQUENCE_KMER_FUNCTION_H_

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

#include "genesis/sequence/kmer/kmer.hpp"
#include "genesis/utils/math/common.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace genesis {
namespace sequence {

// =================================================================================================
//     String Functions
// =================================================================================================

/**
 * @brief Construct a kmer from an input string.
 *
 * The string size has to match the value of k set of the given kmer tag. This is mostly meant for
 * testing and debugging, to be able to quickly create a kmer from a given string value.
 */
template<typename Tag>
Kmer<Tag> kmer_from_string( std::string const& input )
{
    Kmer<Tag> result;
    if( input.size() != Kmer<Tag>::k() ) {
        throw std::invalid_argument(
            "Cannot construct kmer of size " + std::to_string( Kmer<Tag>::k() ) +
            " from input string of size " + std::to_string( input.size() )
        );
    }
    for( size_t i = 0; i < input.size(); ++i ) {
        auto const rank = Kmer<Tag>::Alphabet::char_to_rank( input[i] );
        result.set( i, rank );
    }
    return result;
}

/**
 * @brief Get a string representation of a @p kmer.
 */
template<typename Tag>
std::string kmer_to_string( Kmer<Tag> const& kmer )
{
    auto result = std::string( kmer.k(), '#' );
    for( size_t i = 0; i < kmer.k(); ++i ) {
        result[i] = Kmer<Tag>::Alphabet::rank_to_char( kmer[i] );
    }
    return result;
}

/**
 * @brief Get a string representation of the bits in a @p kmer.
 */
template<typename Tag>
std::string kmer_bits_to_string( Kmer<Tag> const& kmer )
{
    using Bitfield = typename Kmer<Tag>::Bitfield;
    typename Bitfield::WordType const one = 1;

    std::string result;
    for( size_t i = 0; i < Bitfield::BIT_WIDTH; ++i ) {
        if( i > 0 && i % Bitfield::BITS_PER_CHAR == 0 ) {
            result += " ";
        }

        auto const pos = one << ( Bitfield::BIT_WIDTH - i - 1);
        if( kmer.value & pos ) {
            result += "1";
        } else {
            result += "0";
        }
    }
    return result;
}

/**
 * @brief Print the string representation of a @p kmer to an @p output stream.
 */
template<typename Tag>
std::ostream& operator<< ( std::ostream& output, Kmer<Tag> const& kmer )
{
    output << kmer_to_string( kmer );
    return output;
}

// =================================================================================================
//     Kmer Comparison
// =================================================================================================

template<typename Tag>
bool operator==( Kmer<Tag> const& lhs, Kmer<Tag> const& rhs )
{
    return lhs.value == rhs.value;
}

template<typename Tag>
bool operator!=( Kmer<Tag> const& lhs, Kmer<Tag> const& rhs )
{
    return lhs.value != rhs.value;
}

template<typename Tag>
bool operator<=( Kmer<Tag> const& lhs, Kmer<Tag> const& rhs )
{
    return lhs.value <= rhs.value;
}

template<typename Tag>
bool operator>=( Kmer<Tag> const& lhs, Kmer<Tag> const& rhs )
{
    return lhs.value >= rhs.value;
}

template<typename Tag>
bool operator<( Kmer<Tag> const& lhs, Kmer<Tag> const& rhs )
{
    return lhs.value < rhs.value;
}

template<typename Tag>
bool operator>( Kmer<Tag> const& lhs, Kmer<Tag> const& rhs )
{
    return lhs.value > rhs.value;
}

// =================================================================================================
//     Counting
// =================================================================================================

/**
 * @brief Compute the total number of possible k-mers for a given @p k and @p alphabet_size.
 *
 * The default for @p alphabet_size is `4`, for typical nucleotide k-mers over the alphabet `ACGT`.
 * For instance, with `k==6`, this yields `4*4*4*4*4*4 == 4096` possible k-mers of that size.
 */
inline size_t number_of_kmers( uint8_t k, uint8_t alphabet_size = 4 )
{
    // We do an explicit loop instead of using std::pow,
    // in case that double precision is not enough here.
    size_t n = 1;
    for( size_t i = 0; i < k; ++i ) {
        n *= alphabet_size;
    }
    return n;
}

/**
 * @brief Compute the number of canonical k-mers for a given k and nucleotide alphabet.
 *
 * This follows Wittler 2023, [doi:10.24072/pcjournal.323](https://doi.org/10.24072/pcjournal.323).
 * See there for the equations implemented here. We use it for indexing k-mers to achieve a minimal
 * encoding of canonical k-mers, as explained there.
 */
inline size_t number_of_canonical_kmers( uint8_t k )
{
    // We need distinct approaches for even and odd values, due to palindromes.
    // It might be easier to just have a hard coded table... but this way is more approachable.
    if( k == 0 || k > 32 ) {
        throw std::invalid_argument( "Can only compute minimal encoding size for k in [1,32]" );
    } else if( k % 2 == 0 ) {
        // Even numbers, need to add palindromes.
        // We use base 2 here, and instead of dividing the result by 2 in the end, we subtract 1
        // from the exponent, in order to avoid overflowing for the case k=32.
        // The original (overflowing) equation from the paper is commented out below for reference.
        return utils::int_pow( 2, 2 * k - 1 ) + utils::int_pow( 2, 2 * k / 2 - 1 );
        // return ( utils::int_pow( 4, k ) + utils::int_pow( 4, k / 2 )) / 2;
    } else {
        // Odd numbers. No overflow for the valid range.
        return utils::int_pow( 4, k ) / 2;
    }
    return 0;
}

/**
 * @brief Compute the number of palindromes (under reverse complmenet) that exist
 * for a given @p k and nucleotide alphabet.
 *
 * This is `0` for odd values of @p k, and `4^(k/2)` for even values of @p k.
 */
inline size_t number_of_palindromes( uint8_t k )
{
    // Edge and special cases.
    if( k == 0 || k > 32 ) {
        throw std::invalid_argument( "Can only compute minimal encoding size for k in [1,32]" );
    } else if( k % 2 != 0 ) {
        // No palindromes for odd k
        return 0;
    }
    return utils::int_pow( 4, k / 2 );
}

// =================================================================================================
//     Reverse Complementing
// =================================================================================================

/**
 * @brief Set the reverse complement value in the given @p kmer.
 *
 * We store the RC in the kmer for efficiency when possible, but not all methods that create kmers
 * will be able to set it. For these, we offer this function to compute and set it from scratch.
 *
 * If the kmer already carries its own rc, we do nothing. This might be the case for certain
 * methods that set it on the fly in order to avoid re-computing it for every kmer.
 */
template<typename Tag>
void set_reverse_complement( Kmer<Tag>& kmer )
{
    // Function is written for a specific bit width.
    static_assert(
        std::is_same<typename Kmer<Tag>::Bitfield::WordType, std::uint64_t>::value,
        "Kmer::Bitfield::WordType != uint64_t"
    );
    static_assert(
        Kmer<Tag>::Bitfield::BITS_PER_CHAR == 2,
        "Kmer<Tag>::Bitfield::BITS_PER_CHAR != 2"
    );
    static_assert(
        Kmer<Tag>::Alphabet::NEGATE_IS_COMPLEMENT,
        "Kmer<Tag>::Alphabet::NEGATE_IS_COMPLEMENT != true"
    );
    using Bitfield = typename Kmer<Tag>::Bitfield;

    // Nothing to do if already set.
    // If the rc is AAAA, we cannot detect this, and compute it anyway below.
    if( kmer.rev_comp != 0 ) {
        return;
    }

    // Adapted from Kraken2 at https://github.com/DerrickWood/kraken2/blob/master/src/mmscanner.cc
    // which itself adapted this for 64-bit DNA use from public domain code at
    // https://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel

    // Reverse bits (leaving bit pairs intact, as those represent nucleotides):
    // Swap consecutive pairs, then nibbles, then bytes, then byte pairs, then halves of 64-bit word
    auto value = kmer.value;
    value = (( value & 0xCCCCCCCCCCCCCCCCUL ) >>  2) | (( value & 0x3333333333333333UL ) <<  2 );
    value = (( value & 0xF0F0F0F0F0F0F0F0UL ) >>  4) | (( value & 0x0F0F0F0F0F0F0F0FUL ) <<  4 );
    value = (( value & 0xFF00FF00FF00FF00UL ) >>  8) | (( value & 0x00FF00FF00FF00FFUL ) <<  8 );
    value = (( value & 0xFFFF0000FFFF0000UL ) >> 16) | (( value & 0x0000FFFF0000FFFFUL ) << 16 );
    value = ( value >> 32 ) | ( value << 32);

    // Finally, complement, and shift to correct position, removing the invalid lower bits.
    assert( Bitfield::BIT_WIDTH >= kmer.k() * Bitfield::BITS_PER_CHAR );
    value = ((~value) >> ( Bitfield::BIT_WIDTH - kmer.k() * Bitfield::BITS_PER_CHAR ));
    kmer.rev_comp = value;
}

/**
 * @brief Get the reverse complement of a given @p kmer.
 */
template<typename Tag>
Kmer<Tag> reverse_complement( Kmer<Tag> const& kmer )
{
    // Make the result, by flipping the original value and the rev_comp.
    auto result = kmer;
    set_reverse_complement( result );
    std::swap( result.value, result.rev_comp );
    assert( validate( result ));
    return result;
}

/**
 * @brief Turn the @p kmer into its canonical representation.
 *
 * This is the lexicographically smaller of the kmer and its reverse complement.
 */
template<typename Tag>
void make_canonical( Kmer<Tag>& kmer )
{
    // In case the rc is not set, compute it. Does nothing if already set.
    set_reverse_complement( kmer );

    // Now get the lexicographically smaller of the two. Technically, we are comparing this backwards
    // (due to the internal bit representation), but as one is the reverse of the other anyway,
    // that's identical to forward string lexicographical comparison!
    if( kmer.value > kmer.rev_comp ) {
        std::swap( kmer.value, kmer.rev_comp );
    }
}

/**
 * @brief Get the canonical representation of a given @p kmer.
 *
 * This is the lexicographically smaller of the kmer and its reverse complement.
 */
template<typename Tag>
Kmer<Tag> canonical_representation( Kmer<Tag> const& kmer )
{
    // Make the result, by flipping the original value and the rev_comp if needed.
    auto result = kmer;
    make_canonical( result );
    assert( validate( result ));
    return result;
}

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Validate a @p kmer by checking some basic properties.
 */
template<typename Tag>
bool validate( Kmer<Tag> const& kmer, bool throw_if_invalid = false )
{
    using Bitfield = typename Kmer<Tag>::Bitfield;
    bool valid = true;

    // Check k.
    valid &= ( kmer.k() > 0 && kmer.k() <= Bitfield::MAX_CHARS_PER_KMER );

    // Check that only the valid bits for the given k are set.
    valid &= (( kmer.value    & Bitfield::ones_mask[kmer.k()] ) == kmer.value );
    valid &= (( kmer.rev_comp & Bitfield::ones_mask[kmer.k()] ) == kmer.rev_comp );

    // Check that the reverse complement is correct.
    auto copy = Kmer<Tag>( kmer.value );
    set_reverse_complement( copy );
    valid &= ( kmer.rev_comp == copy.rev_comp || kmer.rev_comp == 0 );

    // Respond to the result of the check.
    if( ! valid && throw_if_invalid ) {
        throw std::runtime_error( "Invalid kmer" );
    }
    return valid;
}

} // namespace sequence
} // namespace genesis

#endif // include guard
