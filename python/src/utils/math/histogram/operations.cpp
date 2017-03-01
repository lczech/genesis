/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_histogram_operations_export, ::genesis::utils, scope )
{

    scope.def(
        "normalize",
        ( void ( * )( Histogram &, double ))( &::genesis::utils::normalize ),
            pybind11::arg("h"),
            pybind11::arg("total")=(double)(1.0)
    );

    scope.def(
        "offset",
        ( void ( * )( Histogram &, double ))( &::genesis::utils::offset ),
            pybind11::arg("h"),
            pybind11::arg("value")
    );

    scope.def(
        "scale",
        ( void ( * )( Histogram &, double ))( &::genesis::utils::scale ),
            pybind11::arg("h"),
            pybind11::arg("factor")
    );
}
