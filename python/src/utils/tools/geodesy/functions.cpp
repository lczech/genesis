/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_geodesy_functions_export, ::genesis::utils, scope )
{

    scope.def(
        "convert_geo_coordinate",
        ( GeoCoordinate ( * )( std::string const & ))( &::genesis::utils::convert_geo_coordinate ),
            pybind11::arg("coordinate"),
        get_docstring("GeoCoordinate ::genesis::utils::convert_geo_coordinate (std::string const & coordinate)")
    );

    scope.def(
        "convert_geo_coordinate",
        ( GeoCoordinate ( * )( std::string const &, std::string const & ))( &::genesis::utils::convert_geo_coordinate ),
            pybind11::arg("latitude"),
            pybind11::arg("longitude"),
        get_docstring("GeoCoordinate ::genesis::utils::convert_geo_coordinate (std::string const & latitude, std::string const & longitude)")
    );

    scope.def(
        "geo_distance",
        ( double ( * )( GeoCoordinate const &, GeoCoordinate const & ))( &::genesis::utils::geo_distance ),
            pybind11::arg("c1"),
            pybind11::arg("c2"),
        get_docstring("double ::genesis::utils::geo_distance (GeoCoordinate const & c1, GeoCoordinate const & c2)")
    );

    scope.def(
        "sanitize_geo_coordinate",
        ( std::string ( * )( std::string const &, bool ))( &::genesis::utils::sanitize_geo_coordinate ),
            pybind11::arg("coordinate"),
            pybind11::arg("two_components")=(bool)(true),
        get_docstring("std::string ::genesis::utils::sanitize_geo_coordinate (std::string const & coordinate, bool two_components=true)")
    );
}
