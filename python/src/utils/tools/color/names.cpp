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

PYTHON_EXPORT_CLASS (ColorNames, "utils")
{

    // -------------------------------------------------------------------
    //     Class ColorNames
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::ColorNames > ( "ColorNames" )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_tools_color_names_export, "utils")
{

    boost::python::def(
        "get_named_color",
        ( Color ( * )( std::string const & ))( &::genesis::utils::get_named_color ),
        ( boost::python::arg("name") ),
        get_docstring("Color ::genesis::utils::get_named_color (std::string const & name)")
    );

    boost::python::def(
        "is_named_color",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_named_color ),
        ( boost::python::arg("name") ),
        get_docstring("bool ::genesis::utils::is_named_color (std::string const & name)")
    );

    // boost::python::def(
    //     "get_named_color_iterator",
    //     ( std::array< std::pair< std::string, Color >, 140 >::const_iterator ( * )( std::string ))( &::genesis::utils::get_named_color_iterator ),
    //     ( boost::python::arg("name") ),
    //     get_docstring("std::array< std::pair< std::string, Color >, 140 >::const_iterator ::genesis::utils::get_named_color_iterator (std::string name)")
    // );
}
