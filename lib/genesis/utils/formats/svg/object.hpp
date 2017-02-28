#ifndef GENESIS_UTILS_FORMATS_SVG_OBJECT_H_
#define GENESIS_UTILS_FORMATS_SVG_OBJECT_H_

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
#include "genesis/utils/core/std.hpp"

#include <iosfwd>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Object
// =================================================================================================

class SvgObject
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgObject;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    template< typename T >
    SvgObject( T const& object )
        : pimpl_( make_unique< Model<T> >( object ))
    {}

    SvgObject( SvgObject const& other )
        : pimpl_( other.pimpl_->clone() )
    {}

    SvgObject( SvgObject&& )      = default;

    SvgObject& operator=( SvgObject other )
    {
        std::swap( pimpl_, other.pimpl_ );
        return *this;
    }

    // SvgObject& operator= ( SvgObject const& ) = default;
    // SvgObject& operator= ( SvgObject&& )      = default;

    ~SvgObject() = default;

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    SvgBox bounding_box() const
    {
        return pimpl_->bounding_box_();
    }

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const {
        pimpl_->write_( out, indent, options );
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    struct Concept
    {
        virtual ~Concept() {}

        virtual SvgBox bounding_box_() const = 0;

        virtual void write_(
            std::ostream& out,
            size_t indent = 0,
            SvgDrawingOptions const& options = SvgDrawingOptions()
        ) const = 0;

        virtual std::unique_ptr< Concept > clone() const = 0;
    };

    template< typename T >
    struct Model : Concept
    {
        Model( T const& value )
            : object_( value )
        {}

        SvgBox bounding_box_() const
        {
            return object_.bounding_box();
        }

        void write_(
            std::ostream& out,
            size_t indent = 0,
            SvgDrawingOptions const& options = SvgDrawingOptions()
        ) const {
            object_.write( out, indent, options );
        }

        std::unique_ptr< Concept > clone() const
        {
            return make_unique< Model<T> >( object_ );
        }

        T object_;
    };

    std::unique_ptr< Concept > pimpl_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
