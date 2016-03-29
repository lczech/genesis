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

/*
PYTHON_EXPORT_CLASS (SimulatorTwostep, "placement")
{

    // -------------------------------------------------------------------
    //     Class SimulatorTwostep
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::SimulatorTwostep > ( "SimulatorTwostep", boost::python::init< Sample & >(( boost::python::arg("smp") )) )

        // Public Member Functions

        .def(
            "edge_distribution",
            ( EdgeDistribution & ( ::genesis::placement::SimulatorTwostep::* )(  ))( &::genesis::placement::SimulatorTwostep::edge_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "generate",
            ( void ( ::genesis::placement::SimulatorTwostep::* )( size_t ))( &::genesis::placement::SimulatorTwostep::generate ),
            ( boost::python::arg("n") ),
            get_docstring("void ::genesis::placement::SimulatorTwostep::generate (size_t n)")
        )
        .def(
            "pendant_length_distribution",
            ( PendantLengthDistribution & ( ::genesis::placement::SimulatorTwostep::* )(  ))( &::genesis::placement::SimulatorTwostep::pendant_length_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "proximal_length_distribution",
            ( ProximalLengthDistribution & ( ::genesis::placement::SimulatorTwostep::* )(  ))( &::genesis::placement::SimulatorTwostep::proximal_length_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
    ;
}
*/
