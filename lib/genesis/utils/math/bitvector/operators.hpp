#ifndef GENESIS_UTILS_MATH_BITVECTOR_OPERATORS_H_
#define GENESIS_UTILS_MATH_BITVECTOR_OPERATORS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector.hpp"

#include <iosfwd>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Operators
// =================================================================================================

/**
 * @brief Take the bitwise `and` of two Bitvector%s of potentially different size.
 *
 * The function is the same as normal `and`, but operates only on the number of bits of
 * the shorter Bitvector, that is,
 * it operates on bits `[ 0 , m )` with `m = min( lhs.size(), rhs.size() )`.
 */
Bitvector bitwise_and (Bitvector const& lhs, Bitvector const& rhs);

/**
* @brief Take the bitwise `or` of two Bitvector%s of potentially different size.
*
* The function is the same as normal `or`, but operates only on the number of bits of
* the shorter Bitvector, that is,
* it operates on bits `[ 0 , m )` with `m = min( lhs.size(), rhs.size() )`.
*/
Bitvector bitwise_or (Bitvector const& lhs, Bitvector const& rhs);

/**
* @brief Take the bitwise `xor` of two Bitvector%s of potentially different size.
*
* The function is the same as normal `xor`, but operates only on the number of bits of
* the shorter Bitvector, that is,
* it operates on bits `[ 0 , m )` with `m = min( lhs.size(), rhs.size() )`.
*/
Bitvector bitwise_xor (Bitvector const& lhs, Bitvector const& rhs);

Bitvector set_minus (Bitvector const& lhs, Bitvector const& rhs);
Bitvector symmetric_difference (Bitvector const& lhs, Bitvector const& rhs);

/**
 * @brief Strict subset.
 */
bool is_strict_subset( Bitvector const& sub, Bitvector const& super );

/**
 * @brief Strict superset.
 */
bool is_strict_superset( Bitvector const& super, Bitvector const& sub );

/**
 * @brief Subset or equal.
 */
bool is_subset( Bitvector const& sub, Bitvector const& super );

/**
 * @brief Superset or equal.
 */
bool is_superset( Bitvector const& super, Bitvector const& sub );

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

/**
 * @brief Insertion operator that outputs a Bitvector as a string of '0's and '1's.
 */
std::ostream& operator << ( std::ostream& out, Bitvector const& bv );

/**
* @brief Extraction operator that inputs a Bitvector from a string of '0's and '1's,
* and stops at the first char that is not '0' or '1'.
*/
std::istream& operator >> ( std::istream& in, Bitvector& bv );

} // namespace utils
} // namespace genesis

#endif // include guard
