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
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector/functions.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/bit.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Functions
// =================================================================================================

// -------------------------------------------------------------------------
//     Creation
// -------------------------------------------------------------------------

std::vector<bool> make_bool_vector_from_indices( std::vector<size_t> const& indices, size_t size )
{
    // Get the largest element of the vector. If it's empty, we return an all-false vector.
    auto max_it = std::max_element( indices.begin(), indices.end() );
    if( max_it == indices.end() ) {
        return std::vector<bool>( size, false );
    }
    size_t target_size = *max_it + 1;
    if( size > 0 ) {
        if( target_size > size ) {
            throw std::invalid_argument(
                "Cannot use make_bool_vector_from_indices() with size " + std::to_string( size ) +
                " that is smaller than required to include the larged index " +
                std::to_string( *max_it ) + " in the list of indices (zero-based)."
            );
        }
        target_size = size;
    }

    // Fill a bool vector, setting all positions to true
    // that are indicated by the indices, pun intended.
    auto result = std::vector<bool>( target_size, false );
    for( auto const& idx : indices ) {
        assert( idx < result.size() );
        result[idx] = true;
    }
    return result;
}

Bitvector make_random_bitvector( size_t size )
{
    // Generate random unit64 values
    static std::random_device rd;
    static std::mt19937_64 engine(rd());
    static std::uniform_int_distribution<uint64_t> dist(
        std::numeric_limits<uint64_t>::min(),
        std::numeric_limits<uint64_t>::max()
    );
    static_assert( std::is_same<Bitvector::IntType, uint64_t>::value, "Bitvector::IntType != uint64_t" );

    // Iterate through all value types and assign random values.
    // Way faster than setting individual bits. We only need to unset the remainder in the end.
    auto result = Bitvector( size );
    for( auto& d : result.data() ) {
        d = dist(engine);
    }
    result.unset_padding_bits();
    return result;
}

// -------------------------------------------------------------------------
//     Modification
// -------------------------------------------------------------------------

void negate( Bitvector& bv )
{
    return bv.negate();
}

void invert( Bitvector& bv )
{
    return bv.negate();
}

void normalize( Bitvector& bv )
{
    if( bv.size() > 0 && bv.get(0) ) {
        bv.negate();
    }
}

// -------------------------------------------------------------------------
//     Hashing
// -------------------------------------------------------------------------

size_t bitvector_hash( Bitvector const& bv )
{
    // We factor in the size (in bits) of the bitvector,
    // so that two all-false bitvectors of different size
    // also have differing hashes.
    size_t seed = std::hash<size_t>()( bv.size() );
    for( auto word : bv.data() ) {
        seed = hash_combine( seed, word );
    }
    return seed;
}

Bitvector::IntType bitvector_x_hash( Bitvector const& bv )
{
    Bitvector::IntType seed = 0;
    for( auto word : bv.data() ) {
        seed ^= word;
    }
    return seed;
}

// -------------------------------------------------------------------------
//     Pop Count
// -------------------------------------------------------------------------

size_t pop_count( Bitvector const& bv )
{
    // Use bit trickery to count quickly.
    size_t res = 0;
    for( auto x : bv.data() ) {
        res += pop_count(x);
    }

    // safe, but slow version...
    //~ size_t tmp = 0;
    //~ for( size_t i = 0; i < size_; ++i ) {
        //~ if (get(i)) {
            //~ ++tmp;
        //~ }
    //~ }
    //~ assert(tmp == res);

    return res;
}

