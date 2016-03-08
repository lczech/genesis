#ifndef GENESIS_UTILS_TOOLS_COLOR_GRADIENT_H_
#define GENESIS_UTILS_TOOLS_COLOR_GRADIENT_H_

/**
 * @brief Header for Color gradient functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/tools/color.hpp"

#include <map>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Gradients
// =================================================================================================

Color gradient ( std::map<double, Color> const& ranges, double percentage );

Color heat_gradient (double percentage);

Color interpolate (Color color1, Color color2, double fraction);

} // namespace utils
} // namespace genesis

#endif // include guard
