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

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (NexusTrees, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusTrees
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusTrees > ( "NexusTrees", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusTrees const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_tree",
            ( void ( ::genesis::utils::NexusTrees::* )( std::string, std::string ))( &::genesis::utils::NexusTrees::add_tree ),
            ( boost::python::arg("name"), boost::python::arg("tree") )
        )
        .def(
            "block_name",
            ( std::string ( ::genesis::utils::NexusTrees::* )(  ) const )( &::genesis::utils::NexusTrees::block_name )
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::NexusTrees::* )(  ))( &::genesis::utils::NexusTrees::clear )
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::NexusTrees::* )(  ) const )( &::genesis::utils::NexusTrees::empty )
        )
        .def(
            "erase_tree",
            ( void ( ::genesis::utils::NexusTrees::* )( std::string ))( &::genesis::utils::NexusTrees::erase_tree ),
            ( boost::python::arg("name") )
        )
        .def(
            "has_tree",
            ( bool ( ::genesis::utils::NexusTrees::* )( std::string ) const )( &::genesis::utils::NexusTrees::has_tree ),
            ( boost::python::arg("name") )
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::NexusTrees::* )(  ) const )( &::genesis::utils::NexusTrees::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::NexusTrees::* )( NexusTrees & ))( &::genesis::utils::NexusTrees::swap ),
            ( boost::python::arg("other") )
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusTrees::* )( std::ostream & ) const )( &::genesis::utils::NexusTrees::to_stream ),
            ( boost::python::arg("os") )
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::NexusTrees::begin, &::genesis::utils::NexusTrees::end )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_io_nexus_trees_export, "utils")
{

    boost::python::def(
        "swap",
        ( void ( * )( NexusTrees &, NexusTrees & ))( &::genesis::utils::swap ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );
}
