/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS (PhylipReader, "sequence")
{

    // -------------------------------------------------------------------
    //     Class PhylipReader
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::sequence::PhylipReader > ( "PhylipReader", boost::python::init<  >(  ) )
        .def( boost::python::init< PhylipReader const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "from_file",
            ( void ( ::genesis::sequence::PhylipReader::* )( std::string const &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("sset") ),
            get_docstring("void ::genesis::sequence::PhylipReader::from_file (std::string const & fn, SequenceSet & sset) const")
        )
        .def(
            "from_stream",
            ( void ( ::genesis::sequence::PhylipReader::* )( std::istream &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::from_stream ),
            ( boost::python::arg("is"), boost::python::arg("sset") ),
            get_docstring("void ::genesis::sequence::PhylipReader::from_stream (std::istream & is, SequenceSet & sset) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::sequence::PhylipReader::* )( std::string const &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::from_string ),
            ( boost::python::arg("fs"), boost::python::arg("sset") ),
            get_docstring("void ::genesis::sequence::PhylipReader::from_string (std::string const & fs, SequenceSet & sset) const")
        )
        .def(
            "is_validating",
            ( bool ( ::genesis::sequence::PhylipReader::* )(  ) const )( &::genesis::sequence::PhylipReader::is_validating ),
            get_docstring("bool ::genesis::sequence::PhylipReader::is_validating () const")
        )
        .def(
            "label_length",
            ( PhylipReader & ( ::genesis::sequence::PhylipReader::* )( size_t ))( &::genesis::sequence::PhylipReader::label_length ),
            ( boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
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
        //     ( boost::python::arg("value") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     get_docstring("PhylipReader & ::genesis::sequence::PhylipReader::mode (Mode value)")
        // )
        .def(
            "parse_phylip_header",
            ( std::pair< size_t, size_t > ( ::genesis::sequence::PhylipReader::* )( utils::CountingIstream & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_header ),
            ( boost::python::arg("it") ),
            get_docstring("std::pair< size_t, size_t > ::genesis::sequence::PhylipReader::parse_phylip_header (utils::CountingIstream & it) const")
        )
        .def(
            "parse_phylip_interleaved",
            ( void ( ::genesis::sequence::PhylipReader::* )( utils::CountingIstream &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_interleaved ),
            ( boost::python::arg("it"), boost::python::arg("sset") ),
            get_docstring("void ::genesis::sequence::PhylipReader::parse_phylip_interleaved (utils::CountingIstream & it, SequenceSet & sset) const")
        )
        .def(
            "parse_phylip_label",
            ( std::string ( ::genesis::sequence::PhylipReader::* )( utils::CountingIstream & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_label ),
            ( boost::python::arg("it") ),
            get_docstring("std::string ::genesis::sequence::PhylipReader::parse_phylip_label (utils::CountingIstream & it) const")
        )
        .def(
            "parse_phylip_sequence_line",
            ( std::string ( ::genesis::sequence::PhylipReader::* )( utils::CountingIstream & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_sequence_line ),
            ( boost::python::arg("it") ),
            get_docstring("std::string ::genesis::sequence::PhylipReader::parse_phylip_sequence_line (utils::CountingIstream & it) const")
        )
        .def(
            "parse_phylip_sequential",
            ( void ( ::genesis::sequence::PhylipReader::* )( utils::CountingIstream &, SequenceSet & ) const )( &::genesis::sequence::PhylipReader::parse_phylip_sequential ),
            ( boost::python::arg("it"), boost::python::arg("sset") ),
            get_docstring("void ::genesis::sequence::PhylipReader::parse_phylip_sequential (utils::CountingIstream & it, SequenceSet & sset) const")
        )
        .def(
            "to_upper",
            ( PhylipReader & ( ::genesis::sequence::PhylipReader::* )( bool ))( &::genesis::sequence::PhylipReader::to_upper ),
            ( boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PhylipReader & ::genesis::sequence::PhylipReader::to_upper (bool value)")
        )
        .def(
            "to_upper",
            ( bool ( ::genesis::sequence::PhylipReader::* )(  ) const )( &::genesis::sequence::PhylipReader::to_upper ),
            get_docstring("bool ::genesis::sequence::PhylipReader::to_upper () const")
        )
        .def(
            "valid_char_lookup",
            ( utils::CharLookup & ( ::genesis::sequence::PhylipReader::* )(  ))( &::genesis::sequence::PhylipReader::valid_char_lookup ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("utils::CharLookup & ::genesis::sequence::PhylipReader::valid_char_lookup ()")
        )
        .def(
            "validate_chars",
            ( PhylipReader & ( ::genesis::sequence::PhylipReader::* )( std::string const & ))( &::genesis::sequence::PhylipReader::validate_chars ),
            ( boost::python::arg("chars") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PhylipReader & ::genesis::sequence::PhylipReader::validate_chars (std::string const & chars)")
        )
        .def(
            "validate_chars",
            ( std::string ( ::genesis::sequence::PhylipReader::* )(  ) const )( &::genesis::sequence::PhylipReader::validate_chars ),
            get_docstring("std::string ::genesis::sequence::PhylipReader::validate_chars () const")
        )
    ;
}
