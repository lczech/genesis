/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

template<typename T>
void python_export_function_utils_math_matrix_operators_T ()
{
    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, const Matrix< T > & ))( &::genesis::utils::operator<< ),
        ( boost::python::arg("os"), boost::python::arg("matrix") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>()
    );
}

