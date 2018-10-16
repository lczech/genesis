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
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PhyloxmlWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "to_document",
            ( void ( ::genesis::tree::PhyloxmlWriter::* )( const Tree &, utils::XmlDocument & ) const )( &::genesis::tree::PhyloxmlWriter::to_document ),
            pybind11::arg("tree"),
            pybind11::arg("xml"),
            get_docstring("void ::genesis::tree::PhyloxmlWriter::to_document (const Tree & tree, utils::XmlDocument & xml) const")
        )
        .def(
            "to_file",
            ( void ( ::genesis::tree::PhyloxmlWriter::* )( const Tree &, const std::string ) const )( &::genesis::tree::PhyloxmlWriter::to_file ),
            pybind11::arg("tree"),
            pybind11::arg("filename"),
            get_docstring("void ::genesis::tree::PhyloxmlWriter::to_file (const Tree & tree, const std::string filename) const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::tree::PhyloxmlWriter::* )( const Tree & ) const )( &::genesis::tree::PhyloxmlWriter::to_string ),
            pybind11::arg("tree"),
            get_docstring("std::string ::genesis::tree::PhyloxmlWriter::to_string (const Tree & tree) const")
        )
        .def(
            "to_string",
            ( void ( ::genesis::tree::PhyloxmlWriter::* )( const Tree &, std::string & ) const )( &::genesis::tree::PhyloxmlWriter::to_string ),
            pybind11::arg("tree"),
            pybind11::arg("ts"),
            get_docstring("void ::genesis::tree::PhyloxmlWriter::to_string (const Tree & tree, std::string & ts) const")
        )
    ;
}
