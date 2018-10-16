/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_functions_export, ::genesis::utils, scope )
{

    scope.def(
        "color_from_bytes",
        ( Color ( * )( unsigned char, unsigned char, unsigned char, unsigned char ))( &::genesis::utils::color_from_bytes ),
            pybind11::arg("r"),
            pybind11::arg("g"),
            pybind11::arg("b"),
            pybind11::arg("a")=(unsigned char)(255),
        get_docstring("Color ::genesis::utils::color_from_bytes (unsigned char r, unsigned char g, unsigned char b, unsigned char a=255)")
    );

    scope.def(
        "color_from_hex",
        ( Color ( * )( std::string const &, std::string const & ))( &::genesis::utils::color_from_hex ),
            pybind11::arg("hex_color"),
            pybind11::arg("prefix")=(std::string const &)("#"),
        get_docstring("Color ::genesis::utils::color_from_hex (std::string const & hex_color, std::string const & prefix=\"#\")")
    );

    scope.def(
        "gradient",
        ( Color ( * )( std::map< double, Color > const &, double ))( &::genesis::utils::gradient ),
            pybind11::arg("ranges"),
            pybind11::arg("value"),
        get_docstring("Color ::genesis::utils::gradient (std::map< double, Color > const & ranges, double value)")
    );

    scope.def(
        "heat_gradient",
        ( Color ( * )( double ))( &::genesis::utils::heat_gradient ),
            pybind11::arg("percentage"),
        get_docstring("Color ::genesis::utils::heat_gradient (double percentage)")
    );

    scope.def(
        "interpolate",
        ( Color ( * )( Color const &, Color const &, double ))( &::genesis::utils::interpolate ),
            pybind11::arg("color1"),
            pybind11::arg("color2"),
            pybind11::arg("fraction"),
        get_docstring("Color ::genesis::utils::interpolate (Color const & color1, Color const & color2, double fraction)")
    );

    scope.def(
        "resolve_color_string",
        ( Color ( * )( std::string const & ))( &::genesis::utils::resolve_color_string ),
            pybind11::arg("color_str"),
        get_docstring("Color ::genesis::utils::resolve_color_string (std::string const & color_str)")
    );

    scope.def(
        "color_to_hex",
        ( std::string ( * )( Color const &, std::string const &, bool, bool ))( &::genesis::utils::color_to_hex ),
            pybind11::arg("c"),
            pybind11::arg("prefix")=(std::string const &)("#"),
            pybind11::arg("uppercase")=(bool)(false),
            pybind11::arg("with_alpha")=(bool)(false),
        get_docstring("std::string ::genesis::utils::color_to_hex (Color const & c, std::string const & prefix=\"#\", bool uppercase=false, bool with_alpha=false)")
    );
}
