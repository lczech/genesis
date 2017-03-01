/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::XmlWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class XmlWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::XmlWriter, std::shared_ptr<::genesis::utils::XmlWriter> > ( scope, "XmlWriter" )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::utils::XmlWriter::* )( const XmlDocument &, const std::string & ))( &::genesis::utils::XmlWriter::to_file ),
            pybind11::arg("document"),
            pybind11::arg("filename")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::XmlWriter::* )( const XmlDocument & ))( &::genesis::utils::XmlWriter::to_string ),
            pybind11::arg("document")
        )
        .def(
            "to_string",
            ( void ( ::genesis::utils::XmlWriter::* )( const XmlDocument &, std::string & ))( &::genesis::utils::XmlWriter::to_string ),
            pybind11::arg("document"),
            pybind11::arg("output")
        )
    ;
}
