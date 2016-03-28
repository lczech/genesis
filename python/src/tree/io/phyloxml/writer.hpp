/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename TreeType_>
void PythonExportClass_PhyloxmlWriter(std::string name)
{

    // -------------------------------------------------------------------
    //     Class PhyloxmlWriter
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using PhyloxmlWriterType = PhyloxmlWriter<typename TreeType_>;

    boost::python::class_< PhyloxmlWriterType > ( name.c_str() )

        // Public Member Functions

        .def(
            "to_document",
            ( void ( PhyloxmlWriterType::* )( const TreeType &, utils::XmlDocument & ))( &PhyloxmlWriterType::to_document ),
            ( boost::python::arg("tree"), boost::python::arg("xml") ),
            get_docstring("void ::genesis::tree::PhyloxmlWriter::to_document (const TreeType & tree, utils::XmlDocument & xml)")
        )
        .def(
            "to_file",
            ( void ( PhyloxmlWriterType::* )( const TreeType &, const std::string ))( &PhyloxmlWriterType::to_file ),
            ( boost::python::arg("tree"), boost::python::arg("filename") ),
            get_docstring("void ::genesis::tree::PhyloxmlWriter::to_file (const TreeType & tree, const std::string filename)")
        )
        .def(
            "to_string",
            ( std::string ( PhyloxmlWriterType::* )( const TreeType & ))( &PhyloxmlWriterType::to_string ),
            ( boost::python::arg("tree") ),
            get_docstring("std::string ::genesis::tree::PhyloxmlWriter::to_string (const TreeType & tree)")
        )
        .def(
            "to_string",
            ( void ( PhyloxmlWriterType::* )( const TreeType &, std::string & ))( &PhyloxmlWriterType::to_string ),
            ( boost::python::arg("tree"), boost::python::arg("ts") ),
            get_docstring("void ::genesis::tree::PhyloxmlWriter::to_string (const TreeType & tree, std::string & ts)")
        )
    ;
}
