/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (JplaceWriter, "placement")
{

    // -------------------------------------------------------------------
    //     Class JplaceWriter
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::JplaceWriter > ( "JplaceWriter" )

        // Public Member Functions

        .def(
            "to_document",
            ( void ( ::genesis::placement::JplaceWriter::* )( const Sample &, utils::JsonDocument & ) const )( &::genesis::placement::JplaceWriter::to_document ),
            ( boost::python::arg("smp"), boost::python::arg("doc") ),
            get_docstring("void ::genesis::placement::JplaceWriter::to_document (const Sample & smp, utils::JsonDocument & doc) const")
        )
        .def(
            "to_file",
            ( void ( ::genesis::placement::JplaceWriter::* )( const Sample &, const std::string ) const )( &::genesis::placement::JplaceWriter::to_file ),
            ( boost::python::arg("smp"), boost::python::arg("filename") ),
            get_docstring("void ::genesis::placement::JplaceWriter::to_file (const Sample & smp, const std::string filename) const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::placement::JplaceWriter::* )( const Sample & ) const )( &::genesis::placement::JplaceWriter::to_string ),
            ( boost::python::arg("smp") ),
            get_docstring("std::string ::genesis::placement::JplaceWriter::to_string (const Sample & smp) const")
        )
        .def(
            "to_string",
            ( void ( ::genesis::placement::JplaceWriter::* )( const Sample &, std::string & ) const )( &::genesis::placement::JplaceWriter::to_string ),
            ( boost::python::arg("smp"), boost::python::arg("output") ),
            get_docstring("void ::genesis::placement::JplaceWriter::to_string (const Sample & smp, std::string & output) const")
        )
    ;
}