size_t pop_count( Bitvector const& bv, size_t first, size_t last )
{
    // Boundary checks.
    if( first >= bv.size() || last > bv.size() || first > last ) {
        throw std::invalid_argument(
            "Cannot compute pop count for Bitvector of size " + std::to_string( bv.size() ) +
            " within invalid range [" + std::to_string(first) + "," + std::to_string( last ) + ")"
        );
    }
    assert( first <  bv.size() );
    assert( last  <= bv.size() );
    assert( first <= last );

    // Check special case, as we might otherwise access invalid data at the boundaries.
    if( first == last ) {
        return 0;
    }
    assert( last > 0 );

    // We need to mask the first bits of the first word and last bits of the last word
    // before counting, and then can process the in-between words normally.
    // If first and last are the same word, we need special treatment as well.

    // Get word indices, and bit position indices within those words.
    // The last word is the one where the bit before last is, as last is past-the-end.
    // However, the bit index is still meant to be past-the-end, to use the proper mask.
    auto const f_wrd_idx = first / Bitvector::IntSize;
    auto const l_wrd_idx = (last - 1) / Bitvector::IntSize;
    auto const f_bit_idx = first % Bitvector::IntSize;
    auto const l_bit_idx = last % Bitvector::IntSize;
    assert( f_wrd_idx < bv.data().size() );
    assert( l_wrd_idx < bv.data().size() );
    assert( f_bit_idx < Bitvector::ONES_MASKS.size() );
    assert( l_bit_idx < Bitvector::ONES_MASKS.size() );

    // Get the two words at the boundary. We later check if they are the same,
    // so we do not repeat the code here, and treat the special case later.
    auto f_word = bv.data()[ f_wrd_idx ];
    auto l_word = bv.data()[ l_wrd_idx ];

    // Mask out the beginning and end, respectively.
    // Remove all bits before the first index, and all bits after and including the last index.
    // No special case needed here, as the 0th mask is idempotent.
    // That's because it's the mask that we also use for unset_padding_bits(),
    // we are basically doing the same here.
    f_word &= ~Bitvector::ONES_MASKS[ f_bit_idx ];
    if( l_bit_idx != 0 ) {
        l_word &= Bitvector::ONES_MASKS[ l_bit_idx ];
    }

    // Finally, count up all the parts.
    size_t result = 0;
    if( f_wrd_idx == l_wrd_idx ) {
        // Same word. Mask out the bits we don't want, using only the bits that remained after
        // filtering both words (which are the same, just different ends of the word), then count.
        result = pop_count( f_word & l_word );
    } else {
        // Count the first and last word, and then add everything in between the two.
        result = pop_count( f_word ) + pop_count( l_word );
        for( size_t i = f_wrd_idx + 1; i < l_wrd_idx; ++i ) {
            result += pop_count( bv.data()[i] );
        }
    }
    return result;
}

// -------------------------------------------------------------------------
//     Set Operators
// -------------------------------------------------------------------------

Bitvector set_minus( Bitvector const& lhs, Bitvector const& rhs )
{
    // Not efficient, as it creates temporary vectors...
    // but not needed in any performance relevant way at the moment,
    // so let's keep it like this for now.
    return lhs & (~rhs);
}

Bitvector symmetric_difference( Bitvector const& lhs, Bitvector const& rhs )
{
    // Symmetric difference is simply xor.
    // We provide this function mostly for semamtics... not really used.
    return lhs ^ rhs;
    // return (lhs | rhs) & ~(lhs & rhs);
}

bool is_subset( Bitvector const& sub, Bitvector const& super )
{
    // Sanity check
    if( sub.size() != super.size() ) {
        throw std::invalid_argument(
            "Cannot compute sub/super set between Bitvectors of different size"
        );
    }

    // Shorthands
    auto const& sub_data = sub.data();
    auto const& super_data = super.data();

    // Non-strict subset: Every bit in sub must also be set in super.
    for( size_t i = 0; i < sub_data.size(); ++i ) {
        // If sub has any bit not set in super,
        // then sub is not a subset, and we can return early.
        if( sub_data[i] & ~super_data[i] ) {
            return false;
        }
    }
    return true;
}

bool is_superset( Bitvector const& super, Bitvector const& sub )
{
    return is_subset( sub, super );
}

bool is_strict_subset( Bitvector const& sub, Bitvector const& super )
{
    // Sanity check
    if( sub.size() != super.size() ) {
        throw std::invalid_argument(
            "Cannot compute sub/super set between Bitvectors of different size"
        );
    }

    // Shorthands
    auto const& sub_data = sub.data();
    auto const& super_data = super.data();

    // Strict subset: sub must be a subset of super, and super must have
    // at least one extra bit, i.e., the two vectors are not exactly equal.
    // This is the same as the above is_subset(), but also includes an extra check
    // for inequality. It is better to have some code duplication here,
    // as this allows us to traverse each vector only once, for cache locality.
    bool found_extra_bit = false;
    for( size_t i = 0; i < sub_data.size(); ++i ) {
        // If sub contains a bit that super does not,
        // sub cannot be a subset, and we can return early.
        if( sub_data[i] & ~super_data[i] ) {
            return false;
        }

        // Check if super has any bit that sub does not.
        // We already know that the sub/super set property is met,
        // so if they are inequal, then sub contains extra bits.
        found_extra_bit |= ( sub_data[i] != super_data[i] );
        // found_extra_bit |= ( super_data[i] & ~sub_data[i] );
        // if( !found_extra_bit && ( super_data[i] & ~sub_data[i] )) {
        //     found_extra_bit = true;
        // }
    }
    return found_extra_bit;
}

bool is_strict_superset( Bitvector const& super, Bitvector const& sub )
{
    return is_strict_subset( sub, super );
}

// -------------------------------------------------------------------------
//     Distances
// -------------------------------------------------------------------------

