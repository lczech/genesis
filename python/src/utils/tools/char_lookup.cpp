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

PYTHON_EXPORT_CLASS (CharLookup, "utils")
{

    // -------------------------------------------------------------------
    //     Class CharLookup
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::CharLookup > ( "CharLookup", boost::python::init<  >(  ) )
        .def( boost::python::init< CharLookup const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "all_set",
            ( bool ( ::genesis::utils::CharLookup::* )(  ) const )( &::genesis::utils::CharLookup::all_set ),
            get_docstring("bool ::genesis::utils::CharLookup::all_set () const")
        )
        .def(
            "all_unset",
            ( bool ( ::genesis::utils::CharLookup::* )(  ) const )( &::genesis::utils::CharLookup::all_unset ),
            get_docstring("bool ::genesis::utils::CharLookup::all_unset () const")
        )
        .def(
            "get",
            ( bool ( ::genesis::utils::CharLookup::* )( char ) const )( &::genesis::utils::CharLookup::get ),
            ( boost::python::arg("c") ),
            get_docstring("bool ::genesis::utils::CharLookup::get (char c) const")
        )
        .def(
            "get_selection",
            ( std::string ( ::genesis::utils::CharLookup::* )(  ) const )( &::genesis::utils::CharLookup::get_selection ),
            get_docstring("std::string ::genesis::utils::CharLookup::get_selection () const")
        )
        .def(
            "set_all",
            ( void ( ::genesis::utils::CharLookup::* )( bool ))( &::genesis::utils::CharLookup::set_all ),
            ( boost::python::arg("value")=(bool)(true) ),
            get_docstring("void ::genesis::utils::CharLookup::set_all (bool value=true)")
        )
        .def(
            "set_char",
            ( void ( ::genesis::utils::CharLookup::* )( char, bool ))( &::genesis::utils::CharLookup::set_char ),
            ( boost::python::arg("c"), boost::python::arg("value")=(bool)(true) ),
            get_docstring("void ::genesis::utils::CharLookup::set_char (char c, bool value=true)")
        )
        .def(
            "set_if",
            ( void ( ::genesis::utils::CharLookup::* )( std::function< bool(char)>, bool ))( &::genesis::utils::CharLookup::set_if ),
            ( boost::python::arg("predicate"), boost::python::arg("value")=(bool)(true) ),
            get_docstring("void ::genesis::utils::CharLookup::set_if (std::function< bool(char)> predicate, bool value=true)")
        )
        .def(
            "set_range",
            ( void ( ::genesis::utils::CharLookup::* )( char, char, bool ))( &::genesis::utils::CharLookup::set_range ),
            ( boost::python::arg("first"), boost::python::arg("last"), boost::python::arg("value")=(bool)(true) ),
            get_docstring("void ::genesis::utils::CharLookup::set_range (char first, char last, bool value=true)")
        )
        .def(
            "set_selection",
            ( void ( ::genesis::utils::CharLookup::* )( std::string const &, bool ))( &::genesis::utils::CharLookup::set_selection ),
            ( boost::python::arg("chars"), boost::python::arg("value")=(bool)(true) ),
            get_docstring("void ::genesis::utils::CharLookup::set_selection (std::string const & chars, bool value=true)")
        )

        // Operators

        .def(
            "__getitem__",
            ( bool ( ::genesis::utils::CharLookup::* )( char ) const )( &::genesis::utils::CharLookup::operator[] ),
            ( boost::python::arg("c") ),
            get_docstring("bool ::genesis::utils::CharLookup::operator[] (char c) const")
        )
    ;
}
