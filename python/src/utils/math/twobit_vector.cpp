/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::TwobitVector, scope )
{

    // -------------------------------------------------------------------
    //     Class TwobitVector
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::TwobitVector, std::shared_ptr<::genesis::utils::TwobitVector> > ( scope, "TwobitVector" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< size_t >(),
            pybind11::arg("size")
        )
        .def(
            pybind11::init< TwobitVector const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::utils::TwobitVector::* )(  ))( &::genesis::utils::TwobitVector::clear )
        )
        .def(
            "data_at",
            ( ::genesis::utils::TwobitVector::WordType & ( ::genesis::utils::TwobitVector::* )( size_t ))( &::genesis::utils::TwobitVector::data_at ),
            pybind11::arg("index")
        )
        .def(
            "data_at",
            ( ::genesis::utils::TwobitVector::WordType const & ( ::genesis::utils::TwobitVector::* )( size_t ) const )( &::genesis::utils::TwobitVector::data_at ),
            pybind11::arg("index")
        )
        .def(
            "data_size",
            ( size_t ( ::genesis::utils::TwobitVector::* )(  ) const )( &::genesis::utils::TwobitVector::data_size )
        )
        .def(
            "get",
            ( ::genesis::utils::TwobitVector::ValueType ( ::genesis::utils::TwobitVector::* )( size_t ) const )( &::genesis::utils::TwobitVector::get ),
            pybind11::arg("index")
        )
        .def(
            "hash",
            ( ::genesis::utils::TwobitVector::WordType ( ::genesis::utils::TwobitVector::* )(  ) const )( &::genesis::utils::TwobitVector::hash )
        )
        .def(
            "insert_at",
            ( void ( ::genesis::utils::TwobitVector::* )( size_t, ::genesis::utils::TwobitVector::ValueType ))( &::genesis::utils::TwobitVector::insert_at ),
            pybind11::arg("index"),
            pybind11::arg("value")
        )
        .def(
            "remove_at",
            ( void ( ::genesis::utils::TwobitVector::* )( size_t ))( &::genesis::utils::TwobitVector::remove_at ),
            pybind11::arg("index")
        )
        .def(
            "set",
            ( void ( ::genesis::utils::TwobitVector::* )( size_t, ::genesis::utils::TwobitVector::ValueType ))( &::genesis::utils::TwobitVector::set ),
            pybind11::arg("index"),
            pybind11::arg("value")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::TwobitVector::* )(  ) const )( &::genesis::utils::TwobitVector::size )
        )
        .def(
            "validate",
            ( bool ( ::genesis::utils::TwobitVector::* )(  ) const )( &::genesis::utils::TwobitVector::validate )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
        .def(
            "__getitem__",
            ( ::genesis::utils::TwobitVector::ValueType ( ::genesis::utils::TwobitVector::* )( size_t ) const )( &::genesis::utils::TwobitVector::operator[] ),
            pybind11::arg("index")
        )
    ;
}
