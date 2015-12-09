#ifndef GENESIS_UTILS_TOOLS_COLOR_OPERATORS_H_
#define GENESIS_UTILS_TOOLS_COLOR_OPERATORS_H_

/**
 * @brief Color operators.
 *
 * @file
 * @ingroup utils
 */

#include <iosfwd>
#include <string>

namespace genesis {
namespace color {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Color;

// =================================================================================================
//     Color Conversion
// =================================================================================================

Color color_from_doubles (double r, double g, double b);

Color color_from_hex( std::string h, std::string prefix = "#" );

std::string color_to_hex( const Color& c, std::string prefix = "#", bool uppercase = false );

// =================================================================================================
//     Color Operators
// =================================================================================================

std::ostream& operator<< (std::ostream& os, const Color& h);

} // namespace color
} // namespace genesis

#endif // include guard
