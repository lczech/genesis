/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::PhyloxmlWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class PhyloxmlWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::PhyloxmlWriter, std::shared_ptr<::genesis::tree::PhyloxmlWriter> > ( scope, "PhyloxmlWriter" )

        // Public Member Functions

        .def(
            "to_document",
            ( void ( ::genesis::tree::PhyloxmlWriter::* )( const Tree &, utils::XmlDocument & ))( &::genesis::tree::PhyloxmlWriter::to_document ),
            pybind11::arg("tree"),
            pybind11::arg("xml")
        )
        .def(
            "to_file",
            ( void ( ::genesis::tree::PhyloxmlWriter::* )( const Tree &, const std::string ))( &::genesis::tree::PhyloxmlWriter::to_file ),
            pybind11::arg("tree"),
            pybind11::arg("filename")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::tree::PhyloxmlWriter::* )( const Tree & ) const )( &::genesis::tree::PhyloxmlWriter::to_string ),
            pybind11::arg("tree")
        )
        .def(
            "to_string",
            ( void ( ::genesis::tree::PhyloxmlWriter::* )( const Tree &, std::string & ) const )( &::genesis::tree::PhyloxmlWriter::to_string ),
            pybind11::arg("tree"),
            pybind11::arg("ts")
        )
    ;
}
