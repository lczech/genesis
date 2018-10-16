/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename MT, typename T>
void PythonExportClass_::genesis::utils::MatrixRow(std::string name)
{

    // -------------------------------------------------------------------
    //     Class MatrixRow
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using MatrixRowType = MatrixRow<typename MT, typename T>;

    pybind11::class_< MatrixRowType, std::shared_ptr<MatrixRowType> > ( scope, name.c_str() )
        .def(
            pybind11::init< matrix_type &, size_t >(),
            pybind11::arg("mat"),
            pybind11::arg("row")
        )
        .def(
            pybind11::init< MatrixRow const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "assign",
            ( self_type & ( MatrixRowType::* )( self_type const & ))( &MatrixRowType::assign ),
            pybind11::arg("other"),
            get_docstring("self_type & ::genesis::utils::MatrixRow::assign (self_type const & other)")
        )
        .def(
            "at",
            ( value_type & ( MatrixRowType::* )( size_t ) const )( &MatrixRowType::at ),
            pybind11::arg("column"),
            get_docstring("value_type & ::genesis::utils::MatrixRow::at (size_t column) const")
        )
        .def(
            "matrix",
            ( matrix_type & ( MatrixRowType::* )(  ) const )( &MatrixRowType::matrix ),
            get_docstring("matrix_type & ::genesis::utils::MatrixRow::matrix () const")
        )
        .def(
            "row",
            ( size_t ( MatrixRowType::* )(  ) const )( &MatrixRowType::row ),
            get_docstring("size_t ::genesis::utils::MatrixRow::row () const")
        )
        .def(
            "size",
            ( size_t ( MatrixRowType::* )(  ) const )( &MatrixRowType::size ),
            get_docstring("size_t ::genesis::utils::MatrixRow::size () const")
        )
        .def(
            "to_vector",
            ( std::vector< non_const_value_type > ( MatrixRowType::* )(  ) const )( &MatrixRowType::to_vector ),
            get_docstring("std::vector< non_const_value_type > ::genesis::utils::MatrixRow::to_vector () const")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
        .def(
            "__getitem__",
            ( value_type & ( MatrixRowType::* )( size_t ) const )( &MatrixRowType::operator[] ),
            pybind11::arg("column"),
            get_docstring("value_type & ::genesis::utils::MatrixRow::operator[] (size_t column) const")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::MatrixRow& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            py::keep_alive<0, 1>()
        )
    ;
}
