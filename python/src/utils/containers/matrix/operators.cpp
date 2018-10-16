/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_containers_matrix_operators_export, ::genesis::utils, scope )
{

    scope.def(
        "triangular_index",
        ( size_t ( * )( size_t, size_t, size_t ))( &::genesis::utils::triangular_index ),
            pybind11::arg("i"),
            pybind11::arg("j"),
            pybind11::arg("n"),
        get_docstring("size_t ::genesis::utils::triangular_index (size_t i, size_t j, size_t n)")
    );

    scope.def(
        "triangular_size",
        ( size_t ( * )( size_t ))( &::genesis::utils::triangular_size ),
            pybind11::arg("n"),
        get_docstring("size_t ::genesis::utils::triangular_size (size_t n)")
    );

    scope.def(
        "triangular_indices",
        ( std::pair< size_t, size_t > ( * )( size_t, size_t ))( &::genesis::utils::triangular_indices ),
            pybind11::arg("k"),
            pybind11::arg("n"),
        get_docstring("std::pair< size_t, size_t > ::genesis::utils::triangular_indices (size_t k, size_t n)")
    );
}
