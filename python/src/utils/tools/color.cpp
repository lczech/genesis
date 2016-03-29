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

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (Color, "utils")
{

    // -------------------------------------------------------------------
    //     Class Color
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Color > ( "Color", boost::python::init<  >(  ) )
        .def( boost::python::init< unsigned char, unsigned char, unsigned char >(( boost::python::arg("r"), boost::python::arg("g"), boost::python::arg("b") )) )
        .def( boost::python::init< Color const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "b",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::b )
        )
        .def(
            "b",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::b ),
            ( boost::python::arg("value") )
        )
        .def(
            "g",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::g )
        )
        .def(
            "g",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::g ),
            ( boost::python::arg("value") )
        )
        .def(
            "r",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::r )
        )
        .def(
            "r",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::r ),
            ( boost::python::arg("value") )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::Color::* )( Color & ))( &::genesis::utils::Color::swap ),
            ( boost::python::arg("other") )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_tools_color_export, "utils")
{

    boost::python::def(
        "operator!=",
        ( bool ( * )( Color const &, Color const & ))( &::genesis::utils::operator!= ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );

    boost::python::def(
        "operator==",
        ( bool ( * )( Color const &, Color const & ))( &::genesis::utils::operator== ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );

    boost::python::def(
        "swap",
        ( void ( * )( Color &, Color & ))( &::genesis::utils::swap ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );
}
