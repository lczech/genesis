#ifndef GENESIS_UTILS_MATH_HISTOGRAM_OPERATIONS_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_OPERATIONS_H_

/**
 * @brief Header of Histogram operations functions.
 *
 * @file
 * @ingroup utils
 */

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Histogram;

// =================================================================================================
//     Histogram Operations
// =================================================================================================

void offset (Histogram& h, double value);

void scale (Histogram& h, double factor);

void normalize (Histogram& h, double total = 1.0);

} // namespace genesis

#endif // include guard
