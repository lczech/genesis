/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

template<typename T, typename Pred>
void python_export_function_utils_core_algorithm_T_Pred ()
{
    scope.def(
        "insert_sorted",
        ( std::vector< T >::iterator ( * )( std::vector< T > &, T const &, Pred ))( &::genesis::utils::insert_sorted ),
            pybind11::arg("vec"),
            pybind11::arg("item"),
            pybind11::arg("pred"),
        get_docstring("std::vector< T >::iterator ::genesis::utils::insert_sorted (std::vector< T > & vec, T const & item, Pred pred)")
    );
}

template<typename RandomAccessIterator, typename Comparator>
void python_export_function_utils_core_algorithm_RandomAccessIterator_Comparator ()
{
    scope.def(
        "sort_indices",
        ( std::vector< size_t > ( * )( RandomAccessIterator, RandomAccessIterator, Comparator ))( &::genesis::utils::sort_indices ),
            pybind11::arg("first"),
            pybind11::arg("last"),
            pybind11::arg("comparator"),
        get_docstring("std::vector< size_t > ::genesis::utils::sort_indices (RandomAccessIterator first, RandomAccessIterator last, Comparator comparator)")
    );

    scope.def(
        "stable_sort_indices",
        ( std::vector< size_t > ( * )( RandomAccessIterator, RandomAccessIterator, Comparator ))( &::genesis::utils::stable_sort_indices ),
            pybind11::arg("first"),
            pybind11::arg("last"),
            pybind11::arg("comparator"),
        get_docstring("std::vector< size_t > ::genesis::utils::stable_sort_indices (RandomAccessIterator first, RandomAccessIterator last, Comparator comparator)")
    );
}

template<class C, class T>
void python_export_function_utils_core_algorithm_C_T ()
{
    scope.def(
        "contains",
        ( bool ( * )( const C &, const T & ))( &::genesis::utils::contains ),
            pybind11::arg("v"),
            pybind11::arg("x"),
        get_docstring("bool ::genesis::utils::contains (const C & v, const T & x)")
    );
}

template<typename RandomAccessIterator>
void python_export_function_utils_core_algorithm_RandomAccessIterator ()
{
    scope.def(
        "sort_indices",
        ( std::vector< size_t > ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::sort_indices ),
            pybind11::arg("first"),
            pybind11::arg("last"),
        get_docstring("std::vector< size_t > ::genesis::utils::sort_indices (RandomAccessIterator first, RandomAccessIterator last)")
    );

    scope.def(
        "stable_sort_indices",
        ( std::vector< size_t > ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::stable_sort_indices ),
            pybind11::arg("first"),
            pybind11::arg("last"),
        get_docstring("std::vector< size_t > ::genesis::utils::stable_sort_indices (RandomAccessIterator first, RandomAccessIterator last)")
    );
}

template<class Container, class UnaryPredicate>
void python_export_function_utils_core_algorithm_Container_UnaryPredicate ()
{
    scope.def(
        "erase_if",
        ( void ( * )( Container &, UnaryPredicate ))( &::genesis::utils::erase_if ),
            pybind11::arg("c"),
            pybind11::arg("p"),
        get_docstring("void ::genesis::utils::erase_if (Container & c, UnaryPredicate p)")
    );
}

template<typename T>
void python_export_function_utils_core_algorithm_T ()
{
    scope.def(
        "insert_sorted",
        ( std::vector< T >::iterator ( * )( std::vector< T > &, T const & ))( &::genesis::utils::insert_sorted ),
            pybind11::arg("vec"),
            pybind11::arg("item"),
        get_docstring("std::vector< T >::iterator ::genesis::utils::insert_sorted (std::vector< T > & vec, T const & item)")
    );
}