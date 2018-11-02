/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::ColorWriterPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorWriterPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::ColorWriterPlugin, std::shared_ptr<::genesis::tree::ColorWriterPlugin> > ( scope, "ColorWriterPlugin" )

        // Public Member Functions

        .def(
            "edge_colors",
            ( std::vector< utils::Color > const & ( ::genesis::tree::ColorWriterPlugin::* )(  ) const )( &::genesis::tree::ColorWriterPlugin::edge_colors ),
            get_docstring("std::vector< utils::Color > const & ::genesis::tree::ColorWriterPlugin::edge_colors () const")
        )
        .def(
            "edge_colors",
            ( void ( ::genesis::tree::ColorWriterPlugin::* )( std::vector< utils::Color > const & ))( &::genesis::tree::ColorWriterPlugin::edge_colors ),
            pybind11::arg("color_vector"),
            get_docstring("void ::genesis::tree::ColorWriterPlugin::edge_colors (std::vector< utils::Color > const & color_vector)")
        )
        .def(
            "enable_color",
            ( bool ( ::genesis::tree::ColorWriterPlugin::* )(  ) const )( &::genesis::tree::ColorWriterPlugin::enable_color ),
            get_docstring("bool ::genesis::tree::ColorWriterPlugin::enable_color () const")
        )
        .def(
            "enable_color",
            ( void ( ::genesis::tree::ColorWriterPlugin::* )( bool ))( &::genesis::tree::ColorWriterPlugin::enable_color ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::tree::ColorWriterPlugin::enable_color (bool value)")
        )
        .def(
            "ignored_color",
            ( utils::Color ( ::genesis::tree::ColorWriterPlugin::* )(  ) const )( &::genesis::tree::ColorWriterPlugin::ignored_color ),
            get_docstring("utils::Color ::genesis::tree::ColorWriterPlugin::ignored_color () const")
        )
        .def(
            "ignored_color",
            ( void ( ::genesis::tree::ColorWriterPlugin::* )( utils::Color ))( &::genesis::tree::ColorWriterPlugin::ignored_color ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::tree::ColorWriterPlugin::ignored_color (utils::Color value)")
        )
        .def(
            "use_ignored_color",
            ( bool ( ::genesis::tree::ColorWriterPlugin::* )(  ) const )( &::genesis::tree::ColorWriterPlugin::use_ignored_color ),
            get_docstring("bool ::genesis::tree::ColorWriterPlugin::use_ignored_color () const")
        )
        .def(
            "use_ignored_color",
            ( void ( ::genesis::tree::ColorWriterPlugin::* )( bool ))( &::genesis::tree::ColorWriterPlugin::use_ignored_color ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::tree::ColorWriterPlugin::use_ignored_color (bool value)")
        )
    ;
}
