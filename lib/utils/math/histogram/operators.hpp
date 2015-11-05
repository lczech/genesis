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

// =================================================================================================
//     Histogram Operators
// =================================================================================================

std::ostream& operator<< (std::ostream& os, const Histogram& h);

} // namespace genesis

#endif // include guard
