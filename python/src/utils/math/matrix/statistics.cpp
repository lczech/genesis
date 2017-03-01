/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_matrix_statistics_export, ::genesis::utils, scope )
{

    scope.def(
        "correlation_matrix",
        ( Matrix< double > ( * )( Matrix< double > const & ))( &::genesis::utils::correlation_matrix ),
            pybind11::arg("data"),
        get_docstring("Matrix< double > ::genesis::utils::correlation_matrix (Matrix< double > const & data)")
    );

    scope.def(
        "covariance_matrix",
        ( Matrix< double > ( * )( Matrix< double > const & ))( &::genesis::utils::covariance_matrix ),
            pybind11::arg("data"),
        get_docstring("Matrix< double > ::genesis::utils::covariance_matrix (Matrix< double > const & data)")
    );

    scope.def(
        "sums_of_squares_and_cross_products_matrix",
        ( Matrix< double > ( * )( Matrix< double > const & ))( &::genesis::utils::sums_of_squares_and_cross_products_matrix ),
            pybind11::arg("data"),
        get_docstring("Matrix< double > ::genesis::utils::sums_of_squares_and_cross_products_matrix (Matrix< double > const & data)")
    );

    scope.def(
        "matrix_col_mean_stddev",
        ( std::vector< MeanStddevPair > ( * )( Matrix< double > const & ))( &::genesis::utils::matrix_col_mean_stddev ),
            pybind11::arg("data"),
        get_docstring("std::vector< MeanStddevPair > ::genesis::utils::matrix_col_mean_stddev (Matrix< double > const & data)")
    );

    scope.def(
        "matrix_row_mean_stddev",
        ( std::vector< MeanStddevPair > ( * )( Matrix< double > const & ))( &::genesis::utils::matrix_row_mean_stddev ),
            pybind11::arg("data"),
        get_docstring("std::vector< MeanStddevPair > ::genesis::utils::matrix_row_mean_stddev (Matrix< double > const & data)")
    );

    scope.def(
        "standardize_cols",
        ( std::vector< MeanStddevPair > ( * )( Matrix< double > &, bool, bool ))( &::genesis::utils::standardize_cols ),
            pybind11::arg("data"),
            pybind11::arg("scale_means")=(bool)(true),
            pybind11::arg("scale_std")=(bool)(true),
        get_docstring("std::vector< MeanStddevPair > ::genesis::utils::standardize_cols (Matrix< double > & data, bool scale_means=true, bool scale_std=true)")
    );

    scope.def(
        "standardize_rows",
        ( std::vector< MeanStddevPair > ( * )( Matrix< double > &, bool, bool ))( &::genesis::utils::standardize_rows ),
            pybind11::arg("data"),
            pybind11::arg("scale_means")=(bool)(true),
            pybind11::arg("scale_std")=(bool)(true),
        get_docstring("std::vector< MeanStddevPair > ::genesis::utils::standardize_rows (Matrix< double > & data, bool scale_means=true, bool scale_std=true)")
    );

    scope.def(
        "matrix_col_minmax",
        ( std::vector< MinMaxPair< double > > ( * )( Matrix< double > const & ))( &::genesis::utils::matrix_col_minmax ),
            pybind11::arg("data"),
        get_docstring("std::vector< MinMaxPair< double > > ::genesis::utils::matrix_col_minmax (Matrix< double > const & data)")
    );

    scope.def(
        "matrix_row_minmax",
        ( std::vector< MinMaxPair< double > > ( * )( Matrix< double > const & ))( &::genesis::utils::matrix_row_minmax ),
            pybind11::arg("data"),
        get_docstring("std::vector< MinMaxPair< double > > ::genesis::utils::matrix_row_minmax (Matrix< double > const & data)")
    );

    scope.def(
        "normalize_cols",
        ( std::vector< MinMaxPair< double > > ( * )( Matrix< double > & ))( &::genesis::utils::normalize_cols ),
            pybind11::arg("data"),
        get_docstring("std::vector< MinMaxPair< double > > ::genesis::utils::normalize_cols (Matrix< double > & data)")
    );

    scope.def(
        "normalize_rows",
        ( std::vector< MinMaxPair< double > > ( * )( Matrix< double > & ))( &::genesis::utils::normalize_rows ),
            pybind11::arg("data"),
        get_docstring("std::vector< MinMaxPair< double > > ::genesis::utils::normalize_rows (Matrix< double > & data)")
    );
}
