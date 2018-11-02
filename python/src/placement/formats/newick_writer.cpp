/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::PlacementTreeNewickWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PlacementTreeNewickWriter, std::shared_ptr<::genesis::placement::PlacementTreeNewickWriter> > ( scope, "PlacementTreeNewickWriter" )
        .def(
            pybind11::init<  >()
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::PlacementTreeNewickWriterPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickWriterPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PlacementTreeNewickWriterPlugin, std::shared_ptr<::genesis::placement::PlacementTreeNewickWriterPlugin> > ( scope, "PlacementTreeNewickWriterPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PlacementTreeNewickWriterPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge_to_element",
            ( void ( ::genesis::placement::PlacementTreeNewickWriterPlugin::* )( tree::TreeEdge const &, tree::NewickBrokerElement & ) const )( &::genesis::placement::PlacementTreeNewickWriterPlugin::edge_to_element ),
            pybind11::arg("edge"),
            pybind11::arg("element")
        )
        .def(
            "enable_edge_nums",
            ( bool ( ::genesis::placement::PlacementTreeNewickWriterPlugin::* )(  ) const )( &::genesis::placement::PlacementTreeNewickWriterPlugin::enable_edge_nums )
        )
        .def(
            "enable_edge_nums",
            ( void ( ::genesis::placement::PlacementTreeNewickWriterPlugin::* )( bool ))( &::genesis::placement::PlacementTreeNewickWriterPlugin::enable_edge_nums ),
            pybind11::arg("value")
        )
        .def(
            "enable_placement_counts",
            ( bool ( ::genesis::placement::PlacementTreeNewickWriterPlugin::* )(  ) const )( &::genesis::placement::PlacementTreeNewickWriterPlugin::enable_placement_counts )
        )
        .def(
            "enable_placement_counts",
            ( void ( ::genesis::placement::PlacementTreeNewickWriterPlugin::* )( bool ))( &::genesis::placement::PlacementTreeNewickWriterPlugin::enable_placement_counts ),
            pybind11::arg("value")
        )
        .def(
            "prepare_sample",
            ( void ( ::genesis::placement::PlacementTreeNewickWriterPlugin::* )( Sample const & ))( &::genesis::placement::PlacementTreeNewickWriterPlugin::prepare_sample ),
            pybind11::arg("smp")
        )
        .def(
            "register_with",
            ( void ( ::genesis::placement::PlacementTreeNewickWriterPlugin::* )( tree::NewickWriter & ) const )( &::genesis::placement::PlacementTreeNewickWriterPlugin::register_with ),
            pybind11::arg("writer")
        )
    ;
}
