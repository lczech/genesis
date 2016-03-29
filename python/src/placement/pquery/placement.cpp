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

PYTHON_EXPORT_CLASS (PqueryPlacement, "placement")
{

    // -------------------------------------------------------------------
    //     Class PqueryPlacement
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PqueryPlacement > ( "PqueryPlacement", boost::python::init<  >(  ) )
        .def( boost::python::init< PlacementTreeEdge & >(( boost::python::arg("edge") )) )
        .def( boost::python::init< PqueryPlacement const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "edge",
            ( PlacementTreeEdge & ( ::genesis::placement::PqueryPlacement::* )(  ))( &::genesis::placement::PqueryPlacement::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PlacementTreeEdge & ::genesis::placement::PqueryPlacement::edge ()")
        )
        .def(
            "edge",
            ( const PlacementTreeEdge & ( ::genesis::placement::PqueryPlacement::* )(  ) const )( &::genesis::placement::PqueryPlacement::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("const PlacementTreeEdge & ::genesis::placement::PqueryPlacement::edge () const")
        )
        .def(
            "edge_num",
            ( int ( ::genesis::placement::PqueryPlacement::* )(  ) const )( &::genesis::placement::PqueryPlacement::edge_num ),
            get_docstring("int ::genesis::placement::PqueryPlacement::edge_num () const")
        )
        .def(
            "reset_edge",
            ( void ( ::genesis::placement::PqueryPlacement::* )( PlacementTreeEdge & ))( &::genesis::placement::PqueryPlacement::reset_edge ),
            ( boost::python::arg("edge") ),
            get_docstring("void ::genesis::placement::PqueryPlacement::reset_edge (PlacementTreeEdge & edge)")
        )
    ;
}
