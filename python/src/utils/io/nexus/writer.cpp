/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (NexusWriter, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusWriter
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusWriter > ( "NexusWriter", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusWriter const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::string const & ) const )( &::genesis::utils::NexusWriter::to_file ),
            ( boost::python::arg("doc"), boost::python::arg("filename") )
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::ostream & ) const )( &::genesis::utils::NexusWriter::to_stream ),
            ( boost::python::arg("doc"), boost::python::arg("out") )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::NexusWriter::* )( NexusDocument const & ) const )( &::genesis::utils::NexusWriter::to_string ),
            ( boost::python::arg("doc") )
        )
        .def(
            "to_string",
            ( void ( ::genesis::utils::NexusWriter::* )( NexusDocument const &, std::string & ) const )( &::genesis::utils::NexusWriter::to_string ),
            ( boost::python::arg("doc"), boost::python::arg("output") )
        )
    ;
}
