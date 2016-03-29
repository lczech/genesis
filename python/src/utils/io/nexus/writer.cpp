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

PYTHON_EXPORT_CLASS (NexusWriter, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusWriter
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusWriter > ( "NexusWriter", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusWriter const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::string const & ) const )( &::genesis::utils::NexusWriter::to_file ),
            ( boost::python::arg("doc"), boost::python::arg("filename") )
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::ostream & ) const )( &::genesis::utils::NexusWriter::to_stream ),
            ( boost::python::arg("doc"), boost::python::arg("out") )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::NexusWriter::* )( NexusDocument const & ) const )( &::genesis::utils::NexusWriter::to_string ),
            ( boost::python::arg("doc") )
        )
        .def(
            "to_string",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::string & ) const )( &::genesis::utils::NexusWriter::to_string ),
            ( boost::python::arg("doc"), boost::python::arg("output") )
        )
    ;
}
