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

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS (ColorWriterMixin, "tree")
{

    // -------------------------------------------------------------------
    //     Class ColorWriterMixin
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::ColorWriterMixin > ( "ColorWriterMixin" )

        // Public Member Functions

        .def(
            "edge_colors",
            ( std::vector< utils::Color > ( ::genesis::tree::ColorWriterMixin::* )(  ))( &::genesis::tree::ColorWriterMixin::edge_colors ),
            get_docstring("std::vector< utils::Color > ::genesis::tree::ColorWriterMixin::edge_colors ()")
        )
        .def(
            "edge_colors",
            ( void ( ::genesis::tree::ColorWriterMixin::* )( std::vector< utils::Color > const & ))( &::genesis::tree::ColorWriterMixin::edge_colors ),
            ( boost::python::arg("color_vector") ),
            get_docstring("void ::genesis::tree::ColorWriterMixin::edge_colors (std::vector< utils::Color > const & color_vector)")
        )
        .def(
            "enable_color",
            ( bool ( ::genesis::tree::ColorWriterMixin::* )(  ) const )( &::genesis::tree::ColorWriterMixin::enable_color ),
            get_docstring("bool ::genesis::tree::ColorWriterMixin::enable_color () const")
        )
        .def(
            "enable_color",
            ( void ( ::genesis::tree::ColorWriterMixin::* )( bool ))( &::genesis::tree::ColorWriterMixin::enable_color ),
            ( boost::python::arg("value") ),
            get_docstring("void ::genesis::tree::ColorWriterMixin::enable_color (bool value)")
        )
        .def(
            "ignored_color",
            ( utils::Color ( ::genesis::tree::ColorWriterMixin::* )(  ) const )( &::genesis::tree::ColorWriterMixin::ignored_color ),
            get_docstring("utils::Color ::genesis::tree::ColorWriterMixin::ignored_color () const")
        )
        .def(
            "ignored_color",
            ( void ( ::genesis::tree::ColorWriterMixin::* )( utils::Color ))( &::genesis::tree::ColorWriterMixin::ignored_color ),
            ( boost::python::arg("value") ),
            get_docstring("void ::genesis::tree::ColorWriterMixin::ignored_color (utils::Color value)")
        )
    ;
}
