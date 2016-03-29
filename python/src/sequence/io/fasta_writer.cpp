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

PYTHON_EXPORT_CLASS (FastaWriter, "sequence")
{

    // -------------------------------------------------------------------
    //     Class FastaWriter
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::sequence::FastaWriter > ( "FastaWriter", boost::python::init<  >(  ) )
        .def( boost::python::init< FastaWriter const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "line_length",
            ( FastaWriter & ( ::genesis::sequence::FastaWriter::* )( size_t ))( &::genesis::sequence::FastaWriter::line_length ),
            ( boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
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
            ( boost::python::arg("sset"), boost::python::arg("fn") ),
            get_docstring("void ::genesis::sequence::FastaWriter::to_file (SequenceSet const & sset, std::string const & fn) const")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::sequence::FastaWriter::* )( SequenceSet const &, std::ostream & ) const )( &::genesis::sequence::FastaWriter::to_stream ),
            ( boost::python::arg("sset"), boost::python::arg("os") ),
            get_docstring("void ::genesis::sequence::FastaWriter::to_stream (SequenceSet const & sset, std::ostream & os) const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::sequence::FastaWriter::* )( SequenceSet const & ) const )( &::genesis::sequence::FastaWriter::to_string ),
            ( boost::python::arg("sset") ),
            get_docstring("std::string ::genesis::sequence::FastaWriter::to_string (SequenceSet const & sset) const")
        )
    ;
}
