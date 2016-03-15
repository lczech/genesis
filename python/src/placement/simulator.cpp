/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/simulator.hpp"

using namespace genesis::placement;

void BoostPythonExport_PlacementSimulatorTwostep()
{
    // -------------------------------------------------------------------
    //     Class PlacementSimulatorTwostep
    // -------------------------------------------------------------------

    boost::python::scope twostep = boost::python::class_< ::genesis::placement::PlacementSimulatorTwostep > ( "PlacementSimulatorTwostep", boost::python::init< Sample & >(( boost::python::arg("placements") )) )

        // Public Member Functions

        .def(
            "generate",
            ( void ( ::genesis::placement::PlacementSimulatorTwostep::* )( size_t ))( &::genesis::placement::PlacementSimulatorTwostep::generate ),
            ( boost::python::arg("n") ),
            get_docstring("void ::genesis::placement::PlacementSimulatorTwostep::generate (size_t n)")
        )
        .def(
            "edge_distribution",
            ( ::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution & ( ::genesis::placement::PlacementSimulatorTwostep::* )(  ) )( &::genesis::placement::PlacementSimulatorTwostep::edge_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "pendant_length_distribution",
            ( ::genesis::placement::PlacementSimulatorTwostep::PendantLengthDistribution & ( ::genesis::placement::PlacementSimulatorTwostep::* )(  ))( &::genesis::placement::PlacementSimulatorTwostep::pendant_length_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "proximal_length_distribution",
            ( ::genesis::placement::PlacementSimulatorTwostep::ProximalLengthDistribution & ( ::genesis::placement::PlacementSimulatorTwostep::* )(  ))( &::genesis::placement::PlacementSimulatorTwostep::proximal_length_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
    ;

    // TODO the set_depths_distributed_weights method takes a vector. this way, it is not possible to use a py list
    //      as input. we need some wrapper for this, see http://stackoverflow.com/questions/3761391/boostpython-python-list-to-stdvector
    //     or  http://stackoverflow.com/questions/6190609/add-a-constructor-to-a-boostpython-vector-indexing-suite-exposed-class

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorTwostep :: EdgeDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution > ( "EdgeDistribution", boost::python::init< Sample & >(( boost::python::arg("placements") )) )

        // Public Member Functions

        .def(
            "set_depths_distributed_weights",
            ( void ( ::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::* )( const std::vector< int > & ))( &::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::set_depths_distributed_weights ),
            ( boost::python::arg("depth_weights") )
        )
        .def(
            "set_random_weights",
            ( void ( ::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::* )(  ))( &::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::set_random_weights )
        )
        .def(
            "set_random_subtree_weights",
            ( void ( ::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::* )(  ))( &::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::set_random_subtree_weights )
        )
        .def(
            "set_uniform_weights",
            ( void ( ::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::* )(  ))( &::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::set_uniform_weights )
        )
        .def(
            "transfer_weights",
            ( bool ( ::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::* )( const Sample & ))( &::genesis::placement::PlacementSimulatorTwostep::EdgeDistribution::transfer_weights ),
            ( boost::python::arg("from_map") )
        )
    ;

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorTwostep :: ProximalLengthDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PlacementSimulatorTwostep::ProximalLengthDistribution > ( "ProximalLengthDistribution" )
    ;

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorTwostep :: PendantLengthDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PlacementSimulatorTwostep::PendantLengthDistribution > ( "PendantLengthDistribution" )

        .def_readwrite("min", &::genesis::placement::PlacementSimulatorTwostep::PendantLengthDistribution::min)
        .def_readwrite("max", &::genesis::placement::PlacementSimulatorTwostep::PendantLengthDistribution::max)
    ;
}

PYTHON_EXPORT_CLASS (PlacementSimulator, "placement")
{
    BoostPythonExport_PlacementSimulatorTwostep();
}