double jaccard_similarity( Bitvector const& lhs, Bitvector const& rhs )
{
    if( lhs.size() != rhs.size() ) {
        throw std::invalid_argument(
            "Cannot compute Jaccard similarity between Bitvectors of different size"
        );
    }

    // Shorthands
    auto const& lhs_data = lhs.data();
    auto const& rhs_data = rhs.data();

    // Use the bitvector data directly to count
    // the number of bits in the intersection and the union
    size_t sum_i = 0;
    size_t sum_u = 0;
    for( size_t i = 0; i < lhs_data.size(); ++i ) {
        sum_i += pop_count( lhs_data[i] & rhs_data[i] );
        sum_u += pop_count( lhs_data[i] | rhs_data[i] );
    }

    // Alternative (10x slower) way using operations on the whole bitvectors.
    // auto const sum_i = pop_count( lhs & rhs );
    // auto const sum_u = pop_count( lhs | rhs );

    // Compute the index, taking care of the edge case.
    if( sum_u == 0 ) {
        assert( sum_i == 0 );
        return 0.0;
    }
    return static_cast<double>( sum_i ) / static_cast<double>( sum_u );
}

double jaccard_distance( Bitvector const& lhs, Bitvector const& rhs )
{
    return 1.0 - jaccard_similarity( lhs, rhs );
}

size_t hamming_distance( Bitvector const& lhs, Bitvector const& rhs )
{
    if( lhs.size() != rhs.size() ) {
        throw std::invalid_argument(
            "Cannot compute Hamming distance between Bitvectors of different size"
        );
    }

    // Shorthands
    auto const& lhs_data = lhs.data();
    auto const& rhs_data = rhs.data();

    // Use the bitvector data directly to count the number of resulting bits.
    size_t sum = 0;
    for( size_t i = 0; i < lhs_data.size(); ++i ) {
        sum += pop_count( lhs_data[i] ^ rhs_data[i] );
    }
    return sum;
}

// -------------------------------------------------------------------------
//     Find Set Bits
// -------------------------------------------------------------------------

bool all_set( Bitvector const& bv )
{
    // Edge case. An empty bitvector has all bits set. Ex falso quodlibet.
    if( bv.empty() ) {
        return true;
    }

    // Check if all words (except for the last) are one. If not, we can stop early.
    // Due to padding, we need to check the last word differently.
    auto const& data = bv.data();
    for( size_t i = 0; i < data.size() - 1; ++i ) {
        if( data[i] != Bitvector::ALL_1 ) {
            return false;
        }
    }

    // If there is a non-zero mask, we have padding, and check this.
    // All bits are set if the padding bits are equal to the mask.
    auto const mask = bv.get_padding_mask();
    if( mask ) {
        assert( bv.size() % Bitvector::IntSize != 0 );
        return data.back() == mask;
    }

    // Otherwise, it's a vector without padding, i.e., exactly a multiple of 64.
    // In that case, the last word of the vector needs to be all 1.
    assert( bv.size() % Bitvector::IntSize == 0 );
    return data.back() == Bitvector::ALL_1;
}

bool all_unset( Bitvector const& bv )
{
    // Here, we do not need to take care of padding, as it is zero as well.
    for( auto word : bv.data() ) {
        if( word != Bitvector::ALL_0 ) {
            return false;
        }
    }
    return true;
}

bool any_set( Bitvector const& bv )
{
    return ! all_unset( bv );
}

bool any_unset( Bitvector const& bv )
{
    return ! all_set( bv );
}

bool none_set( Bitvector const& bv )
{
    return all_unset( bv );
}

size_t find_first_set( Bitvector const& bv )
{
    return find_next_set( bv, 0 );
}

size_t find_last_set( Bitvector const& bv )
{
    auto const& data = bv.data();
    if( data.size() == 0 ) {
        return Bitvector::npos;
    }

    // We currently do not have a find_prev_set() function. If we implement one at some point,
    // this function here should instead simply call that one. For now however, we implement
    // only what we need here.

    // Find the last word that is non-zero. We use that unsigned ints have defined underflow
    // behavior, so if we wrap around at word index 0, we just go to int max, and stop.
    // This also works for the boundary case of an empty bitvector.
    size_t wrd_idx = data.size() - 1;
    while( wrd_idx < data.size() && data[wrd_idx] == 0 ) {
        --wrd_idx;
    }

    // Boundary condition: No bits set, or empty vector
    if( wrd_idx >= data.size() ) {
        return Bitvector::npos;
    }

    // Now find the last bit in the word. For now, we do a simple bit-based loop,
    // but we could speed this up with a similar ctz/clz approach as in find_next_set().
    // We start at either the end position of the word index that we found above,
    // or, if that's the last word of the vector, we start at its last bit.
    assert( wrd_idx < data.size() );
    assert( data[wrd_idx] != 0 );
    auto bit_idx = std::min( wrd_idx * Bitvector::IntSize + Bitvector::IntSize - 1, bv.size() - 1 );
    while( bit_idx < data.size() * Bitvector::IntSize ) {
        if( bv.get(bit_idx) ) {
            return bit_idx;
        }
        --bit_idx;
    }

    // We must have found a bit above.
    throw std::runtime_error( "Internal error in Bitvector::find_last_set()" );
}

