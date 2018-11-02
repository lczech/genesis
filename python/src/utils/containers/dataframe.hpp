/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename T>
void PythonExportClass_::genesis::utils::Dataframe(std::string name)
{

    // -------------------------------------------------------------------
    //     Class Dataframe
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using DataframeType = Dataframe<typename T>;

    pybind11::class_< DataframeType, std::shared_ptr<DataframeType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Dataframe const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_col",
            ( self_type & ( DataframeType::* )(  ))( &DataframeType::add_col )
        )
        .def(
            "add_col",
            ( self_type & ( DataframeType::* )( std::string const & ))( &DataframeType::add_col ),
            pybind11::arg("name")
        )
        .def(
            "add_row",
            ( self_type & ( DataframeType::* )(  ))( &DataframeType::add_row )
        )
        .def(
            "add_row",
            ( self_type & ( DataframeType::* )( std::string const & ))( &DataframeType::add_row ),
            pybind11::arg("name")
        )
        .def(
            "at",
            ( const_reference ( DataframeType::* )( size_type ) const )( &DataframeType::at ),
            pybind11::arg("col_index")
        )
        .def(
            "at",
            ( const_reference ( DataframeType::* )( std::string const & ) const )( &DataframeType::at ),
            pybind11::arg("col_name")
        )
        .def(
            "at",
            ( reference ( DataframeType::* )( size_type ))( &DataframeType::at ),
            pybind11::arg("col_index")
        )
        .def(
            "at",
            ( reference ( DataframeType::* )( std::string const & ))( &DataframeType::at ),
            pybind11::arg("col_name")
        )
        .def(
            "cbegin",
            ( const_iterator ( DataframeType::* )(  ))( &DataframeType::cbegin )
        )
        .def(
            "cend",
            ( const_iterator ( DataframeType::* )(  ))( &DataframeType::cend )
        )
        .def(
            "clear",
            ( self_type & ( DataframeType::* )(  ))( &DataframeType::clear )
        )
        .def(
            "clear_cols",
            ( self_type & ( DataframeType::* )(  ))( &DataframeType::clear_cols )
        )
        .def(
            "clear_rows",
            ( self_type & ( DataframeType::* )(  ))( &DataframeType::clear_rows )
        )
        .def(
            "col_index",
            ( size_t ( DataframeType::* )( std::string const & ) const )( &DataframeType::col_index ),
            pybind11::arg("col_name")
        )
        .def(
            "col_name",
            ( self_type & ( DataframeType::* )( size_type, std::string const & ))( &DataframeType::col_name ),
            pybind11::arg("col_index"),
            pybind11::arg("value")
        )
        .def(
            "col_name",
            ( std::string const & ( DataframeType::* )( size_type ) const )( &DataframeType::col_name ),
            pybind11::arg("col_index")
        )
        .def(
            "col_names",
            ( std::vector< std::string > ( DataframeType::* )(  ) const )( &DataframeType::col_names )
        )
        .def(
            "cols",
            ( size_type ( DataframeType::* )(  ) const )( &DataframeType::cols )
        )
        .def(
            "empty",
            ( bool ( DataframeType::* )(  ) const )( &DataframeType::empty )
        )
        .def(
            "remove_col",
            ( self_type & ( DataframeType::* )( size_type ))( &DataframeType::remove_col ),
            pybind11::arg("col_index")
        )
        .def(
            "remove_col",
            ( self_type & ( DataframeType::* )( std::string const & ))( &DataframeType::remove_col ),
            pybind11::arg("col_name")
        )
        .def(
            "remove_row",
            ( self_type & ( DataframeType::* )( size_type ))( &DataframeType::remove_row ),
            pybind11::arg("row_index")
        )
        .def(
            "remove_row",
            ( self_type & ( DataframeType::* )( std::string const & ))( &DataframeType::remove_row ),
            pybind11::arg("row_name")
        )
        .def(
            "row_index",
            ( size_t ( DataframeType::* )( std::string const & ) const )( &DataframeType::row_index ),
            pybind11::arg("row_name")
        )
        .def(
            "row_name",
            ( self_type & ( DataframeType::* )( size_type, std::string const & ))( &DataframeType::row_name ),
            pybind11::arg("row_index"),
            pybind11::arg("value")
        )
        .def(
            "row_name",
            ( std::string const & ( DataframeType::* )( size_type ) const )( &DataframeType::row_name ),
            pybind11::arg("row_index")
        )
        .def(
            "row_names",
            ( std::vector< std::string > const & ( DataframeType::* )(  ) const )( &DataframeType::row_names )
        )
        .def(
            "rows",
            ( size_type ( DataframeType::* )(  ) const )( &DataframeType::rows )
        )

        // Operators

        .def(
            "__getitem__",
            ( reference ( DataframeType::* )( size_type ))( &DataframeType::operator[] ),
            pybind11::arg("col_index")
        )
        .def(
            "__getitem__",
            ( const_reference ( DataframeType::* )( size_type ) const )( &DataframeType::operator[] ),
            pybind11::arg("col_index")
        )
        .def(
            "__getitem__",
            ( reference ( DataframeType::* )( std::string const & ))( &DataframeType::operator[] ),
            pybind11::arg("col_name")
        )
        .def(
            "__getitem__",
            ( const_reference ( DataframeType::* )( std::string const & ) const )( &DataframeType::operator[] ),
            pybind11::arg("col_name")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::Dataframe& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            py::keep_alive<0, 1>()
        )
    ;
}
