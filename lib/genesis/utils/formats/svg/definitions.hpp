#ifndef GENESIS_UTILS_FORMATS_SVG_DEFINITIONS_H_
#define GENESIS_UTILS_FORMATS_SVG_DEFINITIONS_H_

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

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/formats/svg/helper.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Definitions
// =================================================================================================

class SvgDefinitions
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgDefinitions;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    template< typename T >
    SvgDefinitions( T const& object )
        : pimpl_( make_unique< Model<T> >( object ))
    {}

    SvgDefinitions( SvgDefinitions const& other )
        : pimpl_( other.pimpl_->clone() )
    {}

    SvgDefinitions( SvgDefinitions&& )      = default;

    SvgDefinitions& operator=( SvgDefinitions other )
    {
        std::swap( pimpl_, other.pimpl_ );
        return *this;
    }

    // SvgDefinitions& operator= ( SvgDefinitions const& ) = default;
    // SvgDefinitions& operator= ( SvgDefinitions&& )      = default;

    ~SvgDefinitions() = default;

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    void write(
        std::ostream& out,
        size_t indent = 0
    ) const {
        pimpl_->write_( out, indent );
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    struct Concept
    {
        virtual ~Concept() {}

        virtual void write_(
            std::ostream& out,
            size_t indent = 0
        ) const = 0;

        virtual std::unique_ptr< Concept > clone() const = 0;
    };

    template< typename T >
    struct Model : Concept
    {
        Model( T const& value )
            : object_( value )
        {}

        void write_(
            std::ostream& out,
            size_t indent = 0
        ) const {
            object_.write( out, indent );
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
