#ifndef GENESIS_UTILS_MATH_RANDOM_H_
#define GENESIS_UTILS_MATH_RANDOM_H_

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

#include <cstddef>
#include <cstdint>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Fast Randomness
// =================================================================================================

/**
 * @brief Fast random number generator for 32bit integers.
 *
 * See https://en.wikipedia.org/wiki/Permuted_congruential_generator for details.
 * See permuted_congruential_generator_init() to set the seed.
 */
uint32_t permuted_congruential_generator();

/**
 * @brief Fast random number generator for 32bit integers, for numbers in the range `[0, max]`.
 *
 * Note that the interval is closed, i.e., @p max is inclusive.
 */
uint32_t permuted_congruential_generator( uint32_t max );

/**
 * @brief Fast random number generator for 32bit integers, for numbers in the range `[min, max]`.
 *
 * Note that the interval is closed, i.e., @p max is inclusive.
 */
uint32_t permuted_congruential_generator( uint32_t min, uint32_t max );

/**
 * @brief Fast random number generator for 32bit integers, for bool with 0.5 probability.
 */
bool permuted_congruential_generator_bool();

/**
 * @brief Fast random number generator for 32bit integers, for bool with a given chance.
 *
 * This is returning a random bool, with a 1 in @p chance_one_in possibility to be `true`.
 * For instance, calling the function with `chance_one_in == 5` means a 1 in 5 chance,
 * or probability of 0.2 for a `true` result.
 */
bool permuted_congruential_generator_bool( uint32_t chance_one_in );

/**
 * @brief Set the seed for permuted_congruential_generator().
 */
void permuted_congruential_generator_init( uint64_t seed );

// =================================================================================================
//     Sampling
// =================================================================================================

/**
 * @brief Select @p k many unique numbers out of the range `[ 0, n )`.
 *
 * In other words, select @p k numbers out of the sequence of @p n numbers, without replacement.
 *
 * The implementation is based on Algorithm 3.4.2S of Knuth's book Seminumeric Algorithms.
 *
 * @param k Desired sample size.
 * @param n Total population size.
 * @return  List of @p k unique numbers out of the range `[ 0, n )`.
 */
std::vector<size_t> select_without_replacement( size_t k, size_t n );

} // namespace utils
} // namespace genesis

#endif // include guard