size_t find_next_set( Bitvector const& bv, size_t start )
{
    // Boundary check
    if( start >= bv.size() ) {
        // We mimic the behaviour of std::string::find(), which just never finds anything
        // when used beyond the string, but also does not throw an exception in such cases.
        return Bitvector::npos;

        // Alternative: Throw an exception.
        // throw std::invalid_argument(
        //     "Invalid call to find_next_set() with start==" + std::to_string(start) +
        //     " and a Bitvector of size " + std::to_string( size_ )
        // );
    }

    // Helper function to find the index of the first set bit in a non-zero word.
    auto find_next_set_in_word_ = []( Bitvector::IntType word ) {
        assert( word != 0 );

        // We use ffs here, see https://man7.org/linux/man-pages/man3/ffs.3.html
        // It returns the _position_ of the bit, so we need to subtract 1 to get the index.
        // Alternatively, we could use __builtin_ctz, which returns the number of trailing
        // zeros in the given word, but is a compiler intrinsic, so let's stay with POSIX.

        // Check the size of the input and call the appropriate ffs function.
        // Any good compiler will see through this and make this constexpr.
        // In C++17, we could do this ourselves ;-)
        if( sizeof(word) <= sizeof(unsigned int) ) {
            return ffs( static_cast<unsigned int>( word )) - 1;
        } else if( sizeof(word) <= sizeof(unsigned long) ) {
            return ffsl( static_cast<unsigned long>( word )) - 1;
        } else {
            return ffsll(word) - 1;
        }
    };

    // First see if there is anything in the word at the start position.
    // We assume that this function might be called on a dense bitvector,
    // where the given position is already set, so we check that as a shortcut.
    if( bv.get( start )) {
        return start;
    }

    // If that did not work, we see if there is anything in the current word.
    auto wrd_idx = start / Bitvector::IntSize;
    auto bit_idx = start % Bitvector::IntSize;
    assert( wrd_idx < bv.data().size() );
    assert( bit_idx < Bitvector::ONES_MASKS.size() );
    auto word = bv.data()[wrd_idx];

    // For this, we remove the bits before start, and then test the rest.
    // Mask out the beginning of the word, and find the next bit on the remainder.
    // Remove all bits before the first index.
    word &= ~( Bitvector::ONES_MASKS[ bit_idx ]);
    if( word != 0 ) {
        return wrd_idx * Bitvector::IntSize + find_next_set_in_word_( word );
    }

    // We did not find a bit in the word of the start. So now we look for the first word
    // after the start one that has bits set, and return its first set bit position.
    ++wrd_idx;
    while( wrd_idx < bv.data().size() && bv.data()[wrd_idx] == 0 ) {
        ++wrd_idx;
    }
    if( wrd_idx == bv.data().size() ) {
        return Bitvector::npos;

    }
    assert( wrd_idx < bv.data().size() );
    assert( bv.data()[wrd_idx] != 0 );
    return wrd_idx * Bitvector::IntSize + find_next_set_in_word_( bv.data()[wrd_idx] );
}

// -------------------------------------------------------------------------
//     Sorting
// -------------------------------------------------------------------------

// bool lexicographically_compare_helper_( Bitvector const& lhs, Bitvector const& rhs, bool on_equal )
// {
//     // Deactivated at the moment, as this does not take care of the typical little endian-ness
//     // of modern computers, and hence yields wrong results...
//
//     // Local helper function to avoid code duplication.
//     if( lhs.size() != rhs.size() ) {
//         throw std::runtime_error(
//             "Cannot use lexicographical comparison functions on Bitvectors of different size."
//         );
//     }
//     for( size_t i = 0; i < lhs.data().size(); ++i ) {
//         if( lhs.data()[i] < rhs.data()[i] ) {
//             return true;
//         } else if( lhs.data()[i] > rhs.data()[i] ) {
//             return false;
//         }
//     }
//
//     // If we are here, all of the above comparisons shows that lhs == rhs.
//     assert( lhs == rhs );
//     return on_equal;
// }
//
// bool is_lexicographically_less( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( lhs, rhs, false );
// }
//
// bool is_lexicographically_less_or_equal( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( lhs, rhs, true );
// }
//
// bool is_lexicographically_greater( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( rhs, lhs, false );
// }
//
// bool is_lexicographically_greater_or_equal( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( rhs, lhs, true );
// }

} // namespace utils
} // namespace genesis
