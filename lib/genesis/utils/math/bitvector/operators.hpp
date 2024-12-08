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

#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/functions.hpp"
#include "genesis/utils/io/deserializer.hpp"
#include "genesis/utils/io/serializer.hpp"

#include <cstddef>
#include <iosfwd>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Operators
// =================================================================================================

// -------------------------------------------------------------------------
//     Bit Operators
// -------------------------------------------------------------------------

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

// -------------------------------------------------------------------------
//     Input and Output
// -------------------------------------------------------------------------

/**
 * @brief Print the bits of a Bitvector to a string.
 *
 * If @p with_size is set, the string is prefixed by the size of the string in square brackets,
 * such as `[5] 01011`, which is useful for debugging.
 */
std::string to_bit_string( Bitvector const& bv, bool with_size = false );

/**
 * @brief Insertion operator that outputs a Bitvector as a string of '0's and '1's.
 */
std::ostream& operator << ( std::ostream& out, Bitvector const& bv );

/**
* @brief Extraction operator that inputs a Bitvector from a string of '0's and '1's,
* and stops at the first char that is not '0' or '1'.
*/
std::istream& operator >> ( std::istream& in, Bitvector& bv );

/**
 * @brief Serialize a Bitvector to a binary target.
 */
Serializer& operator<<( Serializer& serializer, Bitvector const& bv );

/**
 * @brief Deserialize a Bitvector from a binary source.
 */
Deserializer& operator>>( Deserializer& deserializer, Bitvector& bv );

/**
 * @brief Get the size in the binary output of a serialized Bitvector using Serializer,
 * given the number of bits being serialized.
 */
inline size_t serialized_bitvector_size( size_t bit_size )
{
    // We need its size in bits, its vector size, as well as the data itself.
    size_t total = 2 * sizeof( size_t );
    total += Bitvector::get_vector_size( bit_size ) * sizeof( Bitvector::IntType );
    return total;
}

/**
 * @brief Get the size in the binary output of a serialized Bitvector using Serializer.
 */
inline size_t serialized_bitvector_size( Bitvector const& bv )
{
    return serialized_bitvector_size( bv.size() );
}

// -------------------------------------------------------------------------
//     Hashing
// -------------------------------------------------------------------------

/**
 * @brief Helper structure that yields the hash of a given Bitvector.
 *
 * It is meant to be used in containers such as `unordered_set` or `unordered_map`
 * that can make use of custom hash functions for the key objects. By default,
 * these containers use a specialization of the `std::hash` template, which we also offer,
 * and that also uses the Bitvector::hash() function.
 *
 * Hence, this class here is slightly redundant, as it gives the same result as just using
 * the `std::hash` specialization. Still, it might be useful to have.
 *
 * See also BitvectorXhash for an alternative version that uses Bitvector::x_hash() instead.
 */
struct BitvectorHash
{
    std::size_t operator() ( genesis::utils::Bitvector const& value ) const
    {
        return genesis::utils::bitvector_hash(value);
    }
};

/**
 * @brief Helper structure that yields the x_hash of a given Bitvector.
 *
 * It is meant to be used in containers such as `unordered_set` or `unordered_map`
 * that can make use of custom hash functions for the key objects. Using this class instead
 * of the standard `std::hash` specialization, the Bitvector::x_hash() function is used instead
 * of the standard hash() function. It is hence faster to compute, but without avalanche effect.
 *
 * In some use cases, this might be preferrable - we however recommend to test this, in order to
 * make sure that colliding hashes do not slow down the performance in the end.
 *
 * Note that the function needs to cast from Bitvector::IntType to std::size_t.
 * On most modern systems, these are expecte to be the same, i.e., 64 bit unsigned integers.
 * However, this might cause problem on systems where this is not the case.
 */
struct BitvectorXhash
{
    std::size_t operator() ( genesis::utils::Bitvector const& value ) const
    {
        return static_cast<std::size_t>( bitvector_x_hash( value ));
    }
};

} // namespace utils
} // namespace genesis

// =============================================================================
//     Namespace std extension
// =============================================================================

namespace std {

/**
 * @brief Specialization of std::hash for the Bitvector class.
 *
 * It uses Bitvector::hash() for the hashing. See also BitvectorHash for an alternative class
 * that does the same, but resides in the same namespace as Bitvector, and see BitvectorXhash
 * for a variant that uses Bitvector::x_hash() instead as the hashing function.
 */
template<>
struct hash<genesis::utils::Bitvector>
{
    std::size_t operator() ( genesis::utils::Bitvector const& value ) const
    {
        return genesis::utils::bitvector_hash(value);
    }
};

} // namespace std

#endif // include guard
