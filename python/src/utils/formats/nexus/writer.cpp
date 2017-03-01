/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::NexusWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class NexusWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::NexusWriter, std::shared_ptr<::genesis::utils::NexusWriter> > ( scope, "NexusWriter" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NexusWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::string const & ) const )( &::genesis::utils::NexusWriter::to_file ),
            pybind11::arg("doc"),
            pybind11::arg("filename")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::ostream & ) const )( &::genesis::utils::NexusWriter::to_stream ),
            pybind11::arg("doc"),
            pybind11::arg("out")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::NexusWriter::* )( NexusDocument const & ) const )( &::genesis::utils::NexusWriter::to_string ),
            pybind11::arg("doc")
        )
        .def(
            "to_string",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::string & ) const )( &::genesis::utils::NexusWriter::to_string ),
            pybind11::arg("doc"),
            pybind11::arg("output")
        )
    ;
}
