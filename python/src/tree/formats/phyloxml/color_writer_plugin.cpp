/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::PhyloxmlColorWriterPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class PhyloxmlColorWriterPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::PhyloxmlColorWriterPlugin, std::shared_ptr<::genesis::tree::PhyloxmlColorWriterPlugin> > ( scope, "PhyloxmlColorWriterPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PhyloxmlColorWriterPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge_to_element",
            ( void ( ::genesis::tree::PhyloxmlColorWriterPlugin::* )( TreeEdge const &, utils::XmlElement & ) const )( &::genesis::tree::PhyloxmlColorWriterPlugin::edge_to_element ),
            pybind11::arg("edge"),
            pybind11::arg("element")
        )
        .def(
            "prepare_writing",
            ( void ( ::genesis::tree::PhyloxmlColorWriterPlugin::* )( Tree const &, utils::XmlDocument & ) const )( &::genesis::tree::PhyloxmlColorWriterPlugin::prepare_writing ),
            pybind11::arg("tree"),
            pybind11::arg("xml")
        )
        .def(
            "register_with",
            ( void ( ::genesis::tree::PhyloxmlColorWriterPlugin::* )( PhyloxmlWriter & ) const )( &::genesis::tree::PhyloxmlColorWriterPlugin::register_with ),
            pybind11::arg("writer")
        )
    ;
}
