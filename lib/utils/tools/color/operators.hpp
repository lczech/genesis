#ifndef GENESIS_UTILS_TOOLS_COLOR_OPERATORS_H_
#define GENESIS_UTILS_TOOLS_COLOR_OPERATORS_H_

/**
 * @brief Color operators.
 *
 * @file
 * @ingroup utils
 */

#include <iosfwd>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Color;

// =================================================================================================
//     Color Operators
// =================================================================================================

/**
 * @brief Create a Color given three doubles in the range [0.0, 1.0] for each of the components
 * red, green and blue.
 */
Color color_from_doubles (double r, double g, double b);

/**
 * @brief Write a textual representation of the Color the a stream, in the format "(r, g, b)".
 */
std::ostream& operator<< (std::ostream& os, const Color& h);

} // namespace genesis

#endif // include guard
