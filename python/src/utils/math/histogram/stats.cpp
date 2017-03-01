/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_histogram_stats_export, ::genesis::utils, scope )
{

    scope.def(
        "max_value",
        ( double ( * )( const Histogram & ))( &::genesis::utils::max_value ),
            pybind11::arg("h")
    );

    scope.def(
        "mean",
        ( double ( * )( const Histogram & ))( &::genesis::utils::mean ),
            pybind11::arg("h"),
        get_docstring("double ::genesis::utils::mean (const Histogram & h)")
    );

    scope.def(
        "median",
        ( double ( * )( const Histogram & ))( &::genesis::utils::median ),
            pybind11::arg("h")
    );

    scope.def(
        "min_value",
        ( double ( * )( const Histogram & ))( &::genesis::utils::min_value ),
            pybind11::arg("h")
    );

    scope.def(
        "sigma",
        ( double ( * )( const Histogram & ))( &::genesis::utils::sigma ),
            pybind11::arg("h"),
        get_docstring("double ::genesis::utils::sigma (const Histogram & h)")
    );

    scope.def(
        "sum",
        ( double ( * )( const Histogram & ))( &::genesis::utils::sum ),
            pybind11::arg("h")
    );

    scope.def(
        "max_bin",
        ( size_t ( * )( const Histogram & ))( &::genesis::utils::max_bin ),
            pybind11::arg("h")
    );

    scope.def(
        "min_bin",
        ( size_t ( * )( const Histogram & ))( &::genesis::utils::min_bin ),
            pybind11::arg("h")
    );
}
