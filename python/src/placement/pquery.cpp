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

PYTHON_EXPORT_CLASS (Pquery, "placement")
{

    // -------------------------------------------------------------------
    //     Class Pquery
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::Pquery > ( "Pquery", boost::python::init<  >(  ) )
        .def( boost::python::init< Pquery const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_name",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( PqueryName const & ))( &::genesis::placement::Pquery::add_name ),
            ( boost::python::arg("other") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryName & ::genesis::placement::Pquery::add_name (PqueryName const & other)")
        )
        .def(
            "add_name",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( std::string, double ))( &::genesis::placement::Pquery::add_name ),
            ( boost::python::arg("name")=(std::string)(""), boost::python::arg("multiplicity")=(double)(0.0) ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryName & ::genesis::placement::Pquery::add_name (std::string name="", double multiplicity=0.0)")
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PlacementTreeEdge & ))( &::genesis::placement::Pquery::add_placement ),
            ( boost::python::arg("edge") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryPlacement & ::genesis::placement::Pquery::add_placement (PlacementTreeEdge & edge)")
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PlacementTreeEdge &, PqueryPlacement const & ))( &::genesis::placement::Pquery::add_placement ),
            ( boost::python::arg("edge"), boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryPlacement & ::genesis::placement::Pquery::add_placement (PlacementTreeEdge & edge, PqueryPlacement const & val)")
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PqueryPlacement const & ))( &::genesis::placement::Pquery::add_placement ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryPlacement & ::genesis::placement::Pquery::add_placement (PqueryPlacement const & val)")
        )
        .def(
            "clear",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear ),
            get_docstring("void ::genesis::placement::Pquery::clear ()")
        )
        .def(
            "clear_names",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear_names ),
            get_docstring("void ::genesis::placement::Pquery::clear_names ()")
        )
        .def(
            "clear_placements",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear_placements ),
            get_docstring("void ::genesis::placement::Pquery::clear_placements ()")
        )
        .def(
            "name_at",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::name_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryName & ::genesis::placement::Pquery::name_at (size_t index)")
        )
        .def(
            "name_at",
            ( PqueryName const & ( ::genesis::placement::Pquery::* )( size_t ) const )( &::genesis::placement::Pquery::name_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryName const & ::genesis::placement::Pquery::name_at (size_t index) const")
        )
        .def(
            "name_size",
            ( size_t ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::name_size ),
            get_docstring("size_t ::genesis::placement::Pquery::name_size () const")
        )
        .def(
            "names",
            ( utils::Range< ::genesis::placement::Pquery::const_iterator_names > ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::names ),
            get_docstring("utils::Range< const_iterator_names > ::genesis::placement::Pquery::names () const")
        )
        .def(
            "names",
            ( utils::Range< ::genesis::placement::Pquery::iterator_names > ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::names ),
            get_docstring("utils::Range< iterator_names > ::genesis::placement::Pquery::names ()")
        )
        .def(
            "placement_at",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::placement_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryPlacement & ::genesis::placement::Pquery::placement_at (size_t index)")
        )
        .def(
            "placement_at",
            ( PqueryPlacement const & ( ::genesis::placement::Pquery::* )( size_t ) const )( &::genesis::placement::Pquery::placement_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PqueryPlacement const & ::genesis::placement::Pquery::placement_at (size_t index) const")
        )
        .def(
            "placement_size",
            ( size_t ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::placement_size ),
            get_docstring("size_t ::genesis::placement::Pquery::placement_size () const")
        )
        .def(
            "placements",
            ( utils::Range< ::genesis::placement::Pquery::const_iterator_placements > ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::placements ),
            get_docstring("utils::Range< const_iterator_placements > ::genesis::placement::Pquery::placements () const")
        )
        .def(
            "placements",
            ( utils::Range< ::genesis::placement::Pquery::iterator_placements > ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::placements ),
            get_docstring("utils::Range< iterator_placements > ::genesis::placement::Pquery::placements ()")
        )
        .def(
            "remove_name_at",
            ( void ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::remove_name_at ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::placement::Pquery::remove_name_at (size_t index)")
        )
        .def(
            "remove_placement_at",
            ( void ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::remove_placement_at ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::placement::Pquery::remove_placement_at (size_t index)")
        )

        // Iterators

        // .add_property(
        //     "names",
        //     boost::python::range ( &::genesis::placement::Pquery::begin_names, &::genesis::placement::Pquery::end_names )
        // )
        // .add_property(
        //     "placements",
        //     boost::python::range ( &::genesis::placement::Pquery::begin_placements, &::genesis::placement::Pquery::end_placements )
        // )
    ;
}
