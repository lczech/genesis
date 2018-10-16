/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::FastaReader, scope )
{

    // -------------------------------------------------------------------
    //     Class FastaReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::FastaReader, std::shared_ptr<::genesis::sequence::FastaReader> > ( scope, "FastaReader" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::sequence::FastaReader::FastaReader ()")
        )
        .def(
            pybind11::init< FastaReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "from_file",
            ( SequenceSet ( ::genesis::sequence::FastaReader::* )( std::string const & ) const )( &::genesis::sequence::FastaReader::from_file ),
            pybind11::arg("file_name"),
            get_docstring("SequenceSet ::genesis::sequence::FastaReader::from_file (std::string const & file_name) const")
        )
        .def(
            "from_file",
            ( void ( ::genesis::sequence::FastaReader::* )( std::string const &, SequenceSet & ) const )( &::genesis::sequence::FastaReader::from_file ),
            pybind11::arg("file_name"),
            pybind11::arg("sequence_set"),
            get_docstring("void ::genesis::sequence::FastaReader::from_file (std::string const & file_name, SequenceSet & sequence_set) const")
        )
        .def(
            "from_stream",
            ( SequenceSet ( ::genesis::sequence::FastaReader::* )( std::istream & ) const )( &::genesis::sequence::FastaReader::from_stream ),
            pybind11::arg("input_stream"),
            get_docstring("SequenceSet ::genesis::sequence::FastaReader::from_stream (std::istream & input_stream) const")
        )
        .def(
            "from_stream",
            ( void ( ::genesis::sequence::FastaReader::* )( std::istream &, SequenceSet & ) const )( &::genesis::sequence::FastaReader::from_stream ),
            pybind11::arg("input_stream"),
            pybind11::arg("sequence_set"),
            get_docstring("void ::genesis::sequence::FastaReader::from_stream (std::istream & input_stream, SequenceSet & sequence_set) const")
        )
        .def(
            "from_string",
            ( SequenceSet ( ::genesis::sequence::FastaReader::* )( std::string const & ) const )( &::genesis::sequence::FastaReader::from_string ),
            pybind11::arg("input_string"),
            get_docstring("SequenceSet ::genesis::sequence::FastaReader::from_string (std::string const & input_string) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::sequence::FastaReader::* )( std::string const &, SequenceSet & ) const )( &::genesis::sequence::FastaReader::from_string ),
            pybind11::arg("input_string"),
            pybind11::arg("sequence_set"),
            get_docstring("void ::genesis::sequence::FastaReader::from_string (std::string const & input_string, SequenceSet & sequence_set) const")
        )
        .def(
            "guess_abundances",
            ( FastaReader & ( ::genesis::sequence::FastaReader::* )( bool ))( &::genesis::sequence::FastaReader::guess_abundances ),
            pybind11::arg("value"),
            get_docstring("FastaReader & ::genesis::sequence::FastaReader::guess_abundances (bool value)")
        )
        .def(
            "guess_abundances",
            ( bool ( ::genesis::sequence::FastaReader::* )(  ) const )( &::genesis::sequence::FastaReader::guess_abundances ),
            get_docstring("bool ::genesis::sequence::FastaReader::guess_abundances () const")
        )
        .def(
            "parse_document",
            ( void ( ::genesis::sequence::FastaReader::* )( utils::InputStream &, SequenceSet & ) const )( &::genesis::sequence::FastaReader::parse_document ),
            pybind11::arg("input_stream"),
            pybind11::arg("sequence_set"),
            get_docstring("void ::genesis::sequence::FastaReader::parse_document (utils::InputStream & input_stream, SequenceSet & sequence_set) const")
        )
        .def(
            "parse_sequence",
            ( bool ( ::genesis::sequence::FastaReader::* )( utils::InputStream &, Sequence & ) const )( &::genesis::sequence::FastaReader::parse_sequence ),
            pybind11::arg("input_stream"),
            pybind11::arg("sequence"),
            get_docstring("bool ::genesis::sequence::FastaReader::parse_sequence (utils::InputStream & input_stream, Sequence & sequence) const")
        )
        .def(
            "parse_sequence_pedantic",
            ( bool ( ::genesis::sequence::FastaReader::* )( utils::InputStream &, Sequence & ) const )( &::genesis::sequence::FastaReader::parse_sequence_pedantic ),
            pybind11::arg("input_stream"),
            pybind11::arg("sequence"),
            get_docstring("bool ::genesis::sequence::FastaReader::parse_sequence_pedantic (utils::InputStream & input_stream, Sequence & sequence) const")
        )
        .def(
            "parsing_method",
            ( FastaReader & ( ::genesis::sequence::FastaReader::* )( ::genesis::sequence::FastaReader::ParsingMethod ))( &::genesis::sequence::FastaReader::parsing_method ),
            pybind11::arg("value"),
            get_docstring("FastaReader & ::genesis::sequence::FastaReader::parsing_method (ParsingMethod value)")
        )
        .def(
            "parsing_method",
            ( ::genesis::sequence::FastaReader::ParsingMethod ( ::genesis::sequence::FastaReader::* )(  ) const )( &::genesis::sequence::FastaReader::parsing_method ),
            get_docstring("ParsingMethod ::genesis::sequence::FastaReader::parsing_method () const")
        )
        // .def(
        //     "read",
        //     ( SequenceSet ( ::genesis::sequence::FastaReader::* )( utils::InputSource ) const )( &::genesis::sequence::FastaReader::read ),
        //     pybind11::arg("input")
        // )
        .def(
            "site_casing",
            ( FastaReader & ( ::genesis::sequence::FastaReader::* )( ::genesis::sequence::FastaReader::SiteCasing ))( &::genesis::sequence::FastaReader::site_casing ),
            pybind11::arg("value"),
            get_docstring("FastaReader & ::genesis::sequence::FastaReader::site_casing (SiteCasing value)")
        )
        .def(
            "site_casing",
            ( ::genesis::sequence::FastaReader::SiteCasing ( ::genesis::sequence::FastaReader::* )(  ) const )( &::genesis::sequence::FastaReader::site_casing ),
            get_docstring("SiteCasing ::genesis::sequence::FastaReader::site_casing () const")
        )
        .def(
            "valid_char_lookup",
            ( utils::CharLookup< bool > & ( ::genesis::sequence::FastaReader::* )(  ))( &::genesis::sequence::FastaReader::valid_char_lookup ),
            get_docstring("utils::CharLookup< bool > & ::genesis::sequence::FastaReader::valid_char_lookup ()")
        )
        .def(
            "valid_chars",
            ( FastaReader & ( ::genesis::sequence::FastaReader::* )( std::string const & ))( &::genesis::sequence::FastaReader::valid_chars ),
            pybind11::arg("chars"),
            get_docstring("FastaReader & ::genesis::sequence::FastaReader::valid_chars (std::string const & chars)")
        )
        .def(
            "valid_chars",
            ( std::string ( ::genesis::sequence::FastaReader::* )(  ) const )( &::genesis::sequence::FastaReader::valid_chars ),
            get_docstring("std::string ::genesis::sequence::FastaReader::valid_chars () const")
        )
    ;
}
