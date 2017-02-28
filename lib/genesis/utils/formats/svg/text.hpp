#ifndef GENESIS_UTILS_FORMATS_SVG_TEXT_H_
#define GENESIS_UTILS_FORMATS_SVG_TEXT_H_

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

#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/attributes.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Text
// =================================================================================================

struct SvgText
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgText;

    enum class Anchor
    {
        kNone,
        kStart,
        kMiddle,
        kEnd
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgText(
        std::string const& text,
        SvgPoint const&    position = SvgPoint(),
        SvgFont const&     font     = SvgFont(),
        SvgFill const&     fill     = SvgFill(),
        SvgStroke const&   stroke   = SvgStroke( SvgStroke::Type::kOmit )
    );

    ~SvgText() = default;

    SvgText( SvgText const& ) = default;
    SvgText( SvgText&& )      = default;

    SvgText& operator= ( SvgText const& ) = default;
    SvgText& operator= ( SvgText&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string id;

    std::string text;
    SvgPoint    position;

    SvgFont     font;
    SvgFill     fill;
    SvgStroke   stroke;

    Anchor      anchor = Anchor::kNone;

    double      kerning;
    double      letter_spacing;
    double      word_spacing;

    std::string dx;
    std::string dy;

    SvgTransform transform;

};

} // namespace utils
} // namespace genesis

#endif // include guard
