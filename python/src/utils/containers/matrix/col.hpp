/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename MT, typename T>
void PythonExportClass_::genesis::utils::MatrixCol(std::string name)
{

    // -------------------------------------------------------------------
    //     Class MatrixCol
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using MatrixColType = MatrixCol<typename MT, typename T>;

    pybind11::class_< MatrixColType, std::shared_ptr<MatrixColType> > ( scope, name.c_str() )
        .def(
            pybind11::init< matrix_type &, size_t >(),
            pybind11::arg("mat"),
            pybind11::arg("col")
        )
        .def(
            pybind11::init< MatrixCol const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "assign",
            ( self_type & ( MatrixColType::* )( self_type const & ))( &MatrixColType::assign ),
            pybind11::arg("other"),
            get_docstring("self_type & ::genesis::utils::MatrixCol::assign (self_type const & other)")
        )
        .def(
            "at",
            ( value_type & ( MatrixColType::* )( size_t ) const )( &MatrixColType::at ),
            pybind11::arg("row"),
            get_docstring("value_type & ::genesis::utils::MatrixCol::at (size_t row) const")
        )
        .def(
            "col",
            ( size_t ( MatrixColType::* )(  ) const )( &MatrixColType::col ),
            get_docstring("size_t ::genesis::utils::MatrixCol::col () const")
        )
        .def(
            "matrix",
            ( matrix_type & ( MatrixColType::* )(  ) const )( &MatrixColType::matrix ),
            get_docstring("matrix_type & ::genesis::utils::MatrixCol::matrix () const")
        )
        .def(
            "size",
            ( size_t ( MatrixColType::* )(  ) const )( &MatrixColType::size ),
            get_docstring("size_t ::genesis::utils::MatrixCol::size () const")
        )
        .def(
            "to_vector",
            ( std::vector< non_const_value_type > ( MatrixColType::* )(  ) const )( &MatrixColType::to_vector ),
            get_docstring("std::vector< non_const_value_type > ::genesis::utils::MatrixCol::to_vector () const")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
        .def(
            "__getitem__",
            ( value_type & ( MatrixColType::* )( size_t ) const )( &MatrixColType::operator[] ),
            pybind11::arg("row"),
            get_docstring("value_type & ::genesis::utils::MatrixCol::operator[] (size_t row) const")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::MatrixCol& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            py::keep_alive<0, 1>()
        )
    ;
}
