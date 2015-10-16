#ifndef GENESIS_UTILS_MATH_HISTOGRAM_STATS_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_STATS_H_

/**
 * @brief Header of Histogram statistics functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/histogram.hpp"

namespace genesis {

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

} // namespace genesis

#endif // include guard
