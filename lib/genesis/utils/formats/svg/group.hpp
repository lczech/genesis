#ifndef GENESIS_UTILS_FORMATS_SVG_GROUP_H_
#define GENESIS_UTILS_FORMATS_SVG_GROUP_H_

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

#include "genesis/utils/formats/svg/attributes.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/object.hpp"
#include "genesis/utils/core/std.hpp"

#include <iosfwd>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Group
// =================================================================================================

struct SvgGroup
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgGroup;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgGroup() = default;
    ~SvgGroup() = default;

    SvgGroup( SvgGroup const& ) = default;
    SvgGroup( SvgGroup&& )      = default;

    SvgGroup& operator= ( SvgGroup const& ) = default;
    SvgGroup& operator= ( SvgGroup&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

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
    //     Properties
    // -------------------------------------------------------------

    std::string              id;
    std::vector< SvgObject > content_;
    SvgTransform             transform;

};

} // namespace utils
} // namespace genesis

#endif // include guard
