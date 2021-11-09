#ifndef GENESIS_UTILS_FORMATS_SVG_GROUP_H_
#define GENESIS_UTILS_FORMATS_SVG_GROUP_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/formats/svg/attributes.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/object.hpp"
#include "genesis/utils/core/std.hpp"

#include <iosfwd>
#include <stdexcept>
#include <unordered_map>

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
     * @brief Reverse the order to the stored SvgObject%s.
     */
    self_type& reverse();

    /**
     * @brief Simple tweak to allow making parts of an image clickable.
     *
     * This will simply add an `<a>` tag around the group, making everything in it clickable
     * and hyperlink to the given URL. We currently do not have any per-object hyperlinks,
     * and hence instead everything with a hyperlink has to go in a group.
     */
    self_type& set_hyperlink( std::string const& value )
    {
        hyperlink_ = std::unordered_map<std::string, std::string>{{ "href", value }};
        return *this;
    }

    /**
     * @brief Tweak to allow making parts of an image cickable.
     *
     * This is the same as the simply string version of this function, but additionally allows
     * to set extra attributes to be used within the `<a>` tag. Note that the key `href` needs
     * to be present in the map, in order to actually have a hyerlink to be cickable.
     */
    self_type& set_hyperlink( std::unordered_map<std::string, std::string> const& values )
    {
        if( values.count( "href" ) == 0 ) {
            throw std::invalid_argument(
                "Cannot set an SVG Group hyperlink without the `href` tag."
            );
        }
        hyperlink_ = values;
        return *this;
    }

    /**
     * @brief Shortcut operator for add(), which allows an even more fluent interface.
     */
    self_type& operator << ( SvgObject const& object );
    self_type& operator << ( SvgObject&&      object );

    bool empty() const
    {
        return content_.empty();
    }

    void clear()
    {
        id = "";
        content_.clear();
        transform.clear();
        hyperlink_.clear();
    }

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string              id;
    std::vector< SvgObject > content_;
    SvgTransform             transform;
    std::unordered_map<std::string, std::string> hyperlink_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
