/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS (FastaReader, "sequence")
{

    // -------------------------------------------------------------------
    //     Class FastaReader
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::sequence::FastaReader > ( "FastaReader", boost::python::init<  >(  ) )
        .def( boost::python::init< FastaReader const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "from_file",
            ( void ( ::genesis::sequence::FastaReader::* )( std::string const &, SequenceSet & ) const )( &::genesis::sequence::FastaReader::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("sset") ),
            get_docstring("void ::genesis::sequence::FastaReader::from_file (std::string const & fn, SequenceSet & sset) const")
        )
        .def(
            "from_stream",
            ( void ( ::genesis::sequence::FastaReader::* )( std::istream &, SequenceSet & ) const )( &::genesis::sequence::FastaReader::from_stream ),
            ( boost::python::arg("is"), boost::python::arg("sset") ),
            get_docstring("void ::genesis::sequence::FastaReader::from_stream (std::istream & is, SequenceSet & sset) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::sequence::FastaReader::* )( std::string const &, SequenceSet & ) const )( &::genesis::sequence::FastaReader::from_string ),
            ( boost::python::arg("fs"), boost::python::arg("sset") ),
            get_docstring("void ::genesis::sequence::FastaReader::from_string (std::string const & fs, SequenceSet & sset) const")
        )
        .def(
            "is_validating",
            ( bool ( ::genesis::sequence::FastaReader::* )(  ) const )( &::genesis::sequence::FastaReader::is_validating ),
            get_docstring("bool ::genesis::sequence::FastaReader::is_validating () const")
        )
        .def(
            "parse_fasta_sequence",
            ( bool ( ::genesis::sequence::FastaReader::* )( utils::CountingIstream &, Sequence & ) const )( &::genesis::sequence::FastaReader::parse_fasta_sequence ),
            ( boost::python::arg("input_stream"), boost::python::arg("sequence") ),
            get_docstring("bool ::genesis::sequence::FastaReader::parse_fasta_sequence (utils::CountingIstream & input_stream, Sequence & sequence) const")
        )
        .def(
            "parse_fasta_sequence_fast",
            ( bool ( ::genesis::sequence::FastaReader::* )( utils::CountingIstream &, Sequence & ) const )( &::genesis::sequence::FastaReader::parse_fasta_sequence_fast ),
            ( boost::python::arg("input_stream"), boost::python::arg("sequence") ),
            get_docstring("bool ::genesis::sequence::FastaReader::parse_fasta_sequence_fast (utils::CountingIstream & input_stream, Sequence & sequence) const")
        )
        .def(
            "to_upper",
            ( FastaReader & ( ::genesis::sequence::FastaReader::* )( bool ))( &::genesis::sequence::FastaReader::to_upper ),
            ( boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("FastaReader & ::genesis::sequence::FastaReader::to_upper (bool value)")
        )
        .def(
            "to_upper",
            ( bool ( ::genesis::sequence::FastaReader::* )(  ) const )( &::genesis::sequence::FastaReader::to_upper ),
            get_docstring("bool ::genesis::sequence::FastaReader::to_upper () const")
        )
        .def(
            "valid_char_lookup",
            ( utils::CharLookup & ( ::genesis::sequence::FastaReader::* )(  ))( &::genesis::sequence::FastaReader::valid_char_lookup ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("utils::CharLookup & ::genesis::sequence::FastaReader::valid_char_lookup ()")
        )
        .def(
            "validate_chars",
            ( FastaReader & ( ::genesis::sequence::FastaReader::* )( std::string const & ))( &::genesis::sequence::FastaReader::validate_chars ),
            ( boost::python::arg("chars") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("FastaReader & ::genesis::sequence::FastaReader::validate_chars (std::string const & chars)")
        )
        .def(
            "validate_chars",
            ( std::string ( ::genesis::sequence::FastaReader::* )(  ) const )( &::genesis::sequence::FastaReader::validate_chars ),
            get_docstring("std::string ::genesis::sequence::FastaReader::validate_chars () const")
        )
    ;
}
