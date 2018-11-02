/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::NewickColorWriterPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class NewickColorWriterPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::NewickColorWriterPlugin, std::shared_ptr<::genesis::tree::NewickColorWriterPlugin> > ( scope, "NewickColorWriterPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NewickColorWriterPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "color_tag_prefix",
            ( std::string ( ::genesis::tree::NewickColorWriterPlugin::* )(  ) const )( &::genesis::tree::NewickColorWriterPlugin::color_tag_prefix ),
            get_docstring("std::string ::genesis::tree::NewickColorWriterPlugin::color_tag_prefix () const")
        )
        .def(
            "color_tag_prefix",
            ( void ( ::genesis::tree::NewickColorWriterPlugin::* )( std::string ))( &::genesis::tree::NewickColorWriterPlugin::color_tag_prefix ),
            pybind11::arg("prefix"),
            get_docstring("void ::genesis::tree::NewickColorWriterPlugin::color_tag_prefix (std::string prefix)")
        )
        .def(
            "color_tag_suffix",
            ( std::string ( ::genesis::tree::NewickColorWriterPlugin::* )(  ) const )( &::genesis::tree::NewickColorWriterPlugin::color_tag_suffix ),
            get_docstring("std::string ::genesis::tree::NewickColorWriterPlugin::color_tag_suffix () const")
        )
        .def(
            "color_tag_suffix",
            ( void ( ::genesis::tree::NewickColorWriterPlugin::* )( std::string ))( &::genesis::tree::NewickColorWriterPlugin::color_tag_suffix ),
            pybind11::arg("suffix"),
            get_docstring("void ::genesis::tree::NewickColorWriterPlugin::color_tag_suffix (std::string suffix)")
        )
        .def(
            "edge_to_element",
            ( void ( ::genesis::tree::NewickColorWriterPlugin::* )( TreeEdge const &, NewickBrokerElement & ) const )( &::genesis::tree::NewickColorWriterPlugin::edge_to_element ),
            pybind11::arg("edge"),
            pybind11::arg("element")
        )
        .def(
            "prepare_writing",
            ( void ( ::genesis::tree::NewickColorWriterPlugin::* )( Tree const &, NewickBroker & ) const )( &::genesis::tree::NewickColorWriterPlugin::prepare_writing ),
            pybind11::arg("tree"),
            pybind11::arg("broker")
        )
        .def(
            "register_with",
            ( void ( ::genesis::tree::NewickColorWriterPlugin::* )( NewickWriter & ) const )( &::genesis::tree::NewickColorWriterPlugin::register_with ),
            pybind11::arg("writer")
        )
    ;
}
