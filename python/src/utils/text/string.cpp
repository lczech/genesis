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

PYTHON_EXPORT_FUNCTIONS(utils_text_string_export, "utils")
{

    boost::python::def(
        "equals_ci",
        ( bool ( * )( std::string const &, std::string const & ))( &::genesis::utils::equals_ci ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") ),
        get_docstring("bool ::genesis::utils::equals_ci (std::string const & lhs, std::string const & rhs)")
    );

    boost::python::def(
        "count_substring_occurrences",
        ( size_t ( * )( std::string const &, std::string const & ))( &::genesis::utils::count_substring_occurrences ),
        ( boost::python::arg("str"), boost::python::arg("sub") ),
        get_docstring("size_t ::genesis::utils::count_substring_occurrences (std::string const & str, std::string const & sub)")
    );

    boost::python::def(
        "deescape",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::deescape ),
        ( boost::python::arg("text") ),
        get_docstring("std::string ::genesis::utils::deescape (std::string const & text)")
    );

    boost::python::def(
        "escape",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::escape ),
        ( boost::python::arg("text") ),
        get_docstring("std::string ::genesis::utils::escape (std::string const & text)")
    );

    boost::python::def(
        "replace_all",
        ( std::string ( * )( std::string const &, std::string const &, std::string const & ))( &::genesis::utils::replace_all ),
        ( boost::python::arg("text"), boost::python::arg("search"), boost::python::arg("replace") ),
        get_docstring("std::string ::genesis::utils::replace_all (std::string const & text, std::string const & search, std::string const & replace)")
    );

    boost::python::def(
        "to_lower",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_lower ),
        ( boost::python::arg("str") ),
        get_docstring("std::string ::genesis::utils::to_lower (std::string const & str)")
    );

    boost::python::def(
        "to_string_precise",
        ( std::string ( * )( double, const int ))( &::genesis::utils::to_string_precise ),
        ( boost::python::arg("value"), boost::python::arg("precision") ),
        get_docstring("std::string ::genesis::utils::to_string_precise (double value, const int precision)")
    );

    boost::python::def(
        "to_upper",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_upper ),
        ( boost::python::arg("str") ),
        get_docstring("std::string ::genesis::utils::to_upper (std::string const & str)")
    );

    boost::python::def(
        "trim",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim ),
        ( boost::python::arg("s"), boost::python::arg("delimiters") ),
        get_docstring("std::string ::genesis::utils::trim (std::string const & s, std::string const & delimiters)")
    );

    boost::python::def(
        "trim_left",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim_left ),
        ( boost::python::arg("s"), boost::python::arg("delimiters") ),
        get_docstring("std::string ::genesis::utils::trim_left (std::string const & s, std::string const & delimiters)")
    );

    boost::python::def(
        "trim_right",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim_right ),
        ( boost::python::arg("s"), boost::python::arg("delimiters") ),
        get_docstring("std::string ::genesis::utils::trim_right (std::string const & s, std::string const & delimiters)")
    );

    boost::python::def(
        "unify_newlines",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::unify_newlines ),
        ( boost::python::arg("s") )
    );

    boost::python::def(
        "split",
        ( std::vector< std::string > ( * )( std::string const &, std::string const &, const bool ))( &::genesis::utils::split ),
        ( boost::python::arg("str"), boost::python::arg("delimiters"), boost::python::arg("trim_empty") ),
        get_docstring("std::vector< std::string > ::genesis::utils::split (std::string const & str, std::string const & delimiters, const bool trim_empty)")
    );
}
