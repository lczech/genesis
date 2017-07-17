#ifndef GENESIS_UTILS_FORMATS_SVG_IMAGE_H_
#define GENESIS_UTILS_FORMATS_SVG_IMAGE_H_

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
//     Svg Image
// =================================================================================================

struct SvgImage
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgImage;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgImage(
        std::string const& href
    );

    SvgImage(
        std::string const& href,
        SvgPoint const&    position
    );

    SvgImage(
        std::string const& href,
        SvgPoint const&    position,
        SvgSize const&     size
    );

    SvgImage(
        std::string const& href,
        double x, double y,
        double w, double h
    );

    ~SvgImage() = default;

    SvgImage( SvgImage const& ) = default;
    SvgImage( SvgImage&& )      = default;

    SvgImage& operator= ( SvgImage const& ) = default;
    SvgImage& operator= ( SvgImage&& )      = default;

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

    std::string  id;

    std::string  href;

    SvgPoint     position;
    SvgSize      size;

    SvgTransform transform;

};

} // namespace utils
} // namespace genesis

#endif // include guard
