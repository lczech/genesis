/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgGradientLinear, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgGradientLinear
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgGradientLinear, std::shared_ptr<::genesis::utils::SvgGradientLinear> > ( scope, "SvgGradientLinear" )
        .def(
            pybind11::init< std::string, SvgPoint, SvgPoint >(),
            pybind11::arg("id"),
            pybind11::arg("point_1"),
            pybind11::arg("point_2"),
            get_docstring("::genesis::utils::SvgGradientLinear::SvgGradientLinear (std::string id, SvgPoint point_1, SvgPoint point_2)")
        )
        .def(
            pybind11::init< SvgGradientLinear const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "set_stops",
            ( SvgGradientLinear & ( ::genesis::utils::SvgGradientLinear::* )( std::map< double, Color > const & ))( &::genesis::utils::SvgGradientLinear::set_stops ),
            pybind11::arg("ranges")
        )
        .def(
            "validate",
            ( void ( ::genesis::utils::SvgGradientLinear::* )(  ) const )( &::genesis::utils::SvgGradientLinear::validate )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgGradientLinear::* )( std::ostream &, size_t ) const )( &::genesis::utils::SvgGradientLinear::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0)
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgGradientStop, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgGradientStop
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgGradientStop, std::shared_ptr<::genesis::utils::SvgGradientStop> > ( scope, "SvgGradientStop" )
        .def(
            pybind11::init< double, Color const &, double >(),
            pybind11::arg("offset"),
            pybind11::arg("stop_color"),
            pybind11::arg("stop_opacity")=(double)(1.0)
        )
        .def(
            pybind11::init< SvgGradientStop const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "validate",
            ( void ( ::genesis::utils::SvgGradientStop::* )(  ) const )( &::genesis::utils::SvgGradientStop::validate )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgGradientStop::* )( std::ostream & ) const )( &::genesis::utils::SvgGradientStop::write ),
            pybind11::arg("out")
        )
    ;
}
