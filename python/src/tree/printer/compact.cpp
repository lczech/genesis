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
PYTHON_EXPORT_CLASS (PrinterCompact, "tree")
{

    // -------------------------------------------------------------------
    //     Class PrinterCompact
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::PrinterCompact > ( "PrinterCompact", boost::python::init<  >(  ) )
        .def( boost::python::init< PrinterCompact const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterCompact::* )( TreeType const & ))( &::genesis::tree::PrinterCompact::print ),
            ( boost::python::arg("tree") ),
            get_docstring("std::string ::genesis::tree::PrinterCompact::print (TreeType const & tree)")
        )
        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterCompact::* )( TreeType const &, std::function< std::string(typename TreeType::NodeType const &node, typename TreeType::EdgeType const &edge)> const ))( &::genesis::tree::PrinterCompact::print ),
            ( boost::python::arg("tree"), boost::python::arg("print_line") )
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterCompact::* )( std::ostream &, TreeType const &, std::function< std::string(typename TreeType::NodeType const &node, typename TreeType::EdgeType const &edge)> const ))( &::genesis::tree::PrinterCompact::print ),
            ( boost::python::arg("out"), boost::python::arg("tree"), boost::python::arg("print_line") ),
            get_docstring("void ::genesis::tree::PrinterCompact::print (std::ostream & out, TreeType const & tree, std::function< std::string(typename TreeType::NodeType const &node, typename TreeType::EdgeType const &edge)> const print_line)")
        )
    ;
}
*/
