/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::JplaceWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class JplaceWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::JplaceWriter, std::shared_ptr<::genesis::placement::JplaceWriter> > ( scope, "JplaceWriter" )

        // Public Member Functions

        .def(
            "branch_length_precision",
            ( JplaceWriter & ( ::genesis::placement::JplaceWriter::* )( size_t ))( &::genesis::placement::JplaceWriter::branch_length_precision ),
            pybind11::arg("value"),
            get_docstring("JplaceWriter & ::genesis::placement::JplaceWriter::branch_length_precision (size_t value)")
        )
        .def(
            "branch_length_precision",
            ( size_t ( ::genesis::placement::JplaceWriter::* )(  ) const )( &::genesis::placement::JplaceWriter::branch_length_precision ),
            get_docstring("size_t ::genesis::placement::JplaceWriter::branch_length_precision () const")
        )
        .def(
            "to_document",
            ( void ( ::genesis::placement::JplaceWriter::* )( Sample const &, utils::JsonDocument & ) const )( &::genesis::placement::JplaceWriter::to_document ),
            pybind11::arg("smp"),
            pybind11::arg("doc")
        )
        .def(
            "to_file",
            ( void ( ::genesis::placement::JplaceWriter::* )( Sample const &, std::string const & ) const )( &::genesis::placement::JplaceWriter::to_file ),
            pybind11::arg("smp"),
            pybind11::arg("filename")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::placement::JplaceWriter::* )( Sample const &, std::ostream & ) const )( &::genesis::placement::JplaceWriter::to_stream ),
            pybind11::arg("smp"),
            pybind11::arg("os")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::placement::JplaceWriter::* )( Sample const & ) const )( &::genesis::placement::JplaceWriter::to_string ),
            pybind11::arg("smp")
        )
        .def(
            "to_string",
            ( void ( ::genesis::placement::JplaceWriter::* )( Sample const &, std::string & ) const )( &::genesis::placement::JplaceWriter::to_string ),
            pybind11::arg("smp"),
            pybind11::arg("output")
        )
    ;
}
