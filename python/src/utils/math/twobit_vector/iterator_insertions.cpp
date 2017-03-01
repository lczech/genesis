/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::IteratorInsertions, scope )
{

    // -------------------------------------------------------------------
    //     Class IteratorInsertions
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::IteratorInsertions, std::shared_ptr<::genesis::utils::IteratorInsertions> > ( scope, "IteratorInsertions" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TwobitVector const & >(),
            pybind11::arg("vector")
        )
        .def(
            pybind11::init< IteratorInsertions const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "hash",
            ( TwobitVector::WordType ( ::genesis::utils::IteratorInsertions::* )(  ) const )( &::genesis::utils::IteratorInsertions::hash ),
            get_docstring("TwobitVector::WordType ::genesis::utils::IteratorInsertions::hash () const")
        )
        .def(
            "position",
            ( size_t ( ::genesis::utils::IteratorInsertions::* )(  ) const )( &::genesis::utils::IteratorInsertions::position ),
            get_docstring("size_t ::genesis::utils::IteratorInsertions::position () const")
        )
        .def(
            "vector",
            ( TwobitVector const & ( ::genesis::utils::IteratorInsertions::* )(  ) const )( &::genesis::utils::IteratorInsertions::vector ),
            get_docstring("TwobitVector const & ::genesis::utils::IteratorInsertions::vector () const")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_math_twobit_vector_iterator_insertions_export, ::genesis::utils, scope )
{

    scope.def(
        "iterate_insertions",
        ( utils::Range< IteratorInsertions > ( * )( TwobitVector const & ))( &::genesis::utils::iterate_insertions ),
            pybind11::arg("vector")
    );
}
