/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_random_export, ::genesis::utils, scope )
{

    scope.def(
        "select_without_replacement",
        ( std::vector< size_t > ( * )( size_t, size_t ))( &::genesis::utils::select_without_replacement ),
            pybind11::arg("k"),
            pybind11::arg("n"),
        get_docstring("std::vector< size_t > ::genesis::utils::select_without_replacement (size_t k, size_t n)")
    );
}
