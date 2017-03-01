/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

template<typename T>
void python_export_function_utils_math_matrix_operators_T ()
{
    scope.def(
        "transpose",
        ( Matrix< T > ( * )( Matrix< T > const & ))( &::genesis::utils::transpose ),
            pybind11::arg("data"),
        get_docstring("Matrix< T > ::genesis::utils::transpose (Matrix< T > const & data)")
    );

    scope.def(
        "swap_cols",
        ( void ( * )( Matrix< T > &, size_t, size_t ))( &::genesis::utils::swap_cols ),
            pybind11::arg("data"),
            pybind11::arg("col_a"),
            pybind11::arg("col_b"),
        get_docstring("void ::genesis::utils::swap_cols (Matrix< T > & data, size_t col_a, size_t col_b)")
    );

    scope.def(
        "swap_rows",
        ( void ( * )( Matrix< T > &, size_t, size_t ))( &::genesis::utils::swap_rows ),
            pybind11::arg("data"),
            pybind11::arg("row_a"),
            pybind11::arg("row_b"),
        get_docstring("void ::genesis::utils::swap_rows (Matrix< T > & data, size_t row_a, size_t row_b)")
    );
}
