/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (XmlWriter, "utils")
{

    // -------------------------------------------------------------------
    //     Class XmlWriter
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::XmlWriter > ( "XmlWriter" )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::utils::XmlWriter::* )( const XmlDocument &, const std::string & ))( &::genesis::utils::XmlWriter::to_file ),
            ( boost::python::arg("document"), boost::python::arg("filename") ),
            get_docstring("void ::genesis::utils::XmlWriter::to_file (const XmlDocument & document, const std::string & filename)")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::XmlWriter::* )( const XmlDocument & ))( &::genesis::utils::XmlWriter::to_string ),
            ( boost::python::arg("document") ),
            get_docstring("std::string ::genesis::utils::XmlWriter::to_string (const XmlDocument & document)")
        )
        .def(
            "to_string",
            ( void ( ::genesis::utils::XmlWriter::* )( const XmlDocument &, std::string & ))( &::genesis::utils::XmlWriter::to_string ),
            ( boost::python::arg("document"), boost::python::arg("output") ),
            get_docstring("void ::genesis::utils::XmlWriter::to_string (const XmlDocument & document, std::string & output)")
        )
    ;
}
