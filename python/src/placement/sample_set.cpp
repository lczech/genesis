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

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (SampleSet, "placement")
{

    // -------------------------------------------------------------------
    //     Class SampleSet
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::SampleSet > ( "SampleSet", boost::python::init<  >(  ) )
        .def( boost::python::init< SampleSet const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add",
            ( void ( ::genesis::placement::SampleSet::* )( std::string const &, Sample const & ))( &::genesis::placement::SampleSet::add ),
            ( boost::python::arg("name"), boost::python::arg("smp") ),
            get_docstring("void ::genesis::placement::SampleSet::add (std::string const & name, Sample const & smp)")
        )
        // .def(
        //     "at",
        //     ( NamedSample & ( ::genesis::placement::SampleSet::* )( size_t ))( &::genesis::placement::SampleSet::at ),
        //     ( boost::python::arg("index") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     get_docstring("NamedSample & ::genesis::placement::SampleSet::at (size_t index)")
        // )
        // .def(
        //     "at",
        //     ( const NamedSample & ( ::genesis::placement::SampleSet::* )( size_t ) const )( &::genesis::placement::SampleSet::at ),
        //     ( boost::python::arg("index") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     get_docstring("const NamedSample & ::genesis::placement::SampleSet::at (size_t index) const")
        // )
        .def(
            "clear",
            ( void ( ::genesis::placement::SampleSet::* )(  ))( &::genesis::placement::SampleSet::clear ),
            get_docstring("void ::genesis::placement::SampleSet::clear ()")
        )
        .def(
            "empty",
            ( bool ( ::genesis::placement::SampleSet::* )(  ) const )( &::genesis::placement::SampleSet::empty ),
            get_docstring("bool ::genesis::placement::SampleSet::empty () const")
        )
        .def(
            "remove_at",
            ( void ( ::genesis::placement::SampleSet::* )( size_t ))( &::genesis::placement::SampleSet::remove_at ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::placement::SampleSet::remove_at (size_t index)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::placement::SampleSet::* )(  ) const )( &::genesis::placement::SampleSet::size ),
            get_docstring("size_t ::genesis::placement::SampleSet::size () const")
        )
        .def(
            "swap",
            ( void ( ::genesis::placement::SampleSet::* )( SampleSet & ))( &::genesis::placement::SampleSet::swap ),
            ( boost::python::arg("other") )
        )

        // Operators

        // .def(
        //     "__getitem__",
        //     ( NamedSample & ( ::genesis::placement::SampleSet::* )( size_t ))( &::genesis::placement::SampleSet::operator[] ),
        //     ( boost::python::arg("index") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     get_docstring("NamedSample & ::genesis::placement::SampleSet::operator[] (size_t index)")
        // )
        // .def(
        //     "__getitem__",
        //     ( const NamedSample & ( ::genesis::placement::SampleSet::* )( size_t ) const )( &::genesis::placement::SampleSet::operator[] ),
        //     ( boost::python::arg("index") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     get_docstring("const NamedSample & ::genesis::placement::SampleSet::operator[] (size_t index) const")
        // )

        // Iterators

        // .def(
        //     "__iter__",
        //     boost::python::range ( &::genesis::placement::SampleSet::begin, &::genesis::placement::SampleSet::end )
        // )
    ;
}
