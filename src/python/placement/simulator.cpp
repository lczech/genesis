/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>
#include "placement/simulator.hpp"

const char* get_docstring (const std::string& signature);

void BoostPythonExport_PlacementSimulatorEdgeDistribution()
{
    using namespace genesis;

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorEdgeDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementSimulatorEdgeDistribution > ( "PlacementSimulatorEdgeDistribution" )

        // Public Member Functions

        .def(
            "generate",
            ( size_t ( ::genesis::PlacementSimulatorEdgeDistribution::* )(  ))( &::genesis::PlacementSimulatorEdgeDistribution::generate ),
            get_docstring("size_t ::genesis::PlacementSimulatorEdgeDistribution::generate ()")
        )
        .def(
            "prepare",
            ( size_t ( ::genesis::PlacementSimulatorEdgeDistribution::* )(  ))( &::genesis::PlacementSimulatorEdgeDistribution::prepare ),
            get_docstring("size_t ::genesis::PlacementSimulatorEdgeDistribution::prepare ()")
        )
        .def(
            "set_depths_distributed_weights",
            ( void ( ::genesis::PlacementSimulatorEdgeDistribution::* )( const PlacementMap &, const std::vector< int > & ))( &::genesis::PlacementSimulatorEdgeDistribution::set_depths_distributed_weights ),
            ( boost::python::arg("map"), boost::python::arg("depth_weights") ),
            get_docstring("void ::genesis::PlacementSimulatorEdgeDistribution::set_depths_distributed_weights (const PlacementMap & map, const std::vector< int > & depth_weights)")
        )
        .def(
            "set_transferred_weights",
            ( void ( ::genesis::PlacementSimulatorEdgeDistribution::* )( const PlacementMap & ))( &::genesis::PlacementSimulatorEdgeDistribution::set_transferred_weights ),
            ( boost::python::arg("from_map") ),
            get_docstring("void ::genesis::PlacementSimulatorEdgeDistribution::set_transferred_weights (const PlacementMap & from_map)")
        )
        .def(
            "set_uniform_weights",
            ( void ( ::genesis::PlacementSimulatorEdgeDistribution::* )( const PlacementMap & ))( &::genesis::PlacementSimulatorEdgeDistribution::set_uniform_weights ),
            ( boost::python::arg("map") ),
            get_docstring("void ::genesis::PlacementSimulatorEdgeDistribution::set_uniform_weights (const PlacementMap & map)")
        )
        .def(
            "set_uniform_weights",
            ( void ( ::genesis::PlacementSimulatorEdgeDistribution::* )( const size_t ))( &::genesis::PlacementSimulatorEdgeDistribution::set_uniform_weights ),
            ( boost::python::arg("num_edges") ),
            get_docstring("void ::genesis::PlacementSimulatorEdgeDistribution::set_uniform_weights (const size_t num_edges)")
        )
    ;
}

void BoostPythonExport_PlacementSimulatorPositionDistribution()
{
    using namespace genesis;

    // -------------------------------------------------------------------
    //     Class PlacementSimulatorPositionDistribution
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementSimulatorPositionDistribution > ( "PlacementSimulatorPositionDistribution" )

        // Public Member Functions

        .def(
            "generate",
            ( double ( ::genesis::PlacementSimulatorPositionDistribution::* )( typename PlacementTree::EdgeType * ))( &::genesis::PlacementSimulatorPositionDistribution::generate ),
            ( boost::python::arg("edge") ),
            get_docstring("double ::genesis::PlacementSimulatorPositionDistribution::generate (typename PlacementTree::EdgeType * edge)")
        )
        .def(
            "prepare",
            ( void ( ::genesis::PlacementSimulatorPositionDistribution::* )(  ))( &::genesis::PlacementSimulatorPositionDistribution::prepare ),
            get_docstring("void ::genesis::PlacementSimulatorPositionDistribution::prepare ()")
        )
    ;
}

void BoostPythonExport_PlacementSimulator()
{
    BoostPythonExport_PlacementSimulatorEdgeDistribution();
    BoostPythonExport_PlacementSimulatorPositionDistribution();

    using namespace genesis;

    // -------------------------------------------------------------------
    //     Class PlacementSimulator
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementSimulator > ( "PlacementSimulator" )

        // Public Member Functions

        // .def(
        //     "generate_in_subtree",
        //     ( void ( ::genesis::PlacementSimulator::* )( PlacementMap &, size_t ))( &::genesis::PlacementSimulator::generate_in_subtree ),
        //     ( boost::python::arg("placements"), boost::python::arg("n") )
        // )
        .def(
            "generate_two_step",
            ( void ( ::genesis::PlacementSimulator::* )( PlacementMap &, size_t ))( &::genesis::PlacementSimulator::generate_two_step ),
            ( boost::python::arg("placements"), boost::python::arg("n") ),
            get_docstring("void ::genesis::PlacementSimulator::generate_two_step (PlacementMap & placements, size_t n)")
        )
    ;
}
