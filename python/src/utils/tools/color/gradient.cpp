/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_gradient_export, ::genesis::utils, scope )
{

    scope.def(
        "gradient",
        ( Color ( * )( std::map< double, Color > const &, double ))( &::genesis::utils::gradient ),
            pybind11::arg("ranges"),
            pybind11::arg("percentage")
    );

    scope.def(
        "heat_gradient",
        ( Color ( * )( double ))( &::genesis::utils::heat_gradient ),
            pybind11::arg("percentage")
    );

    scope.def(
        "interpolate",
        ( Color ( * )( Color, Color, double ))( &::genesis::utils::interpolate ),
            pybind11::arg("color1"),
            pybind11::arg("color2"),
            pybind11::arg("fraction")
    );
}
