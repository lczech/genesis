/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

template<class T>
void python_export_function_utils_containers_sorted_vector_operators_T ()
{
    scope.def(
        "equals",
        ( bool ( * )( SortedVector< T > const &, SortedVector< T > const & ))( &::genesis::utils::equals ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "is_subset_of",
        ( bool ( * )( SortedVector< T > const &, SortedVector< T > const & ))( &::genesis::utils::is_subset_of ),
            pybind11::arg("subset"),
            pybind11::arg("superset")
    );

    scope.def(
        "is_subset_or_equal",
        ( bool ( * )( SortedVector< T > const &, SortedVector< T > const & ))( &::genesis::utils::is_subset_or_equal ),
            pybind11::arg("subset"),
            pybind11::arg("superset")
    );

    scope.def(
        "operator!=",
        ( bool ( * )( SortedVector< T > const &, SortedVector< T > const & ))( &::genesis::utils::operator!= ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator==",
        ( bool ( * )( SortedVector< T > const &, SortedVector< T > const & ))( &::genesis::utils::operator== ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}