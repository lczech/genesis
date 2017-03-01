/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;
/*
PYTHON_EXPORT_CLASS( ::genesis::utils::JsonReader, scope )
{

    // -------------------------------------------------------------------
    //     Class JsonReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::JsonReader, std::shared_ptr<::genesis::utils::JsonReader> > ( scope, "JsonReader" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< JsonReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "from_file",
            ( JsonDocument ( ::genesis::utils::JsonReader::* )( const std::string & ) const )( &::genesis::utils::JsonReader::from_file ),
            pybind11::arg("filename"),
            get_docstring("JsonDocument ::genesis::utils::JsonReader::from_file (const std::string & filename) const")
        )
        .def(
            "from_stream",
            ( JsonDocument ( ::genesis::utils::JsonReader::* )( std::istream & ) const )( &::genesis::utils::JsonReader::from_stream ),
            pybind11::arg("input_stream"),
            get_docstring("JsonDocument ::genesis::utils::JsonReader::from_stream (std::istream & input_stream) const")
        )
        .def(
            "from_string",
            ( JsonDocument ( ::genesis::utils::JsonReader::* )( const std::string & ) const )( &::genesis::utils::JsonReader::from_string ),
            pybind11::arg("json"),
            get_docstring("JsonDocument ::genesis::utils::JsonReader::from_string (const std::string & json) const")
        )
        .def(
            "parse",
            ( JsonDocument ( ::genesis::utils::JsonReader::* )( InputStream & ) const )( &::genesis::utils::JsonReader::parse ),
            pybind11::arg("input_stream")
        )
        .def(
            "parse_array",
            ( JsonDocument ( ::genesis::utils::JsonReader::* )( InputStream & ) const )( &::genesis::utils::JsonReader::parse_array ),
            pybind11::arg("input_stream")
        )
        .def(
            "parse_number",
            ( JsonDocument ( ::genesis::utils::JsonReader::* )( InputStream & ) const )( &::genesis::utils::JsonReader::parse_number ),
            pybind11::arg("input_stream")
        )
        .def(
            "parse_object",
            ( JsonDocument ( ::genesis::utils::JsonReader::* )( InputStream & ) const )( &::genesis::utils::JsonReader::parse_object ),
            pybind11::arg("input_stream")
        )
        .def(
            "parse_value",
            ( JsonDocument ( ::genesis::utils::JsonReader::* )( InputStream & ) const )( &::genesis::utils::JsonReader::parse_value ),
            pybind11::arg("input_stream")
        )
    ;
}
*/
