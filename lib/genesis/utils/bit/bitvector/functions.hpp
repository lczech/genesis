#ifndef GENESIS_UTILS_BIT_BITVECTOR_FUNCTIONS_H_
#define GENESIS_UTILS_BIT_BITVECTOR_FUNCTIONS_H_

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

#include "genesis/utils/bit/bitvector.hpp"
#include "genesis/utils/core/std.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Functions
// =================================================================================================

// -------------------------------------------------------------------------
//     Creation
// -------------------------------------------------------------------------

/**
 * @brief Helper function to create a bool vector from a set of indices to be set to `true`.
 *
 * The function expectes a list of indices. It returns a bool vector with the size of the largest
 * index, or the provided @p size (if set to a value > 0), where all positions of these indices are
 * `true`, and all other positions are `false`.
 */
std::vector<bool> make_bool_vector_from_indices(
    std::vector<size_t> const& indices,
    size_t size = 0
);

/**
 * @brief Create a Bitvector of a given @p size, with randomly initialized bits, mostly for testing.
 */
Bitvector make_random_bitvector( size_t size );

// -------------------------------------------------------------------------
//     Modification
// -------------------------------------------------------------------------

/**
* @brief Flip all bits. Alias for invert().
*/
void negate( Bitvector& bv );

/**
* @brief Flip all bits. Alias for negate().
*/
void invert( Bitvector& bv );

/**
* @brief Bring the Bitvector in a normalized form, where the first bit is always zero.
*
* If the first bit is zero, nothing happens. However, if is is one, the whole Bitvector is flipped
* using negate().
*/
void normalize( Bitvector& bv );

// -------------------------------------------------------------------------
//     Hashing
// -------------------------------------------------------------------------

/**
* @brief Return an std::hash value for the Bitvector.
*/
size_t bitvector_hash( Bitvector const& bv );

/**
* @brief Return a hash value of type IntType that is quicker to calculate than hash().
*
* This can be used for obtaining a simple hash using xor of the words.
* The avalanche effect is of course not present, but for many applications, this hash is
* good enough and quite useful.
*/
Bitvector::IntType bitvector_x_hash( Bitvector const& bv );

// -------------------------------------------------------------------------
//     Pop Count
// -------------------------------------------------------------------------

/**
 * @brief Count the number of set bits in the Bitvector, that is, its Hamming weight,
 * or population count (popcnt).
 */
size_t pop_count( Bitvector const& bv );

/**
 * @brief Count the number of set bits between a range of indices in the Bitvector,
 * that is, its Hamming weight, or population count (popcnt), for that range.
 *
 * The range @p first to @p last is zero-based, with last being the past-the-end index.
 * Hence, this is the same as
 *
 *     size_t count = 0;
 *     for( size_t i = first; i < last; ++i ) {
 *         if( bitvector.get( i )) {
 *             ++count;
 *         }
 *     }
 *
 * but faster, as we use whole-word counting internally.
 */
size_t pop_count( Bitvector const& bv, size_t first, size_t last );

// -------------------------------------------------------------------------
//     Find Set Bits
// -------------------------------------------------------------------------

/**
 * @brief Return if all bits are set, i.e., the Bitvector is all one.
 */
bool all_set( Bitvector const& bv );

/**
 * @brief Return if all bits are unset, i.e., the Bitvector is all zero.
 *
 * Alias for none_set()
 */
bool all_unset( Bitvector const& bv );

/**
 * @brief Return if any bits are set, i.e., the Bitvector is not all zero.
 */
bool any_set( Bitvector const& bv );

/**
 * @brief Return if any bits are unset, i.e., the Bitvector is not all one.
 */
bool any_unset( Bitvector const& bv );

/**
 * @brief Return if no bits are set, i.e., the Bitvector is all zero.
 *
 * Alias for all_unset()
 */
bool none_set( Bitvector const& bv );

/**
 * @brief Return the index of the first bit in the Bitvector that is set.
 *
 * If no such position exists (because all bits are `false`), Bitvector::npos
 * is returned.
 */
size_t find_first_set( Bitvector const& bv );

