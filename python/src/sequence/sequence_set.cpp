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

PYTHON_EXPORT_CLASS (SequenceSet, "sequence")
{

    // -------------------------------------------------------------------
    //     Class SequenceSet
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::sequence::SequenceSet > ( "SequenceSet", boost::python::init<  >(  ) )
        .def( boost::python::init< SequenceSet const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "cbegin",
            ( ::genesis::sequence::SequenceSet::const_iterator ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::cbegin )
        )
        .def(
            "cend",
            ( ::genesis::sequence::SequenceSet::const_iterator ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::cend )
        )
        .def(
            "clear",
            ( void ( ::genesis::sequence::SequenceSet::* )(  ))( &::genesis::sequence::SequenceSet::clear ),
            get_docstring("void ::genesis::sequence::SequenceSet::clear ()")
        )
        // .def(
        //     "push_back",
        //     ( void ( ::genesis::sequence::SequenceSet::* )( Sequence && ))( &::genesis::sequence::SequenceSet::push_back ),
        //     ( boost::python::arg("s") )
        // )
        .def(
            "push_back",
            ( void ( ::genesis::sequence::SequenceSet::* )( Sequence const & ))( &::genesis::sequence::SequenceSet::push_back ),
            ( boost::python::arg("s") )
        )
        .def(
            "size",
            ( size_t ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::sequence::SequenceSet::* )( SequenceSet & ))( &::genesis::sequence::SequenceSet::swap ),
            ( boost::python::arg("other") )
        )

        // Operators

        // .def(
        //     "__getitem__",
        //     ( reference ( ::genesis::sequence::SequenceSet::* )( size_t ))( &::genesis::sequence::SequenceSet::operator[] ),
        //     ( boost::python::arg("index") )
        // )
        // .def(
        //     "__getitem__",
        //     ( const_reference ( ::genesis::sequence::SequenceSet::* )( size_t ) const )( &::genesis::sequence::SequenceSet::operator[] ),
        //     ( boost::python::arg("index") )
        // )

        // Iterators

        // .def(
        //     "__iter__",
        //     boost::python::range ( &::genesis::sequence::SequenceSet::begin, &::genesis::sequence::SequenceSet::end )
        // )
    ;
}
