#ifndef GENESIS_UTILS_FORMATS_SVG_DOCUMENT_H_
#define GENESIS_UTILS_FORMATS_SVG_DOCUMENT_H_

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

#include "utils/formats/svg/helper.hpp"
#include "utils/formats/svg/object.hpp"

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

    void offset( double x, double y );
    void write( std::ostream& out ) const;

    self_type& add(          SvgObject const& object );
    self_type& operator << ( SvgObject const& object );

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

public:

    SvgMargin margin;

private:

    std::vector< SvgObject > content_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
