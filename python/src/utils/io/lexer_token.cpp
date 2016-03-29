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

PYTHON_EXPORT_CLASS (LexerToken, "utils")
{

    // -------------------------------------------------------------------
    //     Class LexerToken
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::LexerToken > ( "LexerToken", boost::python::init< const LexerTokenType, const int, const int, const std::string & >(( boost::python::arg("type"), boost::python::arg("line"), boost::python::arg("column"), boost::python::arg("value") )) )

        // Public Member Functions

        .def(
            "at",
            ( std::string ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::at ),
            get_docstring("std::string ::genesis::utils::LexerToken::at () const")
        )
        .def(
            "column",
            ( size_t ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::column ),
            get_docstring("size_t ::genesis::utils::LexerToken::column () const")
        )
        .def(
            "has_new_lines",
            ( size_t ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::has_new_lines ),
            get_docstring("size_t ::genesis::utils::LexerToken::has_new_lines () const")
        )
        .def(
            "is_bracket",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_bracket ),
            get_docstring("bool ::genesis::utils::LexerToken::is_bracket () const")
        )
        .def(
            "is_bracket",
            ( bool ( ::genesis::utils::LexerToken::* )( const std::string & ) const )( &::genesis::utils::LexerToken::is_bracket ),
            ( boost::python::arg("br") ),
            get_docstring("bool ::genesis::utils::LexerToken::is_bracket (const std::string & br) const")
        )
        .def(
            "is_comment",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_comment ),
            get_docstring("bool ::genesis::utils::LexerToken::is_comment () const")
        )
        .def(
            "is_error",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_error ),
            get_docstring("bool ::genesis::utils::LexerToken::is_error () const")
        )
        .def(
            "is_number",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_number ),
            get_docstring("bool ::genesis::utils::LexerToken::is_number () const")
        )
        .def(
            "is_operator",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_operator ),
            get_docstring("bool ::genesis::utils::LexerToken::is_operator () const")
        )
        .def(
            "is_operator",
            ( bool ( ::genesis::utils::LexerToken::* )( const std::string & ) const )( &::genesis::utils::LexerToken::is_operator ),
            ( boost::python::arg("op") ),
            get_docstring("bool ::genesis::utils::LexerToken::is_operator (const std::string & op) const")
        )
        .def(
            "is_string",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_string ),
            get_docstring("bool ::genesis::utils::LexerToken::is_string () const")
        )
        .def(
            "is_symbol",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_symbol ),
            get_docstring("bool ::genesis::utils::LexerToken::is_symbol () const")
        )
        .def(
            "is_tag",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_tag ),
            get_docstring("bool ::genesis::utils::LexerToken::is_tag () const")
        )
        .def(
            "is_unknown",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_unknown ),
            get_docstring("bool ::genesis::utils::LexerToken::is_unknown () const")
        )
        .def(
            "is_white",
            ( bool ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::is_white ),
            get_docstring("bool ::genesis::utils::LexerToken::is_white () const")
        )
        .def(
            "line",
            ( size_t ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::line ),
            get_docstring("size_t ::genesis::utils::LexerToken::line () const")
        )
        .def(
            "type",
            ( LexerTokenType ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::type ),
            get_docstring("LexerTokenType ::genesis::utils::LexerToken::type () const")
        )
        .def(
            "type_to_string",
            ( std::string ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::type_to_string ),
            get_docstring("std::string ::genesis::utils::LexerToken::type_to_string () const")
        )
        .def(
            "value",
            ( std::string ( ::genesis::utils::LexerToken::* )(  ) const )( &::genesis::utils::LexerToken::value ),
            get_docstring("std::string ::genesis::utils::LexerToken::value () const")
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_io_lexer_token_export, "utils")
{

    boost::python::def(
        "lexer_token_type_to_string",
        ( std::string ( * )( const LexerTokenType ))( &::genesis::utils::lexer_token_type_to_string ),
        ( boost::python::arg("t") ),
        get_docstring("std::string ::genesis::utils::lexer_token_type_to_string (const LexerTokenType t)")
    );
}
