/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_matrix_export, ::genesis::utils, scope )
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
        "matrix_mean_stddev",
        ( MeanStddevPair ( * )( Matrix< double > const &, double ))( &::genesis::utils::matrix_mean_stddev ),
            pybind11::arg("data"),
            pybind11::arg("epsilon")=(double)(-1.0),
        get_docstring("MeanStddevPair ::genesis::utils::matrix_mean_stddev (Matrix< double > const & data, double epsilon=-1.0)")
    );

    scope.def(
        "matrix_col_quartiles",
        ( Quartiles ( * )( Matrix< double > const &, size_t ))( &::genesis::utils::matrix_col_quartiles ),
            pybind11::arg("data"),
            pybind11::arg("col")
    );

    scope.def(
        "matrix_quartiles",
        ( Quartiles ( * )( Matrix< double > const & ))( &::genesis::utils::matrix_quartiles ),
            pybind11::arg("data"),
        get_docstring("Quartiles ::genesis::utils::matrix_quartiles (Matrix< double > const & data)")
    );

    scope.def(
        "matrix_row_quartiles",
        ( Quartiles ( * )( Matrix< double > const &, size_t ))( &::genesis::utils::matrix_row_quartiles ),
            pybind11::arg("data"),
            pybind11::arg("row")
    );

    scope.def(
        "matrix_col_pearson_correlation_coefficient",
        ( double ( * )( Matrix< double > const &, size_t, Matrix< double > const &, size_t ))( &::genesis::utils::matrix_col_pearson_correlation_coefficient ),
            pybind11::arg("mat1"),
            pybind11::arg("col1"),
            pybind11::arg("mat2"),
            pybind11::arg("col2"),
        get_docstring("double ::genesis::utils::matrix_col_pearson_correlation_coefficient (Matrix< double > const & mat1, size_t col1, Matrix< double > const & mat2, size_t col2)")
    );

    scope.def(
        "matrix_col_spearmans_rank_correlation_coefficient",
        ( double ( * )( Matrix< double > const &, size_t, Matrix< double > const &, size_t ))( &::genesis::utils::matrix_col_spearmans_rank_correlation_coefficient ),
            pybind11::arg("mat1"),
            pybind11::arg("col1"),
            pybind11::arg("mat2"),
            pybind11::arg("col2"),
        get_docstring("double ::genesis::utils::matrix_col_spearmans_rank_correlation_coefficient (Matrix< double > const & mat1, size_t col1, Matrix< double > const & mat2, size_t col2)")
    );

    scope.def(
        "matrix_row_pearson_correlation_coefficient",
        ( double ( * )( Matrix< double > const &, size_t, Matrix< double > const &, size_t ))( &::genesis::utils::matrix_row_pearson_correlation_coefficient ),
            pybind11::arg("mat1"),
            pybind11::arg("row1"),
            pybind11::arg("mat2"),
            pybind11::arg("row2"),
        get_docstring("double ::genesis::utils::matrix_row_pearson_correlation_coefficient (Matrix< double > const & mat1, size_t row1, Matrix< double > const & mat2, size_t row2)")
    );

    scope.def(
        "matrix_row_spearmans_rank_correlation_coefficient",
        ( double ( * )( Matrix< double > const &, size_t, Matrix< double > const &, size_t ))( &::genesis::utils::matrix_row_spearmans_rank_correlation_coefficient ),
            pybind11::arg("mat1"),
            pybind11::arg("row1"),
            pybind11::arg("mat2"),
            pybind11::arg("row2"),
        get_docstring("double ::genesis::utils::matrix_row_spearmans_rank_correlation_coefficient (Matrix< double > const & mat1, size_t row1, Matrix< double > const & mat2, size_t row2)")
    );

    scope.def(
        "matrix_col_mean_stddev",
        ( std::vector< MeanStddevPair > ( * )( Matrix< double > const &, double ))( &::genesis::utils::matrix_col_mean_stddev ),
            pybind11::arg("data"),
            pybind11::arg("epsilon")=(double)(-1.0),
        get_docstring("std::vector< MeanStddevPair > ::genesis::utils::matrix_col_mean_stddev (Matrix< double > const & data, double epsilon=-1.0)")
    );

    scope.def(
        "matrix_row_mean_stddev",
        ( std::vector< MeanStddevPair > ( * )( Matrix< double > const &, double ))( &::genesis::utils::matrix_row_mean_stddev ),
            pybind11::arg("data"),
            pybind11::arg("epsilon")=(double)(-1.0),
        get_docstring("std::vector< MeanStddevPair > ::genesis::utils::matrix_row_mean_stddev (Matrix< double > const & data, double epsilon=-1.0)")
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

    scope.def(
        "matrix_col_quartiles",
        ( std::vector< Quartiles > ( * )( Matrix< double > const & ))( &::genesis::utils::matrix_col_quartiles ),
            pybind11::arg("data")
    );

    scope.def(
        "matrix_row_quartiles",
        ( std::vector< Quartiles > ( * )( Matrix< double > const & ))( &::genesis::utils::matrix_row_quartiles ),
            pybind11::arg("data")
    );
}
