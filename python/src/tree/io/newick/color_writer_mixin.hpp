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
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_NewickColorWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class NewickColorWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using NewickColorWriterMixinType = NewickColorWriterMixin<typename Base>;

    boost::python::class_< NewickColorWriterMixinType > ( name.c_str() )

        // Public Member Functions

        .def(
            "color_tag_prefix",
            ( std::string ( NewickColorWriterMixinType::* )(  ))( &NewickColorWriterMixinType::color_tag_prefix ),
            get_docstring("std::string ::genesis::tree::NewickColorWriterMixin::color_tag_prefix ()")
        )
        .def(
            "color_tag_prefix",
            ( void ( NewickColorWriterMixinType::* )( std::string ))( &NewickColorWriterMixinType::color_tag_prefix ),
            ( boost::python::arg("prefix") ),
            get_docstring("void ::genesis::tree::NewickColorWriterMixin::color_tag_prefix (std::string prefix)")
        )
        .def(
            "color_tag_suffix",
            ( std::string ( NewickColorWriterMixinType::* )(  ))( &NewickColorWriterMixinType::color_tag_suffix ),
            get_docstring("std::string ::genesis::tree::NewickColorWriterMixin::color_tag_suffix ()")
        )
        .def(
            "color_tag_suffix",
            ( void ( NewickColorWriterMixinType::* )( std::string ))( &NewickColorWriterMixinType::color_tag_suffix ),
            ( boost::python::arg("suffix") ),
            get_docstring("void ::genesis::tree::NewickColorWriterMixin::color_tag_suffix (std::string suffix)")
        )
    ;
}
