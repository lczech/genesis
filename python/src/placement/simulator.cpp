/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>
#include "lib/placement/simulator.hpp"

const char* get_docstring (const std::string& signature);

void BoostPythonExport_PlacementSimulatorTwostep()
{
    using namespace genesis;

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorTwostep
    // -------------------------------------------------------------------

    boost::python::scope twostep = boost::python::class_< ::genesis::PlacementSimulatorTwostep > ( "PlacementSimulatorTwostep", boost::python::init< PlacementMap & >(( boost::python::arg("placements") )) )

        // Public Member Functions

        .def(
            "generate",
            ( void ( ::genesis::PlacementSimulatorTwostep::* )( size_t ))( &::genesis::PlacementSimulatorTwostep::generate ),
            ( boost::python::arg("n") ),
            get_docstring("void ::genesis::PlacementSimulatorTwostep::generate (size_t n)")
        )
        .def(
            "edge_distribution",
            ( ::genesis::PlacementSimulatorTwostep::EdgeDistribution & ( ::genesis::PlacementSimulatorTwostep::* )(  ) )( &::genesis::PlacementSimulatorTwostep::edge_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "pendant_length_distribution",
            ( ::genesis::PlacementSimulatorTwostep::PendantLengthDistribution & ( ::genesis::PlacementSimulatorTwostep::* )(  ))( &::genesis::PlacementSimulatorTwostep::pendant_length_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "proximal_length_distribution",
            ( ::genesis::PlacementSimulatorTwostep::ProximalLengthDistribution & ( ::genesis::PlacementSimulatorTwostep::* )(  ))( &::genesis::PlacementSimulatorTwostep::proximal_length_distribution ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
    ;

    // TODO the set_depths_distributed_weights method takes a vector. this way, it is not possible to use a py list
    //      as input. we need some wrapper for this, see http://stackoverflow.com/questions/3761391/boostpython-python-list-to-stdvector
    //     or  http://stackoverflow.com/questions/6190609/add-a-constructor-to-a-boostpython-vector-indexing-suite-exposed-class

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorTwostep :: EdgeDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementSimulatorTwostep::EdgeDistribution > ( "EdgeDistribution", boost::python::init< PlacementMap & >(( boost::python::arg("placements") )) )

        // Public Member Functions

        .def(
            "set_depths_distributed_weights",
            ( void ( ::genesis::PlacementSimulatorTwostep::EdgeDistribution::* )( const std::vector< int > & ))( &::genesis::PlacementSimulatorTwostep::EdgeDistribution::set_depths_distributed_weights ),
            ( boost::python::arg("depth_weights") )
        )
        .def(
            "set_uniform_weights",
            ( void ( ::genesis::PlacementSimulatorTwostep::EdgeDistribution::* )(  ))( &::genesis::PlacementSimulatorTwostep::EdgeDistribution::set_uniform_weights )
        )
        .def(
            "transfer_weights",
            ( bool ( ::genesis::PlacementSimulatorTwostep::EdgeDistribution::* )( const PlacementMap & ))( &::genesis::PlacementSimulatorTwostep::EdgeDistribution::transfer_weights ),
            ( boost::python::arg("from_map") )
        )
    ;

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorTwostep :: ProximalLengthDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementSimulatorTwostep::ProximalLengthDistribution > ( "ProximalLengthDistribution" )
    ;

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorTwostep :: PendantLengthDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementSimulatorTwostep::PendantLengthDistribution > ( "PendantLengthDistribution" )

        .def_readwrite("min", &::genesis::PlacementSimulatorTwostep::PendantLengthDistribution::min)
        .def_readwrite("max", &::genesis::PlacementSimulatorTwostep::PendantLengthDistribution::max)
    ;
}

void BoostPythonExport_PlacementSimulator()
{
    BoostPythonExport_PlacementSimulatorTwostep();
}
