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

template<class T>
void python_export_function_utils_io_lexer_parser_T ()
{
    boost::python::def(
        "parse_float",
        ( T ( * )( utils::CountingIstream & ))( &::genesis::utils::parse_float ),
        ( boost::python::arg("source") ),
        get_docstring("T ::genesis::utils::parse_float (utils::CountingIstream & source)")
    );

    boost::python::def(
        "parse_integer",
        ( T ( * )( utils::CountingIstream & ))( &::genesis::utils::parse_integer ),
        ( boost::python::arg("source") ),
        get_docstring("T ::genesis::utils::parse_integer (utils::CountingIstream & source)")
    );

    boost::python::def(
        "parse_signed_integer",
        ( T ( * )( utils::CountingIstream & ))( &::genesis::utils::parse_signed_integer ),
        ( boost::python::arg("source") ),
        get_docstring("T ::genesis::utils::parse_signed_integer (utils::CountingIstream & source)")
    );

    boost::python::def(
        "parse_unsigned_integer",
        ( T ( * )( utils::CountingIstream & ))( &::genesis::utils::parse_unsigned_integer ),
        ( boost::python::arg("source") ),
        get_docstring("T ::genesis::utils::parse_unsigned_integer (utils::CountingIstream & source)")
    );
}
