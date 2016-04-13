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

template<class C, class T>
void python_export_function_utils_core_std_C_T ()
{
    boost::python::def(
        "contains",
        ( bool ( * )( const C &, const T & ))( &::genesis::utils::contains ),
        ( boost::python::arg("v"), boost::python::arg("x") ),
        get_docstring("bool ::genesis::utils::contains (const C & v, const T & x)")
    );
}

template<class Container, class UnaryPredicate>
void python_export_function_utils_core_std_Container_UnaryPredicate ()
{
    boost::python::def(
        "erase_if",
        ( void ( * )( Container &, UnaryPredicate ))( &::genesis::utils::erase_if ),
        ( boost::python::arg("c"), boost::python::arg("p") ),
        get_docstring("void ::genesis::utils::erase_if (Container & c, UnaryPredicate p)")
    );
}

template<typename T, typename... Args>
void python_export_function_utils_core_std_T_...Args ()
{
    boost::python::def(
        "make_unique",
        ( std::unique_ptr< T > ( * )( Args &&... ))( &::genesis::utils::make_unique ),
        ( boost::python::arg("args") ),
        get_docstring("std::unique_ptr< T > ::genesis::utils::make_unique (Args &&... args)")
    );
}
