/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::IteratorDeletions, scope )
{

    // -------------------------------------------------------------------
    //     Class IteratorDeletions
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::IteratorDeletions, std::shared_ptr<::genesis::utils::IteratorDeletions> > ( scope, "IteratorDeletions" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TwobitVector const & >(),
            pybind11::arg("vector")
        )
        .def(
            pybind11::init< IteratorDeletions const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "hash",
            ( TwobitVector::WordType ( ::genesis::utils::IteratorDeletions::* )(  ) const )( &::genesis::utils::IteratorDeletions::hash ),
            get_docstring("TwobitVector::WordType ::genesis::utils::IteratorDeletions::hash () const")
        )
        .def(
            "position",
            ( size_t ( ::genesis::utils::IteratorDeletions::* )(  ) const )( &::genesis::utils::IteratorDeletions::position ),
            get_docstring("size_t ::genesis::utils::IteratorDeletions::position () const")
        )
        .def(
            "vector",
            ( TwobitVector const & ( ::genesis::utils::IteratorDeletions::* )(  ) const )( &::genesis::utils::IteratorDeletions::vector ),
            get_docstring("TwobitVector const & ::genesis::utils::IteratorDeletions::vector () const")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_math_twobit_vector_iterator_deletions_export, ::genesis::utils, scope )
{

    scope.def(
        "iterate_deletions",
        ( utils::Range< IteratorDeletions > ( * )( TwobitVector const & ))( &::genesis::utils::iterate_deletions ),
            pybind11::arg("vector")
    );
}
