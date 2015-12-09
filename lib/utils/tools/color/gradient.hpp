#ifndef GENESIS_UTILS_TOOLS_COLOR_GRADIENT_H_
#define GENESIS_UTILS_TOOLS_COLOR_GRADIENT_H_

/**
 * @brief Header for Color gradient functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/tools/color.hpp"

namespace genesis {
namespace color {

// =================================================================================================
//     Color Gradients
// =================================================================================================

/**
 * @brief Returns a Color that represents a heat gradient for a percentage value.
 *
 * Given a percentage value in the range of `[0.0, 1.0]`, the functions represents the heat of
 * that value. For 0.0, the heat gradient value is green, for 0.5 yellow and for 1.0 red.
 * The values in between are interpolated accordingly.
 */
Color heat_gradient (double percentage);

Color interpolate (Color color1, Color color2, double fraction);

unsigned char interpolate (unsigned char d1, unsigned char d2, double fraction);

} // namespace color
} // namespace genesis

#endif // include guard
