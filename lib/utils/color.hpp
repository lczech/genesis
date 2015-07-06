#ifndef GENESIS_UTILS_COLOR_H_
#define GENESIS_UTILS_COLOR_H_

/**
 * @brief Header of Color class.
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <cmath>

#include "utils/logging.hpp"

namespace genesis {

// =================================================================================================
//     Color
// =================================================================================================

/**
 * @brief
 */
class Color
{
public:

    // -------------------------------------------------------------------------
    //     Constructors
    // -------------------------------------------------------------------------

    Color (unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}

    // Color (double r, double g, double b) :
    //     r ( static_cast<unsigned char>(round(std::min(std::max(0.0, r), 1.0) * 255.0 )) ),
    //     g ( static_cast<unsigned char>(round(std::min(std::max(0.0, g), 1.0) * 255.0 )) ),
    //     b ( static_cast<unsigned char>(round(std::min(std::max(0.0, b), 1.0) * 255.0 )) )
    // {}

public:

    static Color heat_gradient (int percentage)
    {
        Color red    {255,0,0};
        Color yellow {255,255,0};
        Color green  {0,255,0};

        if (percentage < 50) {
            return interpolate(green, yellow, percentage / 50.0);
        }
        return interpolate(yellow, red, (percentage - 50) / 50.0);
    }

protected:

    static Color interpolate (Color color1, Color color2, double fraction)
    {
        unsigned char r = interpolate(color1.r, color2.r, fraction);
        unsigned char g = interpolate(color1.g, color2.g, fraction);
        unsigned char b = interpolate(color1.b, color2.b, fraction);
        return Color(r, g, b);
    }

    static unsigned char interpolate (unsigned char d1, unsigned char d2, double fraction)
    {
        fraction = std::min(std::max(0.0, fraction), 1.0);
        return static_cast<unsigned char>(round( fraction * (d2 - d1) + d1 ));
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    std::string dump()
    {
        return "(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ")";
    }

    unsigned char r;
    unsigned char g;
    unsigned char b;

};

} // namespace genesis

#endif // include guard
