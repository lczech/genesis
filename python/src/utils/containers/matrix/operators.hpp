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
void python_export_function_utils_containers_matrix_operators_T ()
{
    scope.def(
        "matrix_transpose",
        ( Matrix< T > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_transpose ),
            pybind11::arg("data"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_transpose (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_is_symmetric",
        ( bool ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_is_symmetric ),
            pybind11::arg("data"),
        get_docstring("bool ::genesis::utils::matrix_is_symmetric (Matrix< T > const & data)")
    );

    scope.def(
        "print",
        ( std::string ( * )( Matrix< T > const &, size_t, size_t ))( &::genesis::utils::print ),
            pybind11::arg("matrix"),
            pybind11::arg("rows")=(size_t)(10),
            pybind11::arg("cols")=(size_t)(10),
        get_docstring("std::string ::genesis::utils::print (Matrix< T > const & matrix, size_t rows=10, size_t cols=10)")
    );

    scope.def(
        "matrix_swap_cols",
        ( void ( * )( Matrix< T > &, size_t, size_t ))( &::genesis::utils::matrix_swap_cols ),
            pybind11::arg("data"),
            pybind11::arg("col_a"),
            pybind11::arg("col_b"),
        get_docstring("void ::genesis::utils::matrix_swap_cols (Matrix< T > & data, size_t col_a, size_t col_b)")
    );

    scope.def(
        "matrix_swap_rows",
        ( void ( * )( Matrix< T > &, size_t, size_t ))( &::genesis::utils::matrix_swap_rows ),
            pybind11::arg("data"),
            pybind11::arg("row_a"),
            pybind11::arg("row_b"),
        get_docstring("void ::genesis::utils::matrix_swap_rows (Matrix< T > & data, size_t row_a, size_t row_b)")
    );

    scope.def(
        "print",
        ( void ( * )( std::ostream &, Matrix< T > const &, size_t, size_t ))( &::genesis::utils::print ),
            pybind11::arg("out"),
            pybind11::arg("matrix"),
            pybind11::arg("rows")=(size_t)(10),
            pybind11::arg("cols")=(size_t)(10),
        get_docstring("void ::genesis::utils::print (std::ostream & out, Matrix< T > const & matrix, size_t rows=10, size_t cols=10)")
    );
}