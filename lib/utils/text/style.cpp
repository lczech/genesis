/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/text/style.hpp"

#include "utils/text/string.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Output
// =================================================================================================

std::string get_attribute_string( Style const& s)
{
    std::vector<std::string> attribs;

    if( s.bold() ) {
        attribs.push_back("1");
    }
    if( s.foreground_color() != "" ) {
        attribs.push_back(s.get_foreground_color(s.foreground_color()));
    }
    if( s.background_color() != "" ) {
        attribs.push_back(s.get_background_color(s.background_color()));
    }

    return join( attribs, ";" );
}

std::string Style::to_bash_string() const
{
    // return "\e[" + get_attribute_string(*this) + "m" + text_ + "\e[0m";

    auto attribs = get_attribute_string(*this);
    if( attribs.size() > 0 ) {
        return "\033[" + get_attribute_string(*this) + "m" + text_ + "\033[0m";
    } else {
        return text_;
    }
}

std::string Style::to_python_string() const
{
    auto attribs = get_attribute_string(*this);
    if( attribs.size() > 0 ) {
        return "\x1b[" + get_attribute_string(*this) + "m" + text_ + "\x1b[0m";
    } else {
        return text_;
    }
}

// =================================================================================================
//     Style Data
// =================================================================================================

// -------------------------------------------------------------------
//     Foreground Color
// -------------------------------------------------------------------

static std::array<std::pair<std::string, std::string>, 17>::const_iterator get_foreground_color_iterator(
    std::string name
) {
    name = replace_all(name, " ", "");
    name = replace_all(name, "_", "");

    return std::find_if(
        Style::foreground_colors.begin(),
        Style::foreground_colors.end(),
        [&name] ( std::pair<std::string, std::string> const& elem ) {
            return equals_ci( elem.first, name );
        }
    );
}

bool Style::is_foreground_color( std::string name )
{
    return get_foreground_color_iterator(name) != Style::foreground_colors.end();
}

std::string Style::get_foreground_color( std::string name )
{
    auto it = get_foreground_color_iterator(name);

    if( it == Style::foreground_colors.end() ) {
        throw std::out_of_range("No foreground color named " + name + ".");
    }
    return it->second;
}

const std::array<std::pair<std::string, std::string>, 17> Style::foreground_colors = {{
    { "Default", "39" },
    { "Black", "30" },
    { "Red", "31" },
    { "Green", "32" },
    { "Yellow", "33" },
    { "Blue", "34" },
    { "Magenta", "35" },
    { "Cyan", "36" },
    { "LightGray", "37" },
    { "DarkGray", "90" },
    { "LightRed", "91" },
    { "LightGreen", "92" },
    { "LightYellow", "93" },
    { "LightBlue", "94" },
    { "LightMagenta", "95" },
    { "LightCyan", "96" },
    { "White", "97" }
}};

// -------------------------------------------------------------------
//     Background Color
// -------------------------------------------------------------------

std::array<std::pair<std::string, std::string>, 17>::const_iterator get_background_color_iterator(
    std::string name
) {
    name = replace_all(name, " ", "");
    name = replace_all(name, "_", "");

    return std::find_if(
        Style::background_colors.begin(),
        Style::background_colors.end(),
        [&name] ( std::pair<std::string, std::string> const& elem ) {
            return equals_ci( elem.first, name );
        }
    );
}

bool Style::is_background_color( std::string name )
{
    return get_background_color_iterator(name) != Style::background_colors.end();
}

std::string Style::get_background_color( std::string name )
{
    auto it = get_background_color_iterator(name);

    if( it == Style::background_colors.end() ) {
        throw std::out_of_range("No background color named " + name + ".");
    }
    return it->second;
}

const std::array<std::pair<std::string, std::string>, 17> Style::background_colors = {{
    { "Default", "49" },
    { "Black", "40" },
    { "Red", "41" },
    { "Green", "42" },
    { "Yellow", "43" },
    { "Blue", "44" },
    { "Magenta", "45" },
    { "Cyan", "46" },
    { "LightGray", "47" },
    { "DarkGray", "100" },
    { "LightRed", "101" },
    { "LightGreen", "102" },
    { "LightYellow", "103" },
    { "LightBlue", "104" },
    { "LightMagenta", "105" },
    { "LightCyan", "106" },
    { "White", "107" }
}};

} // namespace utils
} // namespace genesis
