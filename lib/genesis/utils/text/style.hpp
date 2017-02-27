#ifndef GENESIS_UTILS_TEXT_STYLE_H_
#define GENESIS_UTILS_TEXT_STYLE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include <array>
#include <iosfwd>
#include <utility>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Text Style
// =================================================================================================

/**
 * @brief Simple text style class for colorized and bold output to a terminal.
 *
 * This class bundles the following text style properties for output in a terminal:
 *
 *   * Foreground Color. Set using foreground_color( std::string const& color ).
 *     See Style::foreground_colors for the valid color names.
 *   * Background Color. Set using background_color( std::string const& color ).
 *     See Style::background_colors for the valid color names.
 *   * Bold. Set using bold( bool value ).
 *
 * Those properties can be set using either the respective constructor of this class or using the
 * setter functions. Per default, all of them are empty, meaning that no style manupulation is
 * done.
 *
 * In order to generate textual output with those styles, operator()() is used.
 * Example:
 *
 *     Style blue( "blue" );
 *     blue.bold( true );
 *     std::cout << blue( "some text" );
 *
 * Furthermore, the stream operator can be used to get a summary of the properties of a Style
 * object:
 *
 *     LOG_DBG << blue;
 *
 * will output
 *
 *     Foreground Color: Blue
 *     Background Color:
 *     Bold:             true
 *
 * to the LOG stream.
 */
class Style
{
    // -------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------

public:

    Style() = default;

    explicit Style( std::string const& foreground_color )
        : foreground_(foreground_color)
    {}

    explicit Style( const char * foreground_color )
        : foreground_(foreground_color)
    {}

    explicit Style( bool bold )
        : bold_(bold)
    {}

    Style( std::string const& foreground_color, bool bold )
        : foreground_(foreground_color)
        , bold_(bold)
    {}

    Style( std::string const& foreground_color, std::string const& background_color )
        : foreground_(foreground_color)
        , background_(background_color)
    {}

    Style( std::string const& foreground_color, const char * background_color )
        : foreground_(foreground_color)
        , background_(background_color)
    {}

    Style( std::string const& foreground_color, std::string const& background_color, bool bold )
        : foreground_(foreground_color)
        , background_(background_color)
        , bold_(bold)
    {}

    ~Style() = default;

    Style(Style const&) = default;
    Style(Style&&)      = default;

    Style& operator= (Style const&) = default;
    Style& operator= (Style&&)      = default;

    void swap (Style& other) noexcept
    {
        using std::swap;

        swap(foreground_, other.foreground_);
        swap(background_, other.background_);
        swap(bold_, other.bold_);
    }

    // -------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------

    Style&      reset();

    bool        enabled() const;
    Style&      enabled( bool value );

    bool        bold() const;
    Style&      bold( bool value );

    std::string foreground_color() const;
    Style&      foreground_color( std::string const& color );

    std::string background_color() const;
    Style&      background_color( std::string const& color );

    // -------------------------------------------------------------------
    //     Output
    // -------------------------------------------------------------------

    std::string operator() (      std::string const& text ) const;

    std::string to_bash_string(   std::string const& text ) const;
    std::string to_python_string( std::string const& text ) const;

    // -------------------------------------------------------------------
    //     Style Data
    // -------------------------------------------------------------------

    static bool is_foreground_color( std::string name );
    static bool is_background_color( std::string name );

    static std::string get_foreground_color_value( std::string name );
    static std::string get_background_color_value( std::string name );

    static const std::array<std::pair<std::string, std::string>, 17> foreground_colors;
    static const std::array<std::pair<std::string, std::string>, 17> background_colors;

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

private:

    std::string foreground_ = "";
    std::string background_ = "";

    bool bold_    = false;
    bool enabled_ = true;
};

// =================================================================================================
//     Basic Operators
// =================================================================================================

inline void swap (Style& lhs, Style& rhs) noexcept
{
    lhs.swap(rhs);
}

std::ostream& operator << ( std::ostream& out, Style const& style );

} // namespace utils
} // namespace genesis

#endif // include guard
