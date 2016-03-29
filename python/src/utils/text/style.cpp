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

PYTHON_EXPORT_CLASS (Style, "utils")
{

    // -------------------------------------------------------------------
    //     Class Style
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Style > ( "Style", boost::python::init<  >(  ) )
        .def( boost::python::init< std::string const & >(( boost::python::arg("foreground_color") )) )
        .def( boost::python::init< const char * >(( boost::python::arg("foreground_color") )) )
        .def( boost::python::init< bool >(( boost::python::arg("bold") )) )
        .def( boost::python::init< std::string const &, bool >(( boost::python::arg("foreground_color"), boost::python::arg("bold") )) )
        .def( boost::python::init< std::string const &, std::string const & >(( boost::python::arg("foreground_color"), boost::python::arg("background_color") )) )
        .def( boost::python::init< std::string const &, const char * >(( boost::python::arg("foreground_color"), boost::python::arg("background_color") )) )
        .def( boost::python::init< std::string const &, std::string const &, bool >(( boost::python::arg("foreground_color"), boost::python::arg("background_color"), boost::python::arg("bold") )) )
        .def( boost::python::init< Style const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "background_color",
            ( Style & ( ::genesis::utils::Style::* )( std::string const & ))( &::genesis::utils::Style::background_color ),
            ( boost::python::arg("color") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "background_color",
            ( std::string ( ::genesis::utils::Style::* )(  ) const )( &::genesis::utils::Style::background_color )
        )
        .def(
            "bold",
            ( Style & ( ::genesis::utils::Style::* )( bool ))( &::genesis::utils::Style::bold ),
            ( boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Style & ::genesis::utils::Style::bold (bool value)")
        )
        .def(
            "bold",
            ( bool ( ::genesis::utils::Style::* )(  ) const )( &::genesis::utils::Style::bold ),
            get_docstring("bool ::genesis::utils::Style::bold () const")
        )
        .def(
            "enabled",
            ( Style & ( ::genesis::utils::Style::* )( bool ))( &::genesis::utils::Style::enabled ),
            ( boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Style & ::genesis::utils::Style::enabled (bool value)")
        )
        .def(
            "enabled",
            ( bool ( ::genesis::utils::Style::* )(  ) const )( &::genesis::utils::Style::enabled ),
            get_docstring("bool ::genesis::utils::Style::enabled () const")
        )
        .def(
            "foreground_color",
            ( Style & ( ::genesis::utils::Style::* )( std::string const & ))( &::genesis::utils::Style::foreground_color ),
            ( boost::python::arg("color") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "foreground_color",
            ( std::string ( ::genesis::utils::Style::* )(  ) const )( &::genesis::utils::Style::foreground_color )
        )
        .def(
            "get_background_color_value",
            ( std::string ( * )( std::string ))( &::genesis::utils::Style::get_background_color_value ),
            ( boost::python::arg("name") ),
            get_docstring("static std::string ::genesis::utils::Style::get_background_color_value (std::string name)")
        )
        .staticmethod("get_background_color_value")
        .def(
            "get_foreground_color_value",
            ( std::string ( * )( std::string ))( &::genesis::utils::Style::get_foreground_color_value ),
            ( boost::python::arg("name") ),
            get_docstring("static std::string ::genesis::utils::Style::get_foreground_color_value (std::string name)")
        )
        .staticmethod("get_foreground_color_value")
        .def(
            "is_background_color",
            ( bool ( * )( std::string ))( &::genesis::utils::Style::is_background_color ),
            ( boost::python::arg("name") ),
            get_docstring("static bool ::genesis::utils::Style::is_background_color (std::string name)")
        )
        .staticmethod("is_background_color")
        .def(
            "is_foreground_color",
            ( bool ( * )( std::string ))( &::genesis::utils::Style::is_foreground_color ),
            ( boost::python::arg("name") ),
            get_docstring("static bool ::genesis::utils::Style::is_foreground_color (std::string name)")
        )
        .staticmethod("is_foreground_color")
        .def(
            "reset",
            ( Style & ( ::genesis::utils::Style::* )(  ))( &::genesis::utils::Style::reset ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Style & ::genesis::utils::Style::reset ()")
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::Style::* )( Style & ))( &::genesis::utils::Style::swap ),
            ( boost::python::arg("other") )
        )
        .def(
            "to_bash_string",
            ( std::string ( ::genesis::utils::Style::* )( std::string const & ) const )( &::genesis::utils::Style::to_bash_string ),
            ( boost::python::arg("text") ),
            get_docstring("std::string ::genesis::utils::Style::to_bash_string (std::string const & text) const")
        )
        .def(
            "to_python_string",
            ( std::string ( ::genesis::utils::Style::* )( std::string const & ) const )( &::genesis::utils::Style::to_python_string ),
            ( boost::python::arg("text") ),
            get_docstring("std::string ::genesis::utils::Style::to_python_string (std::string const & text) const")
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_text_style_export, "utils")
{

    // boost::python::def(
    //     "get_background_color_iterator",
    //     ( std::array< std::pair< std::string, std::string >, 17 >::const_iterator ( * )( std::string ))( &::genesis::utils::get_background_color_iterator ),
    //     ( boost::python::arg("name") ),
    //     get_docstring("static std::array< std::pair< std::string, std::string >, 17 >::const_iterator ::genesis::utils::get_background_color_iterator (std::string name)")
    // );
    //
    // boost::python::def(
    //     "get_foreground_color_iterator",
    //     ( std::array< std::pair< std::string, std::string >, 17 >::const_iterator ( * )( std::string ))( &::genesis::utils::get_foreground_color_iterator ),
    //     ( boost::python::arg("name") ),
    //     get_docstring("static std::array< std::pair< std::string, std::string >, 17 >::const_iterator ::genesis::utils::get_foreground_color_iterator (std::string name)")
    // );
    //
    // boost::python::def(
    //     "operator<<",
    //     ( std::ostream & ( * )( std::ostream &, Style const & ))( &::genesis::utils::operator<< ),
    //     ( boost::python::arg("out"), boost::python::arg("style") ),
    //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
    //     get_docstring("std::ostream & ::genesis::utils::operator<< (std::ostream & out, Style const & style)")
    // );
    //
    // boost::python::def(
    //     "get_attribute_string",
    //     ( std::string ( * )( Style const & ))( &::genesis::utils::get_attribute_string ),
    //     ( boost::python::arg("s") ),
    //     get_docstring("std::string ::genesis::utils::get_attribute_string (Style const & s)")
    // );
    //
    // boost::python::def(
    //     "swap",
    //     ( void ( * )( Style &, Style & ))( &::genesis::utils::swap ),
    //     ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    // );
}
