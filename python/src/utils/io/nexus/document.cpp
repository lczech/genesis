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

/*
PYTHON_EXPORT_CLASS (NexusDocument, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusDocument
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusDocument > ( "NexusDocument", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusDocument const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "get_block",
            ( NexusBlock * ( ::genesis::utils::NexusDocument::* )( std::string ))( &::genesis::utils::NexusDocument::get_block ),
            ( boost::python::arg("block_name") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "get_block",
            ( NexusBlock const * ( ::genesis::utils::NexusDocument::* )( std::string ) const )( &::genesis::utils::NexusDocument::get_block ),
            ( boost::python::arg("block_name") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "has_block",
            ( bool ( ::genesis::utils::NexusDocument::* )( std::string ) const )( &::genesis::utils::NexusDocument::has_block ),
            ( boost::python::arg("block_name") )
        )
        .def(
            "set_block",
            ( NexusBlock * ( ::genesis::utils::NexusDocument::* )( std::unique_ptr< NexusBlock > ))( &::genesis::utils::NexusDocument::set_block ),
            ( boost::python::arg("block") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::NexusDocument::begin, &::genesis::utils::NexusDocument::end )
        )
    ;
}
*/
