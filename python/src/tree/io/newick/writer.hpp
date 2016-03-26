/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename TreeType_>
void PythonExportClass_NewickWriter(std::string name)
{

    // -------------------------------------------------------------------
    //     Class NewickWriter
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using NewickWriterType = NewickWriter<typename TreeType_>;

    boost::python::class_< NewickWriterType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< NewickWriter const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( NewickWriterType::* )( const TreeType &, const std::string ))( &NewickWriterType::to_file ),
            ( boost::python::arg("tree"), boost::python::arg("filename") ),
            get_docstring("void ::genesis::tree::NewickWriter::to_file (const TreeType & tree, const std::string filename)")
        )
        .def(
            "to_string",
            ( std::string ( NewickWriterType::* )( const TreeType & ))( &NewickWriterType::to_string ),
            ( boost::python::arg("tree") ),
            get_docstring("std::string ::genesis::tree::NewickWriter::to_string (const TreeType & tree)")
        )
        .def(
            "to_string",
            ( void ( NewickWriterType::* )( const TreeType &, std::string & ))( &NewickWriterType::to_string ),
            ( boost::python::arg("tree"), boost::python::arg("ts") ),
            get_docstring("void ::genesis::tree::NewickWriter::to_string (const TreeType & tree, std::string & ts)")
        )
    ;
}
