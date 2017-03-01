/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_matrix_operators_export, ::genesis::utils, scope )
{

    scope.def(
        "matrix_multiplication",
        ( Matrix< double > ( * )( Matrix< double > const &, Matrix< double > const & ))( &::genesis::utils::matrix_multiplication ),
            pybind11::arg("a"),
            pybind11::arg("b"),
        get_docstring("Matrix< double > ::genesis::utils::matrix_multiplication (Matrix< double > const & a, Matrix< double > const & b)")
    );
}
