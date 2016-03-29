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

using namespace ::genesis::tree;

/*
PYTHON_EXPORT_CLASS (PrinterTable, "tree")
{

    // -------------------------------------------------------------------
    //     Class PrinterTable
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::PrinterTable > ( "PrinterTable", boost::python::init<  >(  ) )
        .def( boost::python::init< PrinterTable const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterTable::* )( TreeType const & ))( &::genesis::tree::PrinterTable::print ),
            ( boost::python::arg("tree") )
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterTable::* )( std::ostream &, TreeType const & ))( &::genesis::tree::PrinterTable::print ),
            ( boost::python::arg("out"), boost::python::arg("tree") )
        )
    ;
}
*/
