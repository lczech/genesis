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

/*
PYTHON_EXPORT_CLASS (Options, "utils")
{

    // -------------------------------------------------------------------
    //     Class Options
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Options > ( "Options" )

        // Public Member Functions

        .def(
            "command_line",
            ( std::vector< std::string > ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::command_line ),
            get_docstring("std::vector< std::string > ::genesis::utils::Options::command_line () const")
        )
        .def(
            "command_line_string",
            ( std::string ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::command_line_string ),
            get_docstring("std::string ::genesis::utils::Options::command_line_string () const")
        )
        .def(
            "dump",
            ( std::string ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::dump ),
            get_docstring("std::string ::genesis::utils::Options::dump () const")
        )
        .def(
            "get",
            ( Options & ( * )(  ))( &::genesis::utils::Options::get ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("static Options & ::genesis::utils::Options::get ()")
        )
        .staticmethod("get")
        .def(
            "number_of_threads",
            ( unsigned int ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::number_of_threads ),
            get_docstring("unsigned int ::genesis::utils::Options::number_of_threads () const")
        )
        .def(
            "random_engine",
            ( std::default_random_engine & ( ::genesis::utils::Options::* )(  ))( &::genesis::utils::Options::random_engine ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("std::default_random_engine & ::genesis::utils::Options::random_engine ()")
        )
        .def(
            "random_seed",
            ( unsigned ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::random_seed ),
            get_docstring("unsigned ::genesis::utils::Options::random_seed () const")
        )
        .def(
            "set_command_line",
            ( void ( ::genesis::utils::Options::* )( const int, const char * ))( &::genesis::utils::Options::set_command_line ),
            ( boost::python::arg("argc"), boost::python::arg("argv") ),
            get_docstring("void ::genesis::utils::Options::set_command_line (const int argc, const char * argv)")
        )
        .def(
            "set_number_of_threads",
            ( void ( ::genesis::utils::Options::* )( const unsigned int ))( &::genesis::utils::Options::set_number_of_threads ),
            ( boost::python::arg("number") ),
            get_docstring("void ::genesis::utils::Options::set_number_of_threads (const unsigned int number)")
        )
        .def(
            "set_random_seed",
            ( void ( ::genesis::utils::Options::* )( const unsigned ))( &::genesis::utils::Options::set_random_seed ),
            ( boost::python::arg("seed") ),
            get_docstring("void ::genesis::utils::Options::set_random_seed (const unsigned seed)")
        )
    ;
}
*/
