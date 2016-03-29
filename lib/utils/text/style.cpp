/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/text/style.hpp"

#include "utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Local Helper Functions
// =================================================================================================

/**
 * @brief Internal helper function that returns an iterator into the foreground color list.
 */
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

/**
 * @brief Internal helper function that returns an iterator into the background color list.
 */
static std::array<std::pair<std::string, std::string>, 17>::const_iterator get_background_color_iterator(
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

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Reset the Style to use not colors and not bold.
 */
Style& Style::reset()
{
    foreground_ = "";
    background_ = "";
    bold_ = false;

    return *this;
}

/**
 * @brief Return whether the Style is currently enabled.
 */
bool Style::enabled() const
{
    return enabled_;
}

/**
 * @brief Set whether the Style is enabled.
 *
 * If set to `false`, no style attributes are outputted when the Style is applied to a text with
 * operator()(). Default is `true`.
 *
 * The function returns the Style object itself, in order to allow a fluent interface.
 */
Style& Style::enabled( bool value )
{
    enabled_ = value;
    return *this;
}

/**
 * @brief Return whether the Style uses bold.
 */
bool Style::bold() const
{
    return bold_;
}

/**
 * @brief Set whether the Style uses bold.
 *
 * The function returns the Style object itself, in order to allow a fluent interface.
 */
Style& Style::bold( bool value )
{
    bold_ = value;
    return *this;
}

std::string Style::foreground_color() const
{
    if( foreground_ == "" ) {
        return "";
    }

    auto it = get_foreground_color_iterator(foreground_);
    assert( it != Style::foreground_colors.end() );
    return it->first;
}

Style& Style::foreground_color( std::string const& color )
{
    // Check if valid color, throw if not.
    if( color != "" ) {
        get_foreground_color_value( color );
    }

    foreground_ = color;
    return *this;
}

std::string Style::background_color() const
{
    if( background_ == "" ) {
        return "";
    }

    auto it = get_background_color_iterator(background_);
    assert( it != Style::background_colors.end() );
    return it->first;
}

Style& Style::background_color( std::string const& color )
{
    // Check if valid color, throw if not.
    if( color != "" ) {
        get_background_color_value( color );
    }

    background_ = color;
    return *this;
}

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Internal helper function that returns the attribute string of a Style based on its
 * properties.
 */
std::string get_attribute_string( Style const& s)
{
    if( ! s.enabled() ) {
        return "";
    }

    std::vector<std::string> attribs;

    if( s.bold() ) {
        attribs.push_back( "1" );
    }
    if( s.foreground_color() != "" ) {
        attribs.push_back( s.get_foreground_color_value( s.foreground_color() ));
    }
    if( s.background_color() != "" ) {
        attribs.push_back( s.get_background_color_value( s.background_color() ));
    }

    return join( attribs, ";" );
}

/**
 * @brief Operator that returns a text with the current Style applied to it.
 *
 * This is most helpful for outputting styled text to a terminal. See the Style class description
 * for an example.
 */
std::string Style::operator() ( std::string const& text ) const
{
    return to_bash_string( text );
}

/**
 * @brief Additional output function with the same purpose as operator().
 *
 * It uses the notation `\033` for the escape symbol, which is best interpreted by bash.
 * Usually, there is no need to use this function. The operator() should just work fine.
 */
std::string Style::to_bash_string( std::string const& text ) const
{
    // return "\e[" + get_attribute_string(*this) + "m" + text_ + "\e[0m";

    auto attribs = get_attribute_string(*this);
    if( attribs.size() > 0 ) {
        return "\033[" + get_attribute_string(*this) + "m" + text + "\033[0m";
    } else {
        return text;
    }
}

/**
 * @brief Additional output function with the same purpose as operator().
 *
 * It uses the notation `\x1b` for the escape symbol, which is best interpreted by python.
 * Usually, there is no need to use this function. The operator() should just work fine.
 */
std::string Style::to_python_string( std::string const& text ) const
{
    auto attribs = get_attribute_string(*this);
    if( attribs.size() > 0 ) {
        return "\x1b[" + get_attribute_string(*this) + "m" + text + "\x1b[0m";
    } else {
        return text;
    }
}

// =================================================================================================
//     Style Data
// =================================================================================================

// -------------------------------------------------------------------
//     Foreground Color
// -------------------------------------------------------------------

/**
 * @brief Return `true` iff the given name is a foreground color name.
 */
bool Style::is_foreground_color( std::string name )
{
    return get_foreground_color_iterator(name) != Style::foreground_colors.end();
}

/**
 * @brief Return the color value string for a given foreground color name.
 *
 * See Style::foreground_colors for valid foreground color names.
 * If the name is invalid, the function throws an `std::out_of_range` exception.
 */
std::string Style::get_foreground_color_value( std::string name )
{
    auto it = get_foreground_color_iterator(name);

    if( it == Style::foreground_colors.end() ) {
        throw std::out_of_range("No foreground color named " + name + ".");
    }
    return it->second;
}

/**
 * @brief List of all valid foreground color names and their color strings.
 */
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

/**
 * @brief Return `true` iff the given name is a background color name.
 */
bool Style::is_background_color( std::string name )
{
    return get_background_color_iterator(name) != Style::background_colors.end();
}

/**
 * @brief Return the color value string for a given background color name.
 *
 * See Style::foreground_colors for background valid color names.
 * If the name is invalid, the function throws an `std::out_of_range` exception.
 */
std::string Style::get_background_color_value( std::string name )
{
    auto it = get_background_color_iterator(name);

    if( it == Style::background_colors.end() ) {
        throw std::out_of_range("No background color named " + name + ".");
    }
    return it->second;
}

/**
 * @brief List of all valid background color names and their color strings.
 */
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

// =================================================================================================
//     Basic Operators
// =================================================================================================

/**
 * @brief Print the properties of a Style object to a stream.
 *
 * The application of Style%s is usually done by invoking its operator(). However, in order to
 * be able to show its properties at a glance, this operator instead prints them:
 *
 *     Style s( "blue" );
 *     std::cout << s;
 *
 * will print this information instead of a stylized text.
 */
std::ostream& operator << ( std::ostream& out, Style const& style )
{
    out << "Foreground Color: " << style.foreground_color() << "\n";
    out << "Background Color: " << style.background_color() << "\n";
    out << "Bold:             " << ( style.bold() ? "true" : "false" ) << "\n";
    return out;
}

} // namespace utils
} // namespace genesis
