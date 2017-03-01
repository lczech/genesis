/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_operators_export, ::genesis::utils, scope )
{

    scope.def(
        "color_from_doubles",
        ( Color ( * )( double, double, double ))( &::genesis::utils::color_from_doubles ),
            pybind11::arg("r"),
            pybind11::arg("g"),
            pybind11::arg("b")
    );

    scope.def(
        "color_from_hex",
        ( Color ( * )( std::string, std::string ))( &::genesis::utils::color_from_hex ),
            pybind11::arg("h"),
            pybind11::arg("prefix")=(std::string)("#")
    );

    scope.def(
        "color_to_hex",
        ( std::string ( * )( const Color &, std::string, bool ))( &::genesis::utils::color_to_hex ),
            pybind11::arg("c"),
            pybind11::arg("prefix")=(std::string)("#"),
            pybind11::arg("uppercase")=(bool)(false)
    );
}
