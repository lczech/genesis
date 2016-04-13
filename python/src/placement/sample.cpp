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

PYTHON_EXPORT_CLASS (Sample, "placement")
{

    // -------------------------------------------------------------------
    //     Class Sample
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::Sample > ( "Sample", boost::python::init<  >(  ) )
        .def( boost::python::init< PlacementTree const & >(( boost::python::arg("tree") )) )
        .def( boost::python::init< Sample const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_pquery",
            ( Pquery & ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::add_pquery ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Pquery & ::genesis::placement::Sample::add_pquery ()")
        )
        .def(
            "add_pquery",
            ( Pquery & ( ::genesis::placement::Sample::* )( Pquery const & ))( &::genesis::placement::Sample::add_pquery ),
            ( boost::python::arg("other") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Pquery & ::genesis::placement::Sample::add_pquery (Pquery const & other)")
        )
        .def(
            "clear",
            ( void ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::clear ),
            get_docstring("void ::genesis::placement::Sample::clear ()")
        )
        .def(
            "clear_pqueries",
            ( void ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::clear_pqueries ),
            get_docstring("void ::genesis::placement::Sample::clear_pqueries ()")
        )
        .def(
            "pqueries",
            ( utils::Range< genesis::placement::Sample::const_iterator_pqueries > ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::pqueries ),
            get_docstring("utils::Range< const_iterator_pqueries > ::genesis::placement::Sample::pqueries () const")
        )
        .def(
            "pqueries",
            ( utils::Range< genesis::placement::Sample::iterator_pqueries > ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::pqueries ),
            get_docstring("utils::Range< iterator_pqueries > ::genesis::placement::Sample::pqueries ()")
        )
        .def(
            "pquery_at",
            ( Pquery & ( ::genesis::placement::Sample::* )( size_t ))( &::genesis::placement::Sample::pquery_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Pquery & ::genesis::placement::Sample::pquery_at (size_t index)")
        )
        .def(
            "pquery_at",
            ( Pquery const & ( ::genesis::placement::Sample::* )( size_t ) const )( &::genesis::placement::Sample::pquery_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Pquery const & ::genesis::placement::Sample::pquery_at (size_t index) const")
        )
        .def(
            "pquery_size",
            ( size_t ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::pquery_size ),
            get_docstring("size_t ::genesis::placement::Sample::pquery_size () const")
        )
        .def(
            "remove_pquery_at",
            ( void ( ::genesis::placement::Sample::* )( size_t ))( &::genesis::placement::Sample::remove_pquery_at ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::placement::Sample::remove_pquery_at (size_t index)")
        )
        .def(
            "swap",
            ( void ( ::genesis::placement::Sample::* )( Sample & ))( &::genesis::placement::Sample::swap ),
            ( boost::python::arg("other") ),
            get_docstring("void ::genesis::placement::Sample::swap (Sample & other)")
        )
        .def(
            "tree",
            ( PlacementTree & ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::tree ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PlacementTree & ::genesis::placement::Sample::tree ()")
        )
        .def(
            "tree",
            ( PlacementTree const & ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::tree ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PlacementTree const & ::genesis::placement::Sample::tree () const")
        )

        // Iterators

        // .def(
        //     "__iter__",
        //     boost::python::range ( &::genesis::placement::Sample::begin, &::genesis::placement::Sample::end )
        // )
    ;
}
