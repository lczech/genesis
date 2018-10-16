/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::PlacementTreeNewickReaderPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickReaderPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PlacementTreeNewickReaderPlugin, std::shared_ptr<::genesis::placement::PlacementTreeNewickReaderPlugin> > ( scope, "PlacementTreeNewickReaderPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PlacementTreeNewickReaderPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "element_to_edge",
            ( void ( ::genesis::placement::PlacementTreeNewickReaderPlugin::* )( tree::NewickBrokerElement const &, tree::TreeEdge & ) const )( &::genesis::placement::PlacementTreeNewickReaderPlugin::element_to_edge ),
            pybind11::arg("element"),
            pybind11::arg("edge")
        )
        .def(
            "finish_reading",
            ( void ( ::genesis::placement::PlacementTreeNewickReaderPlugin::* )( tree::NewickBroker const &, tree::Tree & ) const )( &::genesis::placement::PlacementTreeNewickReaderPlugin::finish_reading ),
            pybind11::arg("broker"),
            pybind11::arg("tree")
        )
        .def(
            "register_with",
            ( void ( ::genesis::placement::PlacementTreeNewickReaderPlugin::* )( tree::NewickReader & ) const )( &::genesis::placement::PlacementTreeNewickReaderPlugin::register_with ),
            pybind11::arg("reader")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::PlacementTreeNewickReader, scope )
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PlacementTreeNewickReader, std::shared_ptr<::genesis::placement::PlacementTreeNewickReader> > ( scope, "PlacementTreeNewickReader" )
        .def(
            pybind11::init<  >()
        )
    ;
}
