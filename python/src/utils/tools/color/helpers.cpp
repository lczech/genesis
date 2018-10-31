/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_helpers_export, ::genesis::utils, scope )
{

    scope.def(
        "color_stops",
        ( std::map< double, Color > ( * )( ColorMap const &, ColorNormalization const & ))( &::genesis::utils::color_stops ),
            pybind11::arg("map"),
            pybind11::arg("norm")
    );

    scope.def(
        "color_stops",
        ( std::map< double, Color > ( * )( ColorMap const &, ColorNormalizationBoundary const & ))( &::genesis::utils::color_stops ),
            pybind11::arg("map"),
            pybind11::arg("norm")
    );

    scope.def(
        "color_stops",
        ( std::map< double, Color > ( * )( ColorMap const &, ColorNormalizationDiverging const & ))( &::genesis::utils::color_stops ),
            pybind11::arg("map"),
            pybind11::arg("norm")
    );

    scope.def(
        "color_stops",
        ( std::map< double, Color > ( * )( ColorMap const &, ColorNormalizationLinear const & ))( &::genesis::utils::color_stops ),
            pybind11::arg("map"),
            pybind11::arg("norm")
    );

    scope.def(
        "color_stops",
        ( std::map< double, Color > ( * )( ColorMap const &, ColorNormalizationLogarithmic const & ))( &::genesis::utils::color_stops ),
            pybind11::arg("map"),
            pybind11::arg("norm")
    );

    scope.def(
        "color_tickmarks",
        ( std::map< double, std::string > ( * )( ColorNormalization const &, size_t ))( &::genesis::utils::color_tickmarks ),
            pybind11::arg("norm"),
            pybind11::arg("num_ticks")
    );

    scope.def(
        "color_tickmarks",
        ( std::map< double, std::string > ( * )( ColorNormalizationBoundary const &, size_t ))( &::genesis::utils::color_tickmarks ),
            pybind11::arg("norm"),
            pybind11::arg("num_ticks")
    );

    scope.def(
        "color_tickmarks",
        ( std::map< double, std::string > ( * )( ColorNormalizationDiverging const &, size_t ))( &::genesis::utils::color_tickmarks ),
            pybind11::arg("norm"),
            pybind11::arg("num_ticks")
    );

    scope.def(
        "color_tickmarks",
        ( std::map< double, std::string > ( * )( ColorNormalizationLinear const &, size_t ))( &::genesis::utils::color_tickmarks ),
            pybind11::arg("norm"),
            pybind11::arg("num_ticks")
    );

    scope.def(
        "color_tickmarks",
        ( std::map< double, std::string > ( * )( ColorNormalizationLogarithmic const &, size_t ))( &::genesis::utils::color_tickmarks ),
            pybind11::arg("norm"),
            pybind11::arg("num_ticks")
    );
}
