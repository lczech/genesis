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

using namespace ::genesis::tree;

/*
PYTHON_EXPORT_CLASS (NewickBroker, "tree")
{

    // -------------------------------------------------------------------
    //     Class NewickBroker
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::NewickBroker > ( "NewickBroker", boost::python::init<  >(  ) )
        .def( boost::python::init< NewickBroker const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "assign_ranks",
            ( void ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::assign_ranks ),
            get_docstring("void ::genesis::tree::NewickBroker::assign_ranks () const")
        )
        .def(
            "at",
            ( NewickBrokerElement & ( ::genesis::tree::NewickBroker::* )( std::size_t ))( &::genesis::tree::NewickBroker::at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NewickBrokerElement & ::genesis::tree::NewickBroker::at (std::size_t index)")
        )
        .def(
            "at",
            ( NewickBrokerElement const & ( ::genesis::tree::NewickBroker::* )( std::size_t ) const )( &::genesis::tree::NewickBroker::at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "bottom",
            ( NewickBrokerElement & ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::bottom ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NewickBrokerElement & ::genesis::tree::NewickBroker::bottom ()")
        )
        .def(
            "bottom",
            ( NewickBrokerElement const & ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::bottom ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "cbegin",
            ( const_iterator ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::cbegin ),
            get_docstring("const_iterator ::genesis::tree::NewickBroker::cbegin () const")
        )
        .def(
            "cend",
            ( const_iterator ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::cend ),
            get_docstring("const_iterator ::genesis::tree::NewickBroker::cend () const")
        )
        .def(
            "clear",
            ( void ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::clear ),
            get_docstring("void ::genesis::tree::NewickBroker::clear ()")
        )
        .def(
            "crbegin",
            ( const_reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::crbegin ),
            get_docstring("const_reverse_iterator ::genesis::tree::NewickBroker::crbegin ()")
        )
        .def(
            "crend",
            ( const_reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::crend ),
            get_docstring("const_reverse_iterator ::genesis::tree::NewickBroker::crend ()")
        )
        .def(
            "dump",
            ( std::string ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::dump ),
            get_docstring("std::string ::genesis::tree::NewickBroker::dump () const")
        )
        .def(
            "empty",
            ( bool ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::empty ),
            get_docstring("bool ::genesis::tree::NewickBroker::empty () const")
        )
        .def(
            "inner_count",
            ( int ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::inner_count )
        )
        .def(
            "is_bifurcating",
            ( bool ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::is_bifurcating )
        )
        .def(
            "leaf_count",
            ( int ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::leaf_count ),
            get_docstring("int ::genesis::tree::NewickBroker::leaf_count () const")
        )
        .def(
            "max_rank",
            ( int ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::max_rank ),
            get_docstring("int ::genesis::tree::NewickBroker::max_rank () const")
        )
        .def(
            "node_count",
            ( int ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::node_count ),
            get_docstring("int ::genesis::tree::NewickBroker::node_count () const")
        )
        .def(
            "pop_bottom",
            ( void ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::pop_bottom )
        )
        .def(
            "pop_top",
            ( void ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::pop_top )
        )
        .def(
            "push_bottom",
            ( void ( ::genesis::tree::NewickBroker::* )( NewickBrokerElement && ))( &::genesis::tree::NewickBroker::push_bottom ),
            ( boost::python::arg("node") )
        )
        .def(
            "push_bottom",
            ( void ( ::genesis::tree::NewickBroker::* )( NewickBrokerElement const & ))( &::genesis::tree::NewickBroker::push_bottom ),
            ( boost::python::arg("node") )
        )
        .def(
            "push_top",
            ( void ( ::genesis::tree::NewickBroker::* )( NewickBrokerElement && ))( &::genesis::tree::NewickBroker::push_top ),
            ( boost::python::arg("node") )
        )
        .def(
            "push_top",
            ( void ( ::genesis::tree::NewickBroker::* )( NewickBrokerElement const & ))( &::genesis::tree::NewickBroker::push_top ),
            ( boost::python::arg("node") )
        )
        .def(
            "rbegin",
            ( const_reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::rbegin ),
            get_docstring("const_reverse_iterator ::genesis::tree::NewickBroker::rbegin () const")
        )
        .def(
            "rbegin",
            ( reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::rbegin ),
            get_docstring("reverse_iterator ::genesis::tree::NewickBroker::rbegin ()")
        )
        .def(
            "rend",
            ( const_reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::rend ),
            get_docstring("const_reverse_iterator ::genesis::tree::NewickBroker::rend () const")
        )
        .def(
            "rend",
            ( reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::rend ),
            get_docstring("reverse_iterator ::genesis::tree::NewickBroker::rend ()")
        )
        .def(
            "size",
            ( size_t ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::size ),
            get_docstring("size_t ::genesis::tree::NewickBroker::size () const")
        )
        .def(
            "top",
            ( NewickBrokerElement & ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::top ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NewickBrokerElement & ::genesis::tree::NewickBroker::top ()")
        )
        .def(
            "top",
            ( NewickBrokerElement const & ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::top ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "validate",
            ( bool ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::validate ),
            get_docstring("bool ::genesis::tree::NewickBroker::validate () const")
        )

        // Operators

        .def(
            "__getitem__",
            ( NewickBrokerElement & ( ::genesis::tree::NewickBroker::* )( std::size_t ))( &::genesis::tree::NewickBroker::operator[] ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NewickBrokerElement & ::genesis::tree::NewickBroker::operator[] (std::size_t index)")
        )
        .def(
            "__getitem__",
            ( NewickBrokerElement const & ( ::genesis::tree::NewickBroker::* )( std::size_t ) const )( &::genesis::tree::NewickBroker::operator[] ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::tree::NewickBroker::begin, &::genesis::tree::NewickBroker::end )
        )
    ;
}
*/
