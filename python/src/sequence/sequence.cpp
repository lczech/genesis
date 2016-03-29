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

PYTHON_EXPORT_CLASS (Sequence, "sequence")
{

    // -------------------------------------------------------------------
    //     Class Sequence
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::sequence::Sequence > ( "Sequence", boost::python::init<  >(  ) )
        .def( boost::python::init< std::string const &, std::string const & >(( boost::python::arg("label"), boost::python::arg("sites") )) )
        .def( boost::python::init< std::string const &, std::string const &, std::string const & >(( boost::python::arg("label"), boost::python::arg("metadata"), boost::python::arg("sites") )) )
        .def( boost::python::init< Sequence const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "cbegin",
            ( ::genesis::sequence::Sequence::const_iterator ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::cbegin )
        )
        .def(
            "cend",
            ( ::genesis::sequence::Sequence::const_iterator ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::cend )
        )
        .def(
            "label",
            ( std::string const & ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::label ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "label",
            ( void ( ::genesis::sequence::Sequence::* )( std::string const & ))( &::genesis::sequence::Sequence::label ),
            ( boost::python::arg("value") )
        )
        .def(
            "length",
            ( size_t ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::length ),
            get_docstring("size_t ::genesis::sequence::Sequence::length () const")
        )
        .def(
            "metadata",
            ( std::string const & ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::metadata ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "metadata",
            ( void ( ::genesis::sequence::Sequence::* )( std::string const & ))( &::genesis::sequence::Sequence::metadata ),
            ( boost::python::arg("value") )
        )
        .def(
            "site_at",
            ( char ( ::genesis::sequence::Sequence::* )( size_t ) const )( &::genesis::sequence::Sequence::site_at ),
            ( boost::python::arg("index") )
        )
        .def(
            "sites",
            ( std::string & ( ::genesis::sequence::Sequence::* )(  ))( &::genesis::sequence::Sequence::sites ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "sites",
            ( std::string const & ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::sites ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        // .def(
        //     "sites",
        //     ( void ( ::genesis::sequence::Sequence::* )( std::string && ))( &::genesis::sequence::Sequence::sites ),
        //     ( boost::python::arg("value") )
        // )
        .def(
            "sites",
            ( void ( ::genesis::sequence::Sequence::* )( std::string const & ))( &::genesis::sequence::Sequence::sites ),
            ( boost::python::arg("value") )
        )
        .def(
            "size",
            ( size_t ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::size ),
            get_docstring("size_t ::genesis::sequence::Sequence::size () const")
        )
        .def(
            "swap",
            ( void ( ::genesis::sequence::Sequence::* )( Sequence & ))( &::genesis::sequence::Sequence::swap ),
            ( boost::python::arg("other") )
        )

        // Operators

        .def(
            "__getitem__",
            ( char ( ::genesis::sequence::Sequence::* )( size_t ) const )( &::genesis::sequence::Sequence::operator[] ),
            ( boost::python::arg("index") )
        )

        // Iterators

        // .def(
        //     "__iter__",
        //     boost::python::range ( &::genesis::sequence::Sequence::begin, &::genesis::sequence::Sequence::end )
        // )
    ;
}
