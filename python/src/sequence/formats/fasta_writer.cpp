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
            "abundance_notation",
            ( ::genesis::sequence::FastaWriter::AbundanceNotation ( ::genesis::sequence::FastaWriter::* )(  ) const )( &::genesis::sequence::FastaWriter::abundance_notation ),
            get_docstring("AbundanceNotation ::genesis::sequence::FastaWriter::abundance_notation () const")
        )
        .def(
            "abundance_notation",
            ( FastaWriter & ( ::genesis::sequence::FastaWriter::* )( ::genesis::sequence::FastaWriter::AbundanceNotation ))( &::genesis::sequence::FastaWriter::abundance_notation ),
            pybind11::arg("value"),
            get_docstring("FastaWriter & ::genesis::sequence::FastaWriter::abundance_notation (AbundanceNotation value)")
        )
        .def(
            "line_length",
            ( FastaWriter & ( ::genesis::sequence::FastaWriter::* )( size_t ))( &::genesis::sequence::FastaWriter::line_length ),
            pybind11::arg("value"),
            get_docstring("FastaWriter & ::genesis::sequence::FastaWriter::line_length (size_t value)")
        )
        .def(
            "line_length",
            ( size_t ( ::genesis::sequence::FastaWriter::* )(  ) const )( &::genesis::sequence::FastaWriter::line_length ),
            get_docstring("size_t ::genesis::sequence::FastaWriter::line_length () const")
        )
        .def(
            "to_file",
            ( void ( ::genesis::sequence::FastaWriter::* )( SequenceSet const &, std::string const & ) const )( &::genesis::sequence::FastaWriter::to_file ),
            pybind11::arg("sset"),
            pybind11::arg("fn"),
            get_docstring("void ::genesis::sequence::FastaWriter::to_file (SequenceSet const & sset, std::string const & fn) const")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::sequence::FastaWriter::* )( SequenceSet const &, std::ostream & ) const )( &::genesis::sequence::FastaWriter::to_stream ),
            pybind11::arg("sset"),
            pybind11::arg("os"),
            get_docstring("void ::genesis::sequence::FastaWriter::to_stream (SequenceSet const & sset, std::ostream & os) const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::sequence::FastaWriter::* )( SequenceSet const & ) const )( &::genesis::sequence::FastaWriter::to_string ),
            pybind11::arg("sset"),
            get_docstring("std::string ::genesis::sequence::FastaWriter::to_string (SequenceSet const & sset) const")
        )
        .def(
            "write_sequence",
            ( void ( ::genesis::sequence::FastaWriter::* )( Sequence const &, std::ostream & ) const )( &::genesis::sequence::FastaWriter::write_sequence ),
            pybind11::arg("seq"),
            pybind11::arg("os"),
            get_docstring("void ::genesis::sequence::FastaWriter::write_sequence (Sequence const & seq, std::ostream & os) const")
        )
    ;
}
