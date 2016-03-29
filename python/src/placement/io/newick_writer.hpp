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

template <typename Base>
void PythonExportClass_PlacementTreeNewickWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::placement;

    using PlacementTreeNewickWriterMixinType = PlacementTreeNewickWriterMixin<typename Base>;

    boost::python::class_< PlacementTreeNewickWriterMixinType > ( name.c_str() )

        // Public Member Functions

        .def(
            "enable_edge_nums",
            ( bool ( PlacementTreeNewickWriterMixinType::* )(  ) const )( &PlacementTreeNewickWriterMixinType::enable_edge_nums )
        )
        .def(
            "enable_edge_nums",
            ( void ( PlacementTreeNewickWriterMixinType::* )( bool ))( &PlacementTreeNewickWriterMixinType::enable_edge_nums ),
            ( boost::python::arg("value") )
        )
        .def(
            "enable_placement_counts",
            ( bool ( PlacementTreeNewickWriterMixinType::* )(  ) const )( &PlacementTreeNewickWriterMixinType::enable_placement_counts )
        )
        .def(
            "enable_placement_counts",
            ( void ( PlacementTreeNewickWriterMixinType::* )( bool ))( &PlacementTreeNewickWriterMixinType::enable_placement_counts ),
            ( boost::python::arg("value") )
        )
        .def(
            "prepare_sample",
            ( void ( PlacementTreeNewickWriterMixinType::* )( Sample const & ))( &PlacementTreeNewickWriterMixinType::prepare_sample ),
            ( boost::python::arg("smp") )
        )
    ;
}
