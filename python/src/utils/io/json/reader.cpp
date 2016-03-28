/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (JsonReader, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonReader
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonReader > ( "JsonReader", boost::python::init<  >(  ) )
        .def( boost::python::init< JsonReader const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "from_file",
            ( void ( ::genesis::utils::JsonReader::* )( const std::string &, JsonDocument & ) const )( &::genesis::utils::JsonReader::from_file ),
            ( boost::python::arg("filename"), boost::python::arg("document") ),
            get_docstring("void ::genesis::utils::JsonReader::from_file (const std::string & filename, JsonDocument & document) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::utils::JsonReader::* )( const std::string &, JsonDocument & ) const )( &::genesis::utils::JsonReader::from_string ),
            ( boost::python::arg("json"), boost::python::arg("document") ),
            get_docstring("void ::genesis::utils::JsonReader::from_string (const std::string & json, JsonDocument & document) const")
        )
    ;
}
