#ifndef GENESIS_UTILS_TOOLS_COLOR_NAMES_H_
#define GENESIS_UTILS_TOOLS_COLOR_NAMES_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/tools/color.hpp"

#include <array>
#include <utility>
#include <string>

namespace genesis {

// =================================================================================================
//     Color Names
// =================================================================================================

/**
 * @brief Struct that stores a static const map of named colors.
 *
 * The map stores all 140 color names of the HTML web colors. It is mainly used as a simple way to
 * create a color scheme in form of a vector of colors.
 *
 * The data of this struct is used by `is_named_color` and `get_named_color`.
 */
struct ColorNames
{
    static const std::array<std::pair<std::string, Color>, 140> map;
};

bool is_named_color( std::string const& name );
Color get_named_color( std::string const& name );

} // namespace genesis

#endif // include guard
