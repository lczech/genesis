#ifndef GENESIS_UTILS_MATH_BITVECTOR_OPERATORS_H_
#define GENESIS_UTILS_MATH_BITVECTOR_OPERATORS_H_

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
 * @brief Policy to decide how to combine Bitvector%s of different lengths.
 *
 * This is used in bitwise_and(), bitwise_or(), and bitwise_xor(). For these functions, when
 * applying them to Bitvector%s of different lengths, we need to decide which length to use.
 */
enum class BitwiseOperatorLengthPolicy
{
    /**
     * @brief Expect both Bitvector%s to be of equal length, throw an exception otherwise.
     */
    kExpectEqual,

    /**
     * @brief Use the length of the shorter of the two Bitvector%s.
     *
     * Operate on bits `[ 0 , m )` with `m = min( lhs.size(), rhs.size() )`.
     * This truncates any remaining bits of the longer Bitvector.
     */
    kUseShorter,

    /**
     * @brief Use the length of the longer of the two Bitvector%s.
     *
     * Operate on bits `[ 0 , m )` with `m = max( lhs.size(), rhs.size() )`.
     * This behaves as if the shorter Bitvector was filled with 0 to the longer length.
     */
    kUseLonger,

    /**
     * @brief Use the length of the first (left hand side) Bitvector of the operator.
     *
     * The other Bitvector (second, right hand side) is either truncated or filled with zeros
     * to match the length.
     */
    kUseFirst,

    /**
     * @brief Use the length of the second (right hand side) Bitvector of the operator.
     *
     * The other Bitvector (first, left hand side) is either truncated or filled with zeros
     * to match the length.
     */
    kUseSecond
};

/**
 * @brief Take the bitwise `and` of two Bitvector%s of potentially different size.
 *
 * The function is the same as the normal version, but allows to use Bitvector%s of different sizes.
 * By default (`policty == kExpectEqual`), the vectors are expected to be of equal length.
 * See BitwiseOperatorLengthPolicy for other choices of the policy.
 */
Bitvector bitwise_and(
    Bitvector const& lhs, Bitvector const& rhs,
    BitwiseOperatorLengthPolicy length_policy = BitwiseOperatorLengthPolicy::kExpectEqual
);

/**
* @brief Take the bitwise `or` of two Bitvector%s of potentially different size.
*
* The function is the same as the normal version, but allows to use Bitvector%s of different sizes.
* By default (`policty == kExpectEqual`), the vectors are expected to be of equal length.
* See BitwiseOperatorLengthPolicy for other choices of the policy.
*/
Bitvector bitwise_or(
    Bitvector const& lhs, Bitvector const& rhs,
    BitwiseOperatorLengthPolicy length_policy = BitwiseOperatorLengthPolicy::kExpectEqual
);

/**
* @brief Take the bitwise `xor` of two Bitvector%s of potentially different size.
*
* @copydetails bitwise_or()
*/
Bitvector bitwise_xor(
    Bitvector const& lhs, Bitvector const& rhs,
    BitwiseOperatorLengthPolicy length_policy = BitwiseOperatorLengthPolicy::kExpectEqual
);

/**
 * @brief Compute the set minus `lhs & (~rhs)` between two Bitvector%s.
 */
Bitvector set_minus (Bitvector const& lhs, Bitvector const& rhs);

/**
 * @brief Compute the symmetric differeence `(lhs | rhs) & ~(lhs & rhs)` between two Bitvector%s.
 */
Bitvector symmetric_difference( Bitvector const& lhs, Bitvector const& rhs );

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
