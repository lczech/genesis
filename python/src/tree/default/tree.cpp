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

PYTHON_EXPORT_CLASS (DefaultTreeNodeData, "tree")
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeNodeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::DefaultTreeNodeData > ( "DefaultTreeNodeData" )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::tree::DefaultTreeNodeData::* )(  ) const )( &::genesis::tree::DefaultTreeNodeData::dump )
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}

PYTHON_EXPORT_CLASS (DefaultTreeEdgeData, "tree")
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeEdgeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::DefaultTreeEdgeData > ( "DefaultTreeEdgeData" )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::tree::DefaultTreeEdgeData::* )(  ) const )( &::genesis::tree::DefaultTreeEdgeData::dump )
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}
