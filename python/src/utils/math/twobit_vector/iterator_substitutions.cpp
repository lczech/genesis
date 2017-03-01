/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::IteratorSubstitutions, scope )
{

    // -------------------------------------------------------------------
    //     Class IteratorSubstitutions
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::IteratorSubstitutions, std::shared_ptr<::genesis::utils::IteratorSubstitutions> > ( scope, "IteratorSubstitutions" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TwobitVector const & >(),
            pybind11::arg("vector")
        )
        .def(
            pybind11::init< IteratorSubstitutions const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "hash",
            ( TwobitVector::WordType ( ::genesis::utils::IteratorSubstitutions::* )(  ) const )( &::genesis::utils::IteratorSubstitutions::hash ),
            get_docstring("TwobitVector::WordType ::genesis::utils::IteratorSubstitutions::hash () const")
        )
        .def(
            "position",
            ( size_t ( ::genesis::utils::IteratorSubstitutions::* )(  ) const )( &::genesis::utils::IteratorSubstitutions::position ),
            get_docstring("size_t ::genesis::utils::IteratorSubstitutions::position () const")
        )
        .def(
            "vector",
            ( TwobitVector const & ( ::genesis::utils::IteratorSubstitutions::* )(  ) const )( &::genesis::utils::IteratorSubstitutions::vector ),
            get_docstring("TwobitVector const & ::genesis::utils::IteratorSubstitutions::vector () const")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_math_twobit_vector_iterator_substitutions_export, ::genesis::utils, scope )
{

    scope.def(
        "iterate_substitutions",
        ( utils::Range< IteratorSubstitutions > ( * )( TwobitVector const & ))( &::genesis::utils::iterate_substitutions ),
            pybind11::arg("vector")
    );
}
