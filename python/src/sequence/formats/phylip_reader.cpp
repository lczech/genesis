/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::PhylipReader, scope )
{

    // -------------------------------------------------------------------
    //     Class PhylipReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::PhylipReader, std::shared_ptr<::genesis::sequence::PhylipReader> > ( scope, "PhylipReader" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::sequence::PhylipReader::PhylipReader ()")
        )
        .def(
            pybind11::init< PhylipReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "from_file",
            ( SequenceSet ( ::genesis::sequence::PhylipReader::* )( std::string const & ) const )( &::genesis::sequence::PhylipReader::from_file ),
            pybind11::arg("file_name"),
            get_docstring("SequenceSet ::genesis::sequence::PhylipReader::from_file (std::string const & file_name) const")
        )
        .def(
            "from_file",
            ( void ( ::genesis::sequence::PhylipReader::* )( std::string const &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::from_file ),
            pybind11::arg("file_name"),
            pybind11::arg("sequence_set"),
            get_docstring("void ::genesis::sequence::PhylipReader::from_file (std::string const & file_name, SequenceSet & sequence_set) const")
        )
        .def(
            "from_stream",
            ( SequenceSet ( ::genesis::sequence::PhylipReader::* )( std::istream & ) const )( &::genesis::sequence::PhylipReader::from_stream ),
            pybind11::arg("input_stream"),
            get_docstring("SequenceSet ::genesis::sequence::PhylipReader::from_stream (std::istream & input_stream) const")
        )
        .def(
            "from_stream",
            ( void ( ::genesis::sequence::PhylipReader::* )( std::istream &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::from_stream ),
            pybind11::arg("input_stream"),
            pybind11::arg("sequence_set"),
            get_docstring("void ::genesis::sequence::PhylipReader::from_stream (std::istream & input_stream, SequenceSet & sequence_set) const")
        )
        .def(
            "from_string",
            ( SequenceSet ( ::genesis::sequence::PhylipReader::* )( std::string const & ) const )( &::genesis::sequence::PhylipReader::from_string ),
            pybind11::arg("input_string"),
            get_docstring("SequenceSet ::genesis::sequence::PhylipReader::from_string (std::string const & input_string) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::sequence::PhylipReader::* )( std::string const &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::from_string ),
            pybind11::arg("input_string"),
            pybind11::arg("sequence_set"),
            get_docstring("void ::genesis::sequence::PhylipReader::from_string (std::string const & input_string, SequenceSet & sequence_set) const")
        )
        .def(
            "label_length",
            ( PhylipReader & ( ::genesis::sequence::PhylipReader::* )( size_t ))( &::genesis::sequence::PhylipReader::label_length ),
            pybind11::arg("value"),
            get_docstring("PhylipReader & ::genesis::sequence::PhylipReader::label_length (size_t value)")
        )
        .def(
            "label_length",
            ( size_t ( ::genesis::sequence::PhylipReader::* )(  ) const )( &::genesis::sequence::PhylipReader::label_length ),
            get_docstring("size_t ::genesis::sequence::PhylipReader::label_length () const")
        )
        // .def(
        //     "mode",
        //     ( Mode ( ::genesis::sequence::PhylipReader::* )(  ) const )( &::genesis::sequence::PhylipReader::mode )
        // )
        // .def(
        //     "mode",
        //     ( PhylipReader & ( ::genesis::sequence::PhylipReader::* )( Mode ))( &::genesis::sequence::PhylipReader::mode ),
        //     pybind11::arg("value"),
        //     get_docstring("PhylipReader & ::genesis::sequence::PhylipReader::mode (Mode value)")
        // )
        .def(
            "parse_phylip_header",
            ( ::genesis::sequence::PhylipReader::Header ( ::genesis::sequence::PhylipReader::* )( utils::InputStream & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_header ),
            pybind11::arg("it"),
            get_docstring("Header ::genesis::sequence::PhylipReader::parse_phylip_header (utils::InputStream & it) const")
        )
        .def(
            "parse_phylip_interleaved",
            ( void ( ::genesis::sequence::PhylipReader::* )( utils::InputStream &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_interleaved ),
            pybind11::arg("it"),
            pybind11::arg("sset"),
            get_docstring("void ::genesis::sequence::PhylipReader::parse_phylip_interleaved (utils::InputStream & it, SequenceSet & sset) const")
        )
        .def(
            "parse_phylip_label",
            ( std::string ( ::genesis::sequence::PhylipReader::* )( utils::InputStream & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_label ),
            pybind11::arg("it"),
            get_docstring("std::string ::genesis::sequence::PhylipReader::parse_phylip_label (utils::InputStream & it) const")
        )
        .def(
            "parse_phylip_sequence_line",
            ( std::string ( ::genesis::sequence::PhylipReader::* )( utils::InputStream & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_sequence_line ),
            pybind11::arg("it"),
            get_docstring("std::string ::genesis::sequence::PhylipReader::parse_phylip_sequence_line (utils::InputStream & it) const")
        )
        .def(
            "parse_phylip_sequential",
            ( void ( ::genesis::sequence::PhylipReader::* )( utils::InputStream &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_sequential ),
            pybind11::arg("it"),
            pybind11::arg("sset"),
            get_docstring("void ::genesis::sequence::PhylipReader::parse_phylip_sequential (utils::InputStream & it, SequenceSet & sset) const")
        )
        .def(
            "to_upper",
            ( PhylipReader & ( ::genesis::sequence::PhylipReader::* )( bool ))( &::genesis::sequence::PhylipReader::to_upper ),
            pybind11::arg("value"),
            get_docstring("PhylipReader & ::genesis::sequence::PhylipReader::to_upper (bool value)")
        )
        .def(
            "to_upper",
            ( bool ( ::genesis::sequence::PhylipReader::* )(  ) const )( &::genesis::sequence::PhylipReader::to_upper ),
            get_docstring("bool ::genesis::sequence::PhylipReader::to_upper () const")
        )
        .def(
            "valid_char_lookup",
            ( utils::CharLookup< bool > & ( ::genesis::sequence::PhylipReader::* )(  ))( &::genesis::sequence::PhylipReader::valid_char_lookup ),
            get_docstring("utils::CharLookup< bool > & ::genesis::sequence::PhylipReader::valid_char_lookup ()")
        )
        .def(
            "valid_chars",
            ( PhylipReader & ( ::genesis::sequence::PhylipReader::* )( std::string const & ))( &::genesis::sequence::PhylipReader::valid_chars ),
            pybind11::arg("chars"),
            get_docstring("PhylipReader & ::genesis::sequence::PhylipReader::valid_chars (std::string const & chars)")
        )
        .def(
            "valid_chars",
            ( std::string ( ::genesis::sequence::PhylipReader::* )(  ) const )( &::genesis::sequence::PhylipReader::valid_chars ),
            get_docstring("std::string ::genesis::sequence::PhylipReader::valid_chars () const")
        )
    ;
}
