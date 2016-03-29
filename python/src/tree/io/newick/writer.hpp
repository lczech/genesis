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

template <typename TreeType_>
void PythonExportClass_NewickWriter(std::string name)
{

    // -------------------------------------------------------------------
    //     Class NewickWriter
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using NewickWriterType = NewickWriter<typename TreeType_>;

    boost::python::class_< NewickWriterType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< NewickWriter const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( NewickWriterType::* )( const TreeType &, const std::string ))( &NewickWriterType::to_file ),
            ( boost::python::arg("tree"), boost::python::arg("filename") ),
            get_docstring("void ::genesis::tree::NewickWriter::to_file (const TreeType & tree, const std::string filename)")
        )
        .def(
            "to_string",
            ( std::string ( NewickWriterType::* )( const TreeType & ))( &NewickWriterType::to_string ),
            ( boost::python::arg("tree") ),
            get_docstring("std::string ::genesis::tree::NewickWriter::to_string (const TreeType & tree)")
        )
        .def(
            "to_string",
            ( void ( NewickWriterType::* )( const TreeType &, std::string & ))( &NewickWriterType::to_string ),
            ( boost::python::arg("tree"), boost::python::arg("ts") ),
            get_docstring("void ::genesis::tree::NewickWriter::to_string (const TreeType & tree, std::string & ts)")
        )
    ;
}
