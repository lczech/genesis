/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

/*
PYTHON_EXPORT_CLASS( ::genesis::tree::ColorWriterMixin, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorWriterMixin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::ColorWriterMixin, std::shared_ptr<::genesis::tree::ColorWriterMixin> > ( scope, "ColorWriterMixin" )

        // Public Member Functions

        .def(
            "edge_colors",
            ( std::vector< utils::Color > ( ::genesis::tree::ColorWriterMixin::* )(  ))( &::genesis::tree::ColorWriterMixin::edge_colors ),
            get_docstring("std::vector< utils::Color > ::genesis::tree::ColorWriterMixin::edge_colors ()")
        )
        .def(
            "edge_colors",
            ( void ( ::genesis::tree::ColorWriterMixin::* )( std::vector< utils::Color > const & ))( &::genesis::tree::ColorWriterMixin::edge_colors ),
            pybind11::arg("color_vector"),
            get_docstring("void ::genesis::tree::ColorWriterMixin::edge_colors (std::vector< utils::Color > const & color_vector)")
        )
        .def(
            "enable_color",
            ( bool ( ::genesis::tree::ColorWriterMixin::* )(  ) const )( &::genesis::tree::ColorWriterMixin::enable_color ),
            get_docstring("bool ::genesis::tree::ColorWriterMixin::enable_color () const")
        )
        .def(
            "enable_color",
            ( void ( ::genesis::tree::ColorWriterMixin::* )( bool ))( &::genesis::tree::ColorWriterMixin::enable_color ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::tree::ColorWriterMixin::enable_color (bool value)")
        )
        .def(
            "ignored_color",
            ( utils::Color ( ::genesis::tree::ColorWriterMixin::* )(  ) const )( &::genesis::tree::ColorWriterMixin::ignored_color ),
            get_docstring("utils::Color ::genesis::tree::ColorWriterMixin::ignored_color () const")
        )
        .def(
            "ignored_color",
            ( void ( ::genesis::tree::ColorWriterMixin::* )( utils::Color ))( &::genesis::tree::ColorWriterMixin::ignored_color ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::tree::ColorWriterMixin::ignored_color (utils::Color value)")
        )
        .def(
            "use_ignored_color",
            ( bool ( ::genesis::tree::ColorWriterMixin::* )(  ) const )( &::genesis::tree::ColorWriterMixin::use_ignored_color ),
            get_docstring("bool ::genesis::tree::ColorWriterMixin::use_ignored_color () const")
        )
        .def(
            "use_ignored_color",
            ( void ( ::genesis::tree::ColorWriterMixin::* )( bool ))( &::genesis::tree::ColorWriterMixin::use_ignored_color ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::tree::ColorWriterMixin::use_ignored_color (bool value)")
        )
    ;
}
*/
