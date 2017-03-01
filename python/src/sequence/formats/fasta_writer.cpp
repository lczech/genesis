/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::FastaWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class FastaWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::FastaWriter, std::shared_ptr<::genesis::sequence::FastaWriter> > ( scope, "FastaWriter" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< FastaWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "enable_metadata",
            ( FastaWriter & ( ::genesis::sequence::FastaWriter::* )( bool ))( &::genesis::sequence::FastaWriter::enable_metadata ),
            pybind11::arg("value")
        )
        .def(
            "enable_metadata",
            ( bool ( ::genesis::sequence::FastaWriter::* )(  ))( &::genesis::sequence::FastaWriter::enable_metadata )
        )
        .def(
            "line_length",
            ( FastaWriter & ( ::genesis::sequence::FastaWriter::* )( size_t ))( &::genesis::sequence::FastaWriter::line_length ),
            pybind11::arg("value")
        )
        .def(
            "line_length",
            ( size_t ( ::genesis::sequence::FastaWriter::* )(  ) const )( &::genesis::sequence::FastaWriter::line_length )
        )
        .def(
            "to_file",
            ( void ( ::genesis::sequence::FastaWriter::* )( SequenceSet const &, std::string const & ) const )( &::genesis::sequence::FastaWriter::to_file ),
            pybind11::arg("sset"),
            pybind11::arg("fn")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::sequence::FastaWriter::* )( SequenceSet const &, std::ostream & ) const )( &::genesis::sequence::FastaWriter::to_stream ),
            pybind11::arg("sset"),
            pybind11::arg("os")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::sequence::FastaWriter::* )( SequenceSet const & ) const )( &::genesis::sequence::FastaWriter::to_string ),
            pybind11::arg("sset")
        )
        .def(
            "write_sequence",
            ( void ( ::genesis::sequence::FastaWriter::* )( Sequence const &, std::ostream & ) const )( &::genesis::sequence::FastaWriter::write_sequence ),
            pybind11::arg("seq"),
            pybind11::arg("os")
        )
    ;
}
