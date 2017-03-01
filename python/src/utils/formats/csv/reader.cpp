/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::CsvReader, scope )
{

    // -------------------------------------------------------------------
    //     Class CsvReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::CsvReader, std::shared_ptr<::genesis::utils::CsvReader> > ( scope, "CsvReader" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< CsvReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "comment_chars",
            ( CsvReader & ( ::genesis::utils::CsvReader::* )( std::string const & ))( &::genesis::utils::CsvReader::comment_chars ),
            pybind11::arg("chars")
        )
        .def(
            "comment_chars",
            ( std::string const & ( ::genesis::utils::CsvReader::* )(  ) const )( &::genesis::utils::CsvReader::comment_chars )
        )
        .def(
            "from_file",
            ( ::genesis::utils::CsvReader::table ( ::genesis::utils::CsvReader::* )( std::string const & ) const )( &::genesis::utils::CsvReader::from_file ),
            pybind11::arg("fn")
        )
        .def(
            "from_stream",
            ( ::genesis::utils::CsvReader::table ( ::genesis::utils::CsvReader::* )( std::istream & ) const )( &::genesis::utils::CsvReader::from_stream ),
            pybind11::arg("is")
        )
        .def(
            "from_string",
            ( ::genesis::utils::CsvReader::table ( ::genesis::utils::CsvReader::* )( std::string const & ) const )( &::genesis::utils::CsvReader::from_string ),
            pybind11::arg("fs")
        )
        .def(
            "merge_separators",
            ( CsvReader & ( ::genesis::utils::CsvReader::* )( bool ))( &::genesis::utils::CsvReader::merge_separators ),
            pybind11::arg("value")
        )
        .def(
            "merge_separators",
            ( bool ( ::genesis::utils::CsvReader::* )(  ) const )( &::genesis::utils::CsvReader::merge_separators )
        )
        .def(
            "parse_document",
            ( ::genesis::utils::CsvReader::table ( ::genesis::utils::CsvReader::* )( utils::InputStream & ) const )( &::genesis::utils::CsvReader::parse_document ),
            pybind11::arg("input_stream")
        )
        .def(
            "parse_field",
            ( std::string ( ::genesis::utils::CsvReader::* )( utils::InputStream & ) const )( &::genesis::utils::CsvReader::parse_field ),
            pybind11::arg("input_stream")
        )
        .def(
            "parse_line",
            ( std::vector< std::string > ( ::genesis::utils::CsvReader::* )( utils::InputStream & ) const )( &::genesis::utils::CsvReader::parse_line ),
            pybind11::arg("input_stream")
        )
        .def(
            "quotation_chars",
            ( CsvReader & ( ::genesis::utils::CsvReader::* )( std::string const & ))( &::genesis::utils::CsvReader::quotation_chars ),
            pybind11::arg("chars")
        )
        .def(
            "quotation_chars",
            ( std::string const & ( ::genesis::utils::CsvReader::* )(  ) const )( &::genesis::utils::CsvReader::quotation_chars )
        )
        .def(
            "separator_chars",
            ( CsvReader & ( ::genesis::utils::CsvReader::* )( std::string const & ))( &::genesis::utils::CsvReader::separator_chars ),
            pybind11::arg("chars")
        )
        .def(
            "separator_chars",
            ( std::string const & ( ::genesis::utils::CsvReader::* )(  ) const )( &::genesis::utils::CsvReader::separator_chars )
        )
        .def(
            "skip_empty_lines",
            ( CsvReader & ( ::genesis::utils::CsvReader::* )( bool ))( &::genesis::utils::CsvReader::skip_empty_lines ),
            pybind11::arg("value")
        )
        .def(
            "skip_empty_lines",
            ( bool ( ::genesis::utils::CsvReader::* )(  ) const )( &::genesis::utils::CsvReader::skip_empty_lines )
        )
        .def(
            "trim_chars",
            ( CsvReader & ( ::genesis::utils::CsvReader::* )( std::string const & ))( &::genesis::utils::CsvReader::trim_chars ),
            pybind11::arg("chars")
        )
        .def(
            "trim_chars",
            ( std::string const & ( ::genesis::utils::CsvReader::* )(  ) const )( &::genesis::utils::CsvReader::trim_chars )
        )
        .def(
            "use_escapes",
            ( CsvReader & ( ::genesis::utils::CsvReader::* )( bool ))( &::genesis::utils::CsvReader::use_escapes ),
            pybind11::arg("value")
        )
        .def(
            "use_escapes",
            ( bool ( ::genesis::utils::CsvReader::* )(  ) const )( &::genesis::utils::CsvReader::use_escapes )
        )
        .def(
            "use_twin_quotes",
            ( CsvReader & ( ::genesis::utils::CsvReader::* )( bool ))( &::genesis::utils::CsvReader::use_twin_quotes ),
            pybind11::arg("value")
        )
        .def(
            "use_twin_quotes",
            ( bool ( ::genesis::utils::CsvReader::* )(  ) const )( &::genesis::utils::CsvReader::use_twin_quotes )
        )
    ;
}
