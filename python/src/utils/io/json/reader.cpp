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

PYTHON_EXPORT_CLASS (JsonReader, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonReader
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonReader > ( "JsonReader", boost::python::init<  >(  ) )
        .def( boost::python::init< JsonReader const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "from_file",
            ( void ( ::genesis::utils::JsonReader::* )( const std::string &, JsonDocument & ) const )( &::genesis::utils::JsonReader::from_file ),
            ( boost::python::arg("filename"), boost::python::arg("document") ),
            get_docstring("void ::genesis::utils::JsonReader::from_file (const std::string & filename, JsonDocument & document) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::utils::JsonReader::* )( const std::string &, JsonDocument & ) const )( &::genesis::utils::JsonReader::from_string ),
            ( boost::python::arg("json"), boost::python::arg("document") ),
            get_docstring("void ::genesis::utils::JsonReader::from_string (const std::string & json, JsonDocument & document) const")
        )
    ;
}
