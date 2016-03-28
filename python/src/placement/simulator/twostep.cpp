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
