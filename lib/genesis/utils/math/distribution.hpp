#ifndef GENESIS_UTILS_MATH_DISTRIBUTION_H_
#define GENESIS_UTILS_MATH_DISTRIBUTION_H_

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
//     Multinomial Distribution
// =================================================================================================

/**
 * @brief Select a random sample following a multinomial distribution.
 *
 * This function computes a random sample formed by @p n trials from an underlying distribution @p p.
 * The values in @p have to be non-negative, and are taken as weights of each category; they do not
 * need to be normalized to sum to 1. The function models drawing from multiple categories, each
 * with probability as given in @p p, with replacement.
 * See multivariate_hypergeometric_distribution() for the equivalent distribution to draw
 * _without_ replacement instead.
 *
 * See https://en.wikipedia.org/wiki/Multinomial_distribution for a description of the distribution.
 * See https://www.gnu.org/software/gsl/doc/html/randist.html#the-multinomial-distribution for
 * details on the distribution and the used algorithm, which follows C.S. Davis, "The computer
 * generation of multinomial random variates", Comp. Stat. Data Anal. 16 (1993) 205–217.
 */
std::vector<size_t> multinomial_distribution( std::vector<size_t> const& p, size_t n );

/**
 * @brief Select a random sample following a multinomial distribution.
 *
 * This overload accepts a `std::vector<double>`, and checks that all values are finite and
 * non-negative. Otherwise the same.
 */
std::vector<size_t> multinomial_distribution( std::vector<double> const& p, size_t n );

/**
 * @brief Select a random sample from a hypergeometric distribution.
 *
 * This is the distribution of drawing balls without replacement from an urn with two colors.
 * Here, @p n1 and @p n2 are the numbers of balls of each color, and @p t is the number of balls
 * to sample, and the return value is the number of balls of the first color.
 * See https://en.wikipedia.org/wiki/Hypergeometric_distribution
 */
size_t hypergeometric_distribution( size_t n1, size_t n2, size_t t );

/**
 * @brief Select a random sample following a multivariate hypergeometric distribution.
 *
 * This function computes a random sample formed by @p n trials from an underlying distribution @p p.
 * The values in @p have to be non-negative, and are taken as weights of each category; they do not
 * need to be normalized to sum to 1. The function models drawing from multiple categories, each
 * with weights as given in @p p, without replacement.
 * See multinomial_distribution() for the equivalent distribution to draw _with_ replacement instead.
 */
std::vector<size_t> multivariate_hypergeometric_distribution( std::vector<size_t> const& p, size_t n );

} // namespace utils
} // namespace genesis

#endif // include guard
