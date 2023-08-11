#ifndef GENESIS_UTILS_FORMATS_SVG_DOCUMENT_H_
#define GENESIS_UTILS_FORMATS_SVG_DOCUMENT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/color/color.hpp"
#include "genesis/utils/formats/svg/definitions.hpp"
#include "genesis/utils/formats/svg/gradient.hpp"
#include "genesis/utils/formats/svg/object.hpp"
#include "genesis/utils/io/output_target.hpp"

#include <iosfwd>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Document
// =================================================================================================

class SvgDocument
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgDocument;

    static std::string indentation_string;

    enum class Overflow
    {
        kNone,
        kVisible,
        kHidden,
        kScroll,
        kAuto,
        kInherit
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgDocument()  = default;
    ~SvgDocument() = default;

    SvgDocument( SvgDocument const& ) = default;
    SvgDocument( SvgDocument&& )      = default;

    SvgDocument& operator= ( SvgDocument const& ) = default;
    SvgDocument& operator= ( SvgDocument&& )      = default;

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    SvgBox bounding_box() const;

    void write( std::ostream& out ) const;
    void write( std::shared_ptr<utils::BaseOutputTarget> target ) const;

    /**
     * @brief Add an SvgObject to the document.
     *
     * Returns the SvgDocument in order to allow for a fluent interface.
     */
    self_type& add(          SvgObject const& object );
    self_type& add(          SvgObject&&      object );

    /**
     * @brief Shortcut operator for add(), which allows an even more fluent interface.
     */
    self_type& operator << ( SvgObject const& object );
    self_type& operator << ( SvgObject&&      object );

    // -------------------------------------------------------------
    //     Helper Functions
    // -------------------------------------------------------------

private:

    static std::string overflow_to_string( Overflow value );

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

public:

    SvgMargin margin;
    Overflow overflow = Overflow::kNone;

    std::vector< SvgDefinitions > defs;
    Color background_color = Color( 1.0, 1.0, 1.0, 1.0 );

private:

    std::vector< SvgObject > content_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
