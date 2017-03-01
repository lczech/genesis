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
PYTHON_EXPORT_CLASS( ::genesis::utils::JsonWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class JsonWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::JsonWriter, std::shared_ptr<::genesis::utils::JsonWriter> > ( scope, "JsonWriter" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< JsonWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "indent",
            ( JsonWriter & ( ::genesis::utils::JsonWriter::* )( size_t ))( &::genesis::utils::JsonWriter::indent ),
            pybind11::arg("value"),
            get_docstring("JsonWriter & ::genesis::utils::JsonWriter::indent (size_t value)")
        )
        .def(
            "indent",
            ( size_t ( ::genesis::utils::JsonWriter::* )(  ) const )( &::genesis::utils::JsonWriter::indent ),
            get_docstring("size_t ::genesis::utils::JsonWriter::indent () const")
        )
        .def(
            "precision",
            ( JsonWriter & ( ::genesis::utils::JsonWriter::* )( size_t ))( &::genesis::utils::JsonWriter::precision ),
            pybind11::arg("value"),
            get_docstring("JsonWriter & ::genesis::utils::JsonWriter::precision (size_t value)")
        )
        .def(
            "precision",
            ( size_t ( ::genesis::utils::JsonWriter::* )(  ) const )( &::genesis::utils::JsonWriter::precision ),
            get_docstring("size_t ::genesis::utils::JsonWriter::precision () const")
        )
        .def(
            "to_file",
            ( void ( ::genesis::utils::JsonWriter::* )( JsonDocument const &, std::string const & ) const )( &::genesis::utils::JsonWriter::to_file ),
            pybind11::arg("document"),
            pybind11::arg("filename"),
            get_docstring("void ::genesis::utils::JsonWriter::to_file (JsonDocument const & document, std::string const & filename) const")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::JsonWriter::* )( JsonDocument const &, std::ostream & ) const )( &::genesis::utils::JsonWriter::to_stream ),
            pybind11::arg("document"),
            pybind11::arg("out"),
            get_docstring("void ::genesis::utils::JsonWriter::to_stream (JsonDocument const & document, std::ostream & out) const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonWriter::* )( JsonDocument const & ) const )( &::genesis::utils::JsonWriter::to_string ),
            pybind11::arg("document"),
            get_docstring("std::string ::genesis::utils::JsonWriter::to_string (JsonDocument const & document) const")
        )
        .def(
            "to_string",
            ( void ( ::genesis::utils::JsonWriter::* )( JsonDocument const &, std::string & ) const )( &::genesis::utils::JsonWriter::to_string ),
            pybind11::arg("document"),
            pybind11::arg("output"),
            get_docstring("void ::genesis::utils::JsonWriter::to_string (JsonDocument const & document, std::string & output) const")
        )
    ;
}
*/
