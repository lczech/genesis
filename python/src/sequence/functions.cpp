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

PYTHON_EXPORT_FUNCTIONS(sequence_functions_export, "sequence")
{

    boost::python::def(
        "find_sequence",
        ( Sequence const * ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::find_sequence ),
        ( boost::python::arg("set"), boost::python::arg("label") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("Sequence const * ::genesis::sequence::find_sequence (SequenceSet const & set, std::string const & label)")
    );

    boost::python::def(
        "is_alignment",
        ( bool ( * )( SequenceSet const & ))( &::genesis::sequence::is_alignment ),
        ( boost::python::arg("set") ),
        get_docstring("bool ::genesis::sequence::is_alignment (SequenceSet const & set)")
    );

    boost::python::def(
        "validate_chars",
        ( bool ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::validate_chars ),
        ( boost::python::arg("set"), boost::python::arg("chars") ),
        get_docstring("bool ::genesis::sequence::validate_chars (SequenceSet const & set, std::string const & chars)")
    );

    boost::python::def(
        "gapyness",
        ( double ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::gapyness ),
        ( boost::python::arg("set"), boost::python::arg("undetermined_chars") ),
        get_docstring("double ::genesis::sequence::gapyness (SequenceSet const & set, std::string const & undetermined_chars)")
    );

    boost::python::def(
        "count_chars",
        ( size_t ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::count_chars ),
        ( boost::python::arg("set"), boost::python::arg("chars") ),
        get_docstring("size_t ::genesis::sequence::count_chars (SequenceSet const & set, std::string const & chars)")
    );

    boost::python::def(
        "total_length",
        ( size_t ( * )( SequenceSet const & ))( &::genesis::sequence::total_length ),
        ( boost::python::arg("set") ),
        get_docstring("size_t ::genesis::sequence::total_length (SequenceSet const & set)")
    );

    // boost::python::def(
    //     "make_lookup_table",
    //     ( std::array< bool, 128 > ( * )( std::string const & ))( &::genesis::sequence::make_lookup_table ),
    //     ( boost::python::arg("chars") ),
    //     get_docstring("std::array< bool, 128 > ::genesis::sequence::make_lookup_table (std::string const & chars)")
    // );

    boost::python::def(
        "base_frequencies",
        ( std::map< char, double > ( * )( Sequence const &, std::string const & ))( &::genesis::sequence::base_frequencies ),
        ( boost::python::arg("seq"), boost::python::arg("plain_chars") ),
        get_docstring("std::map< char, double > ::genesis::sequence::base_frequencies (Sequence const & seq, std::string const & plain_chars)")
    );

    boost::python::def(
        "base_frequencies",
        ( std::map< char, double > ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::base_frequencies ),
        ( boost::python::arg("set"), boost::python::arg("plain_chars") ),
        get_docstring("std::map< char, double > ::genesis::sequence::base_frequencies (SequenceSet const & set, std::string const & plain_chars)")
    );

    // boost::python::def(
    //     "base_frequencies_accumulator",
    //     ( std::map< char, double > ( * )( std::map< char, size_t > const &, std::string const & ))( &::genesis::sequence::base_frequencies_accumulator ),
    //     ( boost::python::arg("sitehistogram"), boost::python::arg("plain_chars") ),
    //     get_docstring("std::map< char, double > ::genesis::sequence::base_frequencies_accumulator (std::map< char, size_t > const & sitehistogram, std::string const & plain_chars)")
    // );

    boost::python::def(
        "site_histogram",
        ( std::map< char, size_t > ( * )( Sequence const & ))( &::genesis::sequence::site_histogram ),
        ( boost::python::arg("seq") ),
        get_docstring("std::map< char, size_t > ::genesis::sequence::site_histogram (Sequence const & seq)")
    );

    boost::python::def(
        "site_histogram",
        ( std::map< char, size_t > ( * )( SequenceSet const & ))( &::genesis::sequence::site_histogram ),
        ( boost::python::arg("set") ),
        get_docstring("std::map< char, size_t > ::genesis::sequence::site_histogram (SequenceSet const & set)")
    );

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, Sequence const & ))( &::genesis::sequence::operator<< ),
        ( boost::python::arg("out"), boost::python::arg("seq") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("std::ostream & ::genesis::sequence::operator<< (std::ostream & out, Sequence const & seq)")
    );

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, SequenceSet const & ))( &::genesis::sequence::operator<< ),
        ( boost::python::arg("out"), boost::python::arg("set") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("std::ostream & ::genesis::sequence::operator<< (std::ostream & out, SequenceSet const & set)")
    );

    boost::python::def(
        "print",
        ( std::string ( * )( Sequence const &, bool, size_t ))( &::genesis::sequence::print ),
        ( boost::python::arg("seq"), boost::python::arg("print_label"), boost::python::arg("length_limit") ),
        get_docstring("std::string ::genesis::sequence::print (Sequence const & seq, bool print_label, size_t length_limit)")
    );

    boost::python::def(
        "print",
        ( std::string ( * )( SequenceSet const &, bool, size_t, size_t ))( &::genesis::sequence::print ),
        ( boost::python::arg("set"), boost::python::arg("print_label"), boost::python::arg("length_limit"), boost::python::arg("sequence_limit") ),
        get_docstring("std::string ::genesis::sequence::print (SequenceSet const & set, bool print_label, size_t length_limit, size_t sequence_limit)")
    );

    boost::python::def(
        "print_color",
        ( std::string ( * )( Sequence const &, std::map< char, std::string > const &, bool, size_t, bool ))( &::genesis::sequence::print_color ),
        ( boost::python::arg("seq"), boost::python::arg("colors"), boost::python::arg("print_label"), boost::python::arg("length_limit"), boost::python::arg("background") ),
        get_docstring("std::string ::genesis::sequence::print_color (Sequence const & seq, std::map< char, std::string > const & colors, bool print_label, size_t length_limit, bool background)")
    );

    boost::python::def(
        "print_color",
        ( std::string ( * )( SequenceSet const &, std::map< char, std::string > const &, bool, size_t, size_t, bool ))( &::genesis::sequence::print_color ),
        ( boost::python::arg("set"), boost::python::arg("colors"), boost::python::arg("print_label"), boost::python::arg("length_limit"), boost::python::arg("sequence_limit"), boost::python::arg("background") ),
        get_docstring("std::string ::genesis::sequence::print_color (SequenceSet const & set, std::map< char, std::string > const & colors, bool print_label, size_t length_limit, size_t sequence_limit, bool background)")
    );

    // boost::python::def(
    //     "print_to_ostream",
    //     ( void ( * )( std::ostream &, Sequence const &, std::map< char, std::string > const &, bool, size_t, bool ))( &::genesis::sequence::print_to_ostream ),
    //     ( boost::python::arg("out"), boost::python::arg("seq"), boost::python::arg("colors"), boost::python::arg("print_label"), boost::python::arg("length_limit"), boost::python::arg("background") ),
    //     get_docstring("void ::genesis::sequence::print_to_ostream (std::ostream & out, Sequence const & seq, std::map< char, std::string > const & colors, bool print_label, size_t length_limit, bool background)")
    // );

    // boost::python::def(
    //     "print_to_ostream",
    //     ( void ( * )( std::ostream &, SequenceSet const &, std::map< char, std::string > const &, bool, size_t, size_t, bool ))( &::genesis::sequence::print_to_ostream ),
    //     ( boost::python::arg("out"), boost::python::arg("set"), boost::python::arg("colors"), boost::python::arg("print_label"), boost::python::arg("length_limit"), boost::python::arg("sequence_limit"), boost::python::arg("background") ),
    //     get_docstring("void ::genesis::sequence::print_to_ostream (std::ostream & out, SequenceSet const & set, std::map< char, std::string > const & colors, bool print_label, size_t length_limit, size_t sequence_limit, bool background)")
    // );
}
