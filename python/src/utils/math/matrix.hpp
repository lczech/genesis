/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

template<typename T, typename A, typename B>
void python_export_function_utils_math_matrix_T_A_B ()
{
    scope.def(
        "matrix_addition",
        ( Matrix< T > ( * )( Matrix< A > const &, Matrix< B > const & ))( &::genesis::utils::matrix_addition ),
            pybind11::arg("a"),
            pybind11::arg("b"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_addition (Matrix< A > const & a, Matrix< B > const & b)")
    );

    scope.def(
        "matrix_addition",
        ( Matrix< T > ( * )( Matrix< A > const &, B const & ))( &::genesis::utils::matrix_addition ),
            pybind11::arg("matrix"),
            pybind11::arg("scalar"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_addition (Matrix< A > const & matrix, B const & scalar)")
    );

    scope.def(
        "matrix_multiplication",
        ( Matrix< T > ( * )( Matrix< A > const &, Matrix< B > const & ))( &::genesis::utils::matrix_multiplication ),
            pybind11::arg("a"),
            pybind11::arg("b"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_multiplication (Matrix< A > const & a, Matrix< B > const & b)")
    );

    scope.def(
        "matrix_multiplication",
        ( Matrix< T > ( * )( Matrix< A > const &, B const & ))( &::genesis::utils::matrix_multiplication ),
            pybind11::arg("matrix"),
            pybind11::arg("scalar"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_multiplication (Matrix< A > const & matrix, B const & scalar)")
    );

    scope.def(
        "matrix_subtraction",
        ( Matrix< T > ( * )( Matrix< A > const &, Matrix< B > const & ))( &::genesis::utils::matrix_subtraction ),
            pybind11::arg("a"),
            pybind11::arg("b"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_subtraction (Matrix< A > const & a, Matrix< B > const & b)")
    );

    scope.def(
        "matrix_multiplication",
        ( std::vector< T > ( * )( Matrix< A > const &, std::vector< B > const & ))( &::genesis::utils::matrix_multiplication ),
            pybind11::arg("a"),
            pybind11::arg("b"),
        get_docstring("std::vector< T > ::genesis::utils::matrix_multiplication (Matrix< A > const & a, std::vector< B > const & b)")
    );

    scope.def(
        "matrix_multiplication",
        ( std::vector< T > ( * )( std::vector< A > const &, Matrix< B > const & ))( &::genesis::utils::matrix_multiplication ),
            pybind11::arg("a"),
            pybind11::arg("b"),
        get_docstring("std::vector< T > ::genesis::utils::matrix_multiplication (std::vector< A > const & a, Matrix< B > const & b)")
    );
}

template<typename T>
void python_export_function_utils_math_matrix_T ()
{
    scope.def(
        "matrix_sort_by_col_sum_symmetric",
        ( Matrix< T > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_sort_by_col_sum_symmetric ),
            pybind11::arg("data"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_sort_by_col_sum_symmetric (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_sort_by_row_sum_symmetric",
        ( Matrix< T > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_sort_by_row_sum_symmetric ),
            pybind11::arg("data"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_sort_by_row_sum_symmetric (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_sort_diagonal_symmetric",
        ( Matrix< T > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_sort_diagonal_symmetric ),
            pybind11::arg("data"),
        get_docstring("Matrix< T > ::genesis::utils::matrix_sort_diagonal_symmetric (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_minmax",
        ( MinMaxPair< T > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_minmax ),
            pybind11::arg("data"),
        get_docstring("MinMaxPair< T > ::genesis::utils::matrix_minmax (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_sum",
        ( T ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_sum ),
            pybind11::arg("data"),
        get_docstring("T ::genesis::utils::matrix_sum (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_col_minmax",
        ( std::vector< MinMaxPair< T > > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_col_minmax ),
            pybind11::arg("data"),
        get_docstring("std::vector< MinMaxPair< T > > ::genesis::utils::matrix_col_minmax (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_row_minmax",
        ( std::vector< MinMaxPair< T > > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_row_minmax ),
            pybind11::arg("data"),
        get_docstring("std::vector< MinMaxPair< T > > ::genesis::utils::matrix_row_minmax (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_col_sums",
        ( std::vector< T > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_col_sums ),
            pybind11::arg("data"),
        get_docstring("std::vector< T > ::genesis::utils::matrix_col_sums (Matrix< T > const & data)")
    );

    scope.def(
        "matrix_row_sums",
        ( std::vector< T > ( * )( Matrix< T > const & ))( &::genesis::utils::matrix_row_sums ),
            pybind11::arg("data"),
        get_docstring("std::vector< T > ::genesis::utils::matrix_row_sums (Matrix< T > const & data)")
    );
}