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

PYTHON_EXPORT_CLASS (XmlWriter, "utils")
{

    // -------------------------------------------------------------------
    //     Class XmlWriter
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::XmlWriter > ( "XmlWriter" )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::utils::XmlWriter::* )( const XmlDocument &, const std::string & ))( &::genesis::utils::XmlWriter::to_file ),
            ( boost::python::arg("document"), boost::python::arg("filename") ),
            get_docstring("void ::genesis::utils::XmlWriter::to_file (const XmlDocument & document, const std::string & filename)")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::XmlWriter::* )( const XmlDocument & ))( &::genesis::utils::XmlWriter::to_string ),
            ( boost::python::arg("document") ),
            get_docstring("std::string ::genesis::utils::XmlWriter::to_string (const XmlDocument & document)")
        )
        .def(
            "to_string",
            ( void ( ::genesis::utils::XmlWriter::* )( const XmlDocument &, std::string & ))( &::genesis::utils::XmlWriter::to_string ),
            ( boost::python::arg("document"), boost::python::arg("output") ),
            get_docstring("void ::genesis::utils::XmlWriter::to_string (const XmlDocument & document, std::string & output)")
        )
    ;
}
