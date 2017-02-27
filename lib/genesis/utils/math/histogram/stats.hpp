#ifndef GENESIS_UTILS_MATH_HISTOGRAM_STATS_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_STATS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Header of Histogram statistics functions.
 *
 * @file
 * @ingroup utils
 */

#include <stddef.h>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Histogram;

// =================================================================================================
//     Histogram Statistics
// =================================================================================================

double min_value(const Histogram& h);

double max_value(const Histogram& h);

size_t min_bin(const Histogram& h);

size_t max_bin(const Histogram& h);

double median(const Histogram& h);

/**
 * @brief Compute the bin-weighted arithmetic mean.
 *
 * The histogram is regarded as a probability distribution.
 * Negative bin values are ignored for the purposes of this calculation.
 * The accuracy of the result is limited by the bin width.
 */
double mean(const Histogram& h);

/**
 * @brief Compute the bin-weighted standard deviation.
 *
 * The histogram is regarded as a probability distribution.
 * Negative bin values are ignored for the purposes of this calculation.
 * The accuracy of the result is limited by the bin width.
 */
double sigma(const Histogram& h);

double sum(const Histogram& h);

} // namespace utils
} // namespace genesis

#endif // include guard
