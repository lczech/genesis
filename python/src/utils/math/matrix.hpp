/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename T>
void PythonExportClass_::genesis::utils::Matrix(std::string name)
{

    // -------------------------------------------------------------------
    //     Class Matrix
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using MatrixType = Matrix<typename T>;

    pybind11::class_< MatrixType, std::shared_ptr<MatrixType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< size_t, size_t >(),
            pybind11::arg("rows"),
            pybind11::arg("cols")
        )
        .def(
            pybind11::init< size_t, size_t, T >(),
            pybind11::arg("rows"),
            pybind11::arg("cols"),
            pybind11::arg("init")
        )
        .def(
            pybind11::init< size_t, size_t, std::initializer_list< T > const & >(),
            pybind11::arg("rows"),
            pybind11::arg("cols"),
            pybind11::arg("init_list")
        )
        .def(
            pybind11::init< Matrix const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "at",
            ( T & ( MatrixType::* )( const size_t, const size_t ))( &MatrixType::at ),
            pybind11::arg("row"),
            pybind11::arg("col")
        )
        .def(
            "at",
            ( const T ( MatrixType::* )( const size_t, const size_t ) const )( &MatrixType::at ),
            pybind11::arg("row"),
            pybind11::arg("col")
        )
        .def(
            "cbegin",
            ( const_iterator ( MatrixType::* )(  ) const )( &MatrixType::cbegin )
        )
        .def(
            "cend",
            ( const_iterator ( MatrixType::* )(  ) const )( &MatrixType::cend )
        )
        .def(
            "col",
            ( std::vector< T > ( MatrixType::* )( size_t ) const )( &MatrixType::col ),
            pybind11::arg("index")
        )
        .def(
            "cols",
            ( size_t ( MatrixType::* )(  ) const )( &MatrixType::cols )
        )
        .def(
            "row",
            ( std::vector< T > ( MatrixType::* )( size_t ) const )( &MatrixType::row ),
            pybind11::arg("index")
        )
        .def(
            "rows",
            ( size_t ( MatrixType::* )(  ) const )( &MatrixType::rows )
        )
        .def(
            "size",
            ( size_t ( MatrixType::* )(  ) const )( &MatrixType::size )
        )
        .def(
            "swap",
            ( void ( MatrixType::* )( Matrix & ))( &MatrixType::swap ),
            pybind11::arg("other")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::Matrix& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}
