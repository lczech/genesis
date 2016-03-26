/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (JsonWriter, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonWriter
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonWriter > ( "JsonWriter", boost::python::init<  >(  ) )
        .def( boost::python::init< JsonWriter const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::utils::JsonWriter::* )( JsonDocument const &, std::string const & ) const )( &::genesis::utils::JsonWriter::to_file ),
            ( boost::python::arg("document"), boost::python::arg("filename") ),
            get_docstring("void ::genesis::utils::JsonWriter::to_file (JsonDocument const & document, std::string const & filename) const")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::JsonWriter::* )( JsonDocument const &, std::ostream & ) const )( &::genesis::utils::JsonWriter::to_stream ),
            ( boost::python::arg("document"), boost::python::arg("out") ),
            get_docstring("void ::genesis::utils::JsonWriter::to_stream (JsonDocument const & document, std::ostream & out) const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonWriter::* )( JsonDocument const & ) const )( &::genesis::utils::JsonWriter::to_string ),
            ( boost::python::arg("document") ),
            get_docstring("std::string ::genesis::utils::JsonWriter::to_string (JsonDocument const & document) const")
        )
        .def(
            "to_string",
            ( void ( ::genesis::utils::JsonWriter::* )( JsonDocument const &, std::string & ) const )( &::genesis::utils::JsonWriter::to_string ),
            ( boost::python::arg("document"), boost::python::arg("output") ),
            get_docstring("void ::genesis::utils::JsonWriter::to_string (JsonDocument const & document, std::string & output) const")
        )
    ;
}
