#ifndef GENESIS_UTILS_MATH_HISTOGRAM_OPERATORS_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_OPERATORS_H_

/**
 * @brief Header of Histogram operator functions.
 *
 * @file
 * @ingroup utils
 */

#include <iosfwd>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Histogram;
class HistogramAccumulator;

// =================================================================================================
//     Histogram Operators
// =================================================================================================

std::ostream& operator<< (std::ostream& os, const Histogram& h);

// =================================================================================================
//     Histogram Accumulator Operators
// =================================================================================================

std::ostream& operator<< (std::ostream& os, const HistogramAccumulator& h);

} // namespace genesis

#endif // include guard
