/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_histogram_distances_export, ::genesis::utils, scope )
{

    scope.def(
        "earth_movers_distance",
        ( double ( * )( const Histogram &, const Histogram &, bool ))( &::genesis::utils::earth_movers_distance ),
            pybind11::arg("h1"),
            pybind11::arg("h2"),
            pybind11::arg("normalize")=(bool)(true)
    );
}
