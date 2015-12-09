/**
 * @brief Implementationb of Color gradient functions.
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <cmath>
#include <string>

#include "utils/tools/color/gradient.hpp"

namespace genesis {
namespace color {

// =================================================================================================
//     Color Gradients
// =================================================================================================

Color heat_gradient (double percentage)
{
    Color red    {255,   0, 0};
    Color yellow {255, 255, 0};
    Color green  {  0, 255, 0};

    percentage = std::min(std::max(0.0, percentage), 1.0);

    if (percentage < 0.5) {
        return interpolate(green, yellow, percentage / 0.5);
    }
    return interpolate(yellow, red, (percentage - 0.5) / 0.5);
}

Color interpolate (Color color1, Color color2, double fraction)
{
    unsigned char r = interpolate(color1.r(), color2.r(), fraction);
    unsigned char g = interpolate(color1.g(), color2.g(), fraction);
    unsigned char b = interpolate(color1.b(), color2.b(), fraction);
    return Color(r, g, b);
}

unsigned char interpolate (unsigned char d1, unsigned char d2, double fraction)
{
    fraction = std::min(std::max(0.0, fraction), 1.0);
    return static_cast<unsigned char>(round( fraction * (d2 - d1) + d1 ));
}

} // namespace color
} // namespace genesis
