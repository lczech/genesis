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

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (SampleSerializer, "placement")
{

    // -------------------------------------------------------------------
    //     Class SampleSerializer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::SampleSerializer > ( "SampleSerializer" )

        // Public Member Functions

        .def(
            "load",
            ( void ( * )( const std::string &, Sample & ))( &::genesis::placement::SampleSerializer::load ),
            ( boost::python::arg("file_name"), boost::python::arg("map") ),
            get_docstring("static void ::genesis::placement::SampleSerializer::load (const std::string & file_name, Sample & map)")
        )
        .staticmethod("load")
        .def(
            "save",
            ( void ( * )( const Sample &, const std::string & ))( &::genesis::placement::SampleSerializer::save ),
            ( boost::python::arg("map"), boost::python::arg("file_name") ),
            get_docstring("static void ::genesis::placement::SampleSerializer::save (const Sample & map, const std::string & file_name)")
        )
        .staticmethod("save")
    ;
}
