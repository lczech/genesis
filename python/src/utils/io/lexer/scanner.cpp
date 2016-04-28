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

PYTHON_EXPORT_FUNCTIONS(utils_io_lexer_scanner_export, "utils")
{

    boost::python::def(
        "read_char_if",
        ( char ( * )( utils::CountingIstream &, char ))( &::genesis::utils::read_char_if ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("char ::genesis::utils::read_char_if (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "read_char_if",
        ( char ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::read_char_if ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("char ::genesis::utils::read_char_if (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "read_until",
        ( std::string ( * )( utils::CountingIstream &, char ))( &::genesis::utils::read_until ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("std::string ::genesis::utils::read_until (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "read_until",
        ( std::string ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::read_until ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("std::string ::genesis::utils::read_until (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "read_while",
        ( std::string ( * )( utils::CountingIstream &, char ))( &::genesis::utils::read_while ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("std::string ::genesis::utils::read_while (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "read_while",
        ( std::string ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::read_while ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("std::string ::genesis::utils::read_while (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "expect_char",
        ( void ( * )( utils::CountingIstream &, char ))( &::genesis::utils::expect_char ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::expect_char (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "expect_char",
        ( void ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::expect_char ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::expect_char (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "skip_until",
        ( void ( * )( utils::CountingIstream &, char ))( &::genesis::utils::skip_until ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::skip_until (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "skip_until",
        ( void ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::skip_until ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::skip_until (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "skip_while",
        ( void ( * )( utils::CountingIstream &, char ))( &::genesis::utils::skip_while ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::skip_while (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "skip_while",
        ( void ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::skip_while ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::skip_while (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );
}
