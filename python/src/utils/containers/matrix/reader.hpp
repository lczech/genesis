/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename T>
void PythonExportClass_::genesis::utils::MatrixReader(std::string name)
{

    // -------------------------------------------------------------------
    //     Class MatrixReader
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using MatrixReaderType = MatrixReader<typename T>;

    pybind11::class_< MatrixReaderType, std::shared_ptr<MatrixReaderType> > ( scope, name.c_str() )
        .def(
            pybind11::init< char >(),
            pybind11::arg("separator_char")=(char)('\t')
        )
        .def(
            pybind11::init< CsvReader const & >(),
            pybind11::arg("reader")
        )
        .def(
            pybind11::init< MatrixReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "csv_reader",
            ( CsvReader & ( MatrixReaderType::* )(  ))( &MatrixReaderType::csv_reader )
        )
        .def(
            "csv_reader",
            ( CsvReader const & ( MatrixReaderType::* )(  ) const )( &MatrixReaderType::csv_reader )
        )
        .def(
            "from_file",
            ( Matrix< T > ( MatrixReaderType::* )( std::string const & ) const )( &MatrixReaderType::from_file ),
            pybind11::arg("fn")
        )
        .def(
            "from_stream",
            ( Matrix< T > ( MatrixReaderType::* )( std::istream & ) const )( &MatrixReaderType::from_stream ),
            pybind11::arg("is")
        )
        .def(
            "from_string",
            ( Matrix< T > ( MatrixReaderType::* )( std::string const & ) const )( &MatrixReaderType::from_string ),
            pybind11::arg("fs")
        )
        .def(
            "parse_value_functor",
            ( MatrixReader & ( MatrixReaderType::* )( std::function< T(std::string const &)> ))( &MatrixReaderType::parse_value_functor ),
            pybind11::arg("functor")
        )
        .def(
            "skip_first_col",
            ( MatrixReader & ( MatrixReaderType::* )( bool ))( &MatrixReaderType::skip_first_col ),
            pybind11::arg("value")
        )
        .def(
            "skip_first_col",
            ( bool ( MatrixReaderType::* )(  ) const )( &MatrixReaderType::skip_first_col )
        )
        .def(
            "skip_first_row",
            ( MatrixReader & ( MatrixReaderType::* )( bool ))( &MatrixReaderType::skip_first_row ),
            pybind11::arg("value")
        )
        .def(
            "skip_first_row",
            ( bool ( MatrixReaderType::* )(  ) const )( &MatrixReaderType::skip_first_row )
        )
    ;
}
