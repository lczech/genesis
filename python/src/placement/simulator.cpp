/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/simulator/simulator.hpp"
#include "lib/placement/simulator/twostep.hpp"
#include "lib/placement/simulator/subtree.hpp"

using namespace genesis::placement;

void BoostPythonExport_SimulatorTwostep()
{
    // -------------------------------------------------------------------
    //     Class SimulatorTwostep
    // -------------------------------------------------------------------

    boost::python::scope twostep = boost::python::class_< ::genesis::placement::SimulatorTwostep > ( "SimulatorTwostep", boost::python::init< Sample & >(( boost::python::arg("placements") )) )

        // Public Member Functions

        .def(
            "generate",
            ( void ( ::genesis::placement::SimulatorTwostep::* )( size_t ))( &::genesis::placement::SimulatorTwostep::generate ),
            ( boost::python::arg("n") ),
            get_docstring("void ::genesis::placement::SimulatorTwostep::generate (size_t n)")
        )
        .def(
            "edge_distribution",
            ( ::genesis::placement::SimulatorTwostep::EdgeDistribution & ( ::genesis::placement::SimulatorTwostep::* )(  ) )( &::genesis::placement::SimulatorTwostep::edge_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "pendant_length_distribution",
            ( ::genesis::placement::SimulatorTwostep::PendantLengthDistribution & ( ::genesis::placement::SimulatorTwostep::* )(  ))( &::genesis::placement::SimulatorTwostep::pendant_length_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "proximal_length_distribution",
            ( ::genesis::placement::SimulatorTwostep::ProximalLengthDistribution & ( ::genesis::placement::SimulatorTwostep::* )(  ))( &::genesis::placement::SimulatorTwostep::proximal_length_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
    ;

    // TODO the set_depths_distributed_weights method takes a vector. this way, it is not possible to use a py list
    //      as input. we need some wrapper for this, see http://stackoverflow.com/questions/3761391/boostpython-python-list-to-stdvector
    //     or  http://stackoverflow.com/questions/6190609/add-a-constructor-to-a-boostpython-vector-indexing-suite-exposed-class

    // -------------------------------------------------------------------
    //     Class SimulatorTwostep :: EdgeDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::SimulatorTwostep::EdgeDistribution > ( "EdgeDistribution", boost::python::init< Sample & >(( boost::python::arg("placements") )) )

        // Public Member Functions

        .def(
            "set_depths_distributed_weights",
            ( void ( ::genesis::placement::SimulatorTwostep::EdgeDistribution::* )( const std::vector< int > & ))( &::genesis::placement::SimulatorTwostep::EdgeDistribution::set_depths_distributed_weights ),
            ( boost::python::arg("depth_weights") )
        )
        .def(
            "set_random_weights",
            ( void ( ::genesis::placement::SimulatorTwostep::EdgeDistribution::* )(  ))( &::genesis::placement::SimulatorTwostep::EdgeDistribution::set_random_weights )
        )
        .def(
            "set_random_subtree_weights",
            ( void ( ::genesis::placement::SimulatorTwostep::EdgeDistribution::* )(  ))( &::genesis::placement::SimulatorTwostep::EdgeDistribution::set_random_subtree_weights )
        )
        .def(
            "set_uniform_weights",
            ( void ( ::genesis::placement::SimulatorTwostep::EdgeDistribution::* )(  ))( &::genesis::placement::SimulatorTwostep::EdgeDistribution::set_uniform_weights )
        )
        .def(
            "transfer_weights",
            ( bool ( ::genesis::placement::SimulatorTwostep::EdgeDistribution::* )( const Sample & ))( &::genesis::placement::SimulatorTwostep::EdgeDistribution::transfer_weights ),
            ( boost::python::arg("from_map") )
        )
    ;

    // -------------------------------------------------------------------
    //     Class SimulatorTwostep :: ProximalLengthDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::SimulatorTwostep::ProximalLengthDistribution > ( "ProximalLengthDistribution" )
    ;

    // -------------------------------------------------------------------
    //     Class SimulatorTwostep :: PendantLengthDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::SimulatorTwostep::PendantLengthDistribution > ( "PendantLengthDistribution" )

        .def_readwrite("min", &::genesis::placement::SimulatorTwostep::PendantLengthDistribution::min)
        .def_readwrite("max", &::genesis::placement::SimulatorTwostep::PendantLengthDistribution::max)
    ;
}

PYTHON_EXPORT_CLASS (Simulator, "placement")
{
    BoostPythonExport_SimulatorTwostep();
}
