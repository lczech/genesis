#ifndef GENESIS_UTILS_TOOLS_COLOR_OPERATORS_H_
#define GENESIS_UTILS_TOOLS_COLOR_OPERATORS_H_

/**
 * @brief Color operators.
 *
 * @file
 * @ingroup utils
 */

#include "utils/tools/color/operators.hpp"

#include <cmath>
#include <ostream>

#include "utils/tools/color.hpp"

namespace genesis {

// =================================================================================================
//     Color Operators
// =================================================================================================

Color color_from_doubles (double r, double g, double b)
{
    auto rc = static_cast<unsigned char>( round(std::min(std::max(0.0, r), 1.0) * 255.0 ) );
    auto gc = static_cast<unsigned char>( round(std::min(std::max(0.0, g), 1.0) * 255.0 ) );
    auto bc = static_cast<unsigned char>( round(std::min(std::max(0.0, b), 1.0) * 255.0 ) );
    return Color(rc, gc, bc);
}

std::ostream& operator<< (std::ostream& os, const Color& color)
{
    os << "(" << color.r() << ", " << color.g() << ", " << color.b() << ")";
    return os;
}

} // namespace genesis

#endif // include guard
