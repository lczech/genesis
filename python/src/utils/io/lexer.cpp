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

PYTHON_EXPORT_CLASS (Lexer, "utils")
{

    // -------------------------------------------------------------------
    //     Class Lexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Lexer > ( "Lexer" )

        // Public Member Functions

        .def(
            "back",
            ( LexerToken ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::back ),
            get_docstring("LexerToken ::genesis::utils::Lexer::back () const")
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::Lexer::* )(  ))( &::genesis::utils::Lexer::clear ),
            get_docstring("void ::genesis::utils::Lexer::clear ()")
        )
        .def(
            "dump",
            ( std::string ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::dump ),
            get_docstring("std::string ::genesis::utils::Lexer::dump () const")
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::empty ),
            get_docstring("bool ::genesis::utils::Lexer::empty () const")
        )
        .def(
            "from_string",
            ( bool ( ::genesis::utils::Lexer::* )( const std::string & ))( &::genesis::utils::Lexer::from_string ),
            ( boost::python::arg("in") ),
            get_docstring("bool ::genesis::utils::Lexer::from_string (const std::string & in)")
        )
        .def(
            "has_error",
            ( bool ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::has_error ),
            get_docstring("bool ::genesis::utils::Lexer::has_error () const")
        )
        .def(
            "process_all",
            ( bool ( ::genesis::utils::Lexer::* )(  ))( &::genesis::utils::Lexer::process_all )
        )
        .def(
            "process_step",
            ( bool ( ::genesis::utils::Lexer::* )(  ))( &::genesis::utils::Lexer::process_step ),
            get_docstring("bool ::genesis::utils::Lexer::process_step ()")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::size ),
            get_docstring("size_t ::genesis::utils::Lexer::size () const")
        )
        .def(
            "validate_brackets",
            ( bool ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::validate_brackets ),
            get_docstring("bool ::genesis::utils::Lexer::validate_brackets () const")
        )

        // Iterators

        // .def(
        //     "__iter__",
        //     boost::python::range ( &::genesis::utils::Lexer::begin, &::genesis::utils::Lexer::end )
        // )
    ;
}
