/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename T>
void PythonExportClass_::genesis::utils::DataframeReader(std::string name)
{

    // -------------------------------------------------------------------
    //     Class DataframeReader
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using DataframeReaderType = DataframeReader<typename T>;

    pybind11::class_< DataframeReaderType, std::shared_ptr<DataframeReaderType> > ( scope, name.c_str() )
        .def(
            pybind11::init< char >(),
            pybind11::arg("separator_char")=(char)(',')
        )
        .def(
            pybind11::init< CsvReader const & >(),
            pybind11::arg("reader")
        )
        .def(
            pybind11::init< DataframeReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "csv_reader",
            ( CsvReader & ( DataframeReaderType::* )(  ))( &DataframeReaderType::csv_reader )
        )
        .def(
            "csv_reader",
            ( CsvReader const & ( DataframeReaderType::* )(  ) const )( &DataframeReaderType::csv_reader )
        )
        .def(
            "from_file",
            ( Dataframe< T > ( DataframeReaderType::* )( std::string const & ) const )( &DataframeReaderType::from_file ),
            pybind11::arg("fn")
        )
        .def(
            "from_stream",
            ( Dataframe< T > ( DataframeReaderType::* )( std::istream & ) const )( &DataframeReaderType::from_stream ),
            pybind11::arg("is")
        )
        .def(
            "from_string",
            ( Dataframe< T > ( DataframeReaderType::* )( std::string const & ) const )( &DataframeReaderType::from_string ),
            pybind11::arg("fs")
        )
        .def(
            "names_from_first_col",
            ( DataframeReader & ( DataframeReaderType::* )( bool ))( &DataframeReaderType::names_from_first_col ),
            pybind11::arg("value")
        )
        .def(
            "names_from_first_col",
            ( bool ( DataframeReaderType::* )(  ) const )( &DataframeReaderType::names_from_first_col )
        )
        .def(
            "names_from_first_row",
            ( DataframeReader & ( DataframeReaderType::* )( bool ))( &DataframeReaderType::names_from_first_row ),
            pybind11::arg("value")
        )
        .def(
            "names_from_first_row",
            ( bool ( DataframeReaderType::* )(  ) const )( &DataframeReaderType::names_from_first_row )
        )
        .def(
            "parse_value_functor",
            ( DataframeReader & ( DataframeReaderType::* )( std::function< T(std::string const &)> ))( &DataframeReaderType::parse_value_functor ),
            pybind11::arg("functor")
        )
    ;
}
