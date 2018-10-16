/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_names_export, ::genesis::utils, scope )
{

    scope.def(
        "color_from_name",
        ( Color ( * )( std::string const & ))( &::genesis::utils::color_from_name ),
            pybind11::arg("name"),
        get_docstring("Color ::genesis::utils::color_from_name (std::string const & name)")
    );

    scope.def(
        "color_from_name_web",
        ( Color ( * )( std::string const & ))( &::genesis::utils::color_from_name_web ),
            pybind11::arg("name"),
        get_docstring("Color ::genesis::utils::color_from_name_web (std::string const & name)")
    );

    scope.def(
        "color_from_name_xkcd",
        ( Color ( * )( std::string const & ))( &::genesis::utils::color_from_name_xkcd ),
            pybind11::arg("name"),
        get_docstring("Color ::genesis::utils::color_from_name_xkcd (std::string const & name)")
    );

    scope.def(
        "is_color_name",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_color_name ),
            pybind11::arg("name"),
        get_docstring("bool ::genesis::utils::is_color_name (std::string const & name)")
    );

    scope.def(
        "is_web_color_name",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_web_color_name ),
            pybind11::arg("name"),
        get_docstring("bool ::genesis::utils::is_web_color_name (std::string const & name)")
    );

    scope.def(
        "is_xkcd_color_name",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_xkcd_color_name ),
            pybind11::arg("name"),
        get_docstring("bool ::genesis::utils::is_xkcd_color_name (std::string const & name)")
    );

    scope.def(
        "color_palette_web",
        ( std::vector< Color > ( * )(  ))( &::genesis::utils::color_palette_web )
    );

    scope.def(
        "color_palette_xkcd",
        ( std::vector< Color > ( * )(  ))( &::genesis::utils::color_palette_xkcd )
    );
}
