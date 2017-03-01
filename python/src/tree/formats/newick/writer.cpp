/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::NewickWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class NewickWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::NewickWriter, std::shared_ptr<::genesis::tree::NewickWriter> > ( scope, "NewickWriter" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NewickWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::tree::NewickWriter::* )( const Tree &, const std::string ))( &::genesis::tree::NewickWriter::to_file ),
            pybind11::arg("tree"),
            pybind11::arg("filename")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::tree::NewickWriter::* )( const Tree & ))( &::genesis::tree::NewickWriter::to_string ),
            pybind11::arg("tree")
        )
        .def(
            "to_string",
            ( void ( ::genesis::tree::NewickWriter::* )( const Tree &, std::string & ))( &::genesis::tree::NewickWriter::to_string ),
            pybind11::arg("tree"),
            pybind11::arg("ts")
        )
    ;
}
