#ifndef GENESIS_UTILS_MATH_HISTOGRAM_OPERATIONS_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_OPERATIONS_H_

/**
 * @brief Header of Histogram operations functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/histogram.hpp"

namespace genesis {

// =================================================================================================
//     Histogram Operations
// =================================================================================================

void add (Histogram& h, double value);

bool add (Histogram& h1, const Histogram& h2);

void scale (Histogram& h, double factor);

void normalize (Histogram& h, double total = 1.0);

} // namespace genesis

#endif // include guard