/**
 * @brief Return the index of the last bit in the Bitvector that is set.
 *
 * If no such position exists (because all bits are `false`), Bitvector::npos
 * is returned.
 */
size_t find_last_set( Bitvector const& bv );

/**
 * @brief Return the index of the next position in the Bitvector that is set.
 *
 * This returns the first position starting at @p start, including @p start itself, that is set.
 * If no such position exists (because all following bits are `false`), or if @p start is beyond
 * the length of the vector, Bitvector::npos is returned instead.
 */
size_t find_next_set( Bitvector const& bv, size_t start );

/**
 * @brief Call a function for every bit position that is set in the @p bitvector
 *
 * The callback receives the overall bit position.
 */
void for_each_set_bit( Bitvector const& bitvector, std::function<void(size_t)> const& callback );

// -------------------------------------------------------------------------
//     Set Operators
// -------------------------------------------------------------------------

/**
 * @brief Compute the set minus `lhs & (~rhs)` between two Bitvector%s.
 */
Bitvector set_minus( Bitvector const& lhs, Bitvector const& rhs );

/**
 * @brief Compute the symmetric differeence `(lhs | rhs) & ~(lhs & rhs)` between two Bitvector%s.
 */
Bitvector symmetric_difference( Bitvector const& lhs, Bitvector const& rhs );

/**
 * @brief Subset or equal.
 */
bool is_subset( Bitvector const& sub, Bitvector const& super );

/**
 * @brief Superset or equal.
 */
bool is_superset( Bitvector const& super, Bitvector const& sub );

/**
 * @brief Strict subset.
 */
bool is_strict_subset( Bitvector const& sub, Bitvector const& super );

/**
 * @brief Strict superset.
 */
bool is_strict_superset( Bitvector const& super, Bitvector const& sub );

// -------------------------------------------------------------------------
//     Distances
// -------------------------------------------------------------------------

/**
 * @brief Compute the Jaccard index (Jaccard similarity coefficient) for two Bitvector%s
 * of the same size.
 *
 * This is simply the count of bits in the intersection divided by the count of bits in the union
 * of the Bitvectors.
 */
double jaccard_similarity( Bitvector const& lhs, Bitvector const& rhs );

/**
 * @brief Compute the Jaccard distance for two Bitvector%s of the same size.
 *
 * This dissimilarity is simply 1 - jaccard_similarity().
 */
double jaccard_distance( Bitvector const& lhs, Bitvector const& rhs );

/**
 * @brief Compute the Hamming distance between two Bitvector%s,
 * i.e., the Hamming weight (pop count) of the `xor` of the inputs.
 */
size_t hamming_distance( Bitvector const& lhs, Bitvector const& rhs );

// -------------------------------------------------------------------------
//     Sorting
// -------------------------------------------------------------------------

// /* *
//  * @brief Return whether @p lhs is lexicographically less than @p rhs.
//  *
//  * This function consideres the bits ordered with most significant bits towards the left, so that
//  * `0001 < 1000` for example. Both Bitvector%s have to have the same length (this could be
//  * implemented for Bitvectors of different size, but that is not needed as of now, so it isn't).
//  */
// bool is_lexicographically_less( Bitvector const& lhs, Bitvector const& rhs );
//
// /* *
//  * @brief Return whether @p lhs is lexicographically less than @p rhs, or equal to it.
//  *
//  * @copydetails is_lexicographically_less()
//  */
// bool is_lexicographically_less_or_equal( Bitvector const& lhs, Bitvector const& rhs );
//
// /* *
//  * @brief Return whether @p lhs is lexicographically greater than @p rhs.
//  *
//  * @copydetails is_lexicographically_less()
//  */
// bool is_lexicographically_greater( Bitvector const& lhs, Bitvector const& rhs );
//
// /* *
//  * @brief Return whether @p lhs is lexicographically greater than @p rhs, or equal to it.
//  *
//  * @copydetails is_lexicographically_less()
//  */
// bool is_lexicographically_greater_or_equal( Bitvector const& lhs, Bitvector const& rhs );

} // namespace utils
} // namespace genesis

#endif // include guard
