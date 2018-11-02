/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::CommonTreePhyloxmlWriterPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class CommonTreePhyloxmlWriterPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CommonTreePhyloxmlWriterPlugin, std::shared_ptr<::genesis::tree::CommonTreePhyloxmlWriterPlugin> > ( scope, "CommonTreePhyloxmlWriterPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< CommonTreePhyloxmlWriterPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge_to_element",
            ( void ( ::genesis::tree::CommonTreePhyloxmlWriterPlugin::* )( TreeEdge const &, utils::XmlElement & ) const )( &::genesis::tree::CommonTreePhyloxmlWriterPlugin::edge_to_element ),
            pybind11::arg("edge"),
            pybind11::arg("element")
        )
        .def(
            "node_to_element",
            ( void ( ::genesis::tree::CommonTreePhyloxmlWriterPlugin::* )( TreeNode const &, utils::XmlElement & ) const )( &::genesis::tree::CommonTreePhyloxmlWriterPlugin::node_to_element ),
            pybind11::arg("node"),
            pybind11::arg("element")
        )
        .def(
            "register_with",
            ( void ( ::genesis::tree::CommonTreePhyloxmlWriterPlugin::* )( PhyloxmlWriter & ) const )( &::genesis::tree::CommonTreePhyloxmlWriterPlugin::register_with ),
            pybind11::arg("writer")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::CommonTreePhyloxmlWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class CommonTreePhyloxmlWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CommonTreePhyloxmlWriter, std::shared_ptr<::genesis::tree::CommonTreePhyloxmlWriter> > ( scope, "CommonTreePhyloxmlWriter" )
        .def(
            pybind11::init<  >()
        )
    ;
}
