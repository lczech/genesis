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

PYTHON_EXPORT_CLASS (NexusTaxa, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusTaxa
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusTaxa > ( "NexusTaxa", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusTaxa const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_taxa",
            ( void ( ::genesis::utils::NexusTaxa::* )( std::vector< std::string > ))( &::genesis::utils::NexusTaxa::add_taxa ),
            ( boost::python::arg("taxa") )
        )
        .def(
            "add_taxon",
            ( void ( ::genesis::utils::NexusTaxa::* )( std::string ))( &::genesis::utils::NexusTaxa::add_taxon ),
            ( boost::python::arg("name") )
        )
        .def(
            "block_name",
            ( std::string ( ::genesis::utils::NexusTaxa::* )(  ) const )( &::genesis::utils::NexusTaxa::block_name )
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::NexusTaxa::* )(  ))( &::genesis::utils::NexusTaxa::clear )
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::NexusTaxa::* )(  ) const )( &::genesis::utils::NexusTaxa::empty )
        )
        .def(
            "erase_taxon",
            ( void ( ::genesis::utils::NexusTaxa::* )( std::string ))( &::genesis::utils::NexusTaxa::erase_taxon ),
            ( boost::python::arg("name") )
        )
        .def(
            "has_taxon",
            ( bool ( ::genesis::utils::NexusTaxa::* )( std::string ) const )( &::genesis::utils::NexusTaxa::has_taxon ),
            ( boost::python::arg("name") )
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::NexusTaxa::* )(  ) const )( &::genesis::utils::NexusTaxa::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::NexusTaxa::* )( NexusTaxa & ))( &::genesis::utils::NexusTaxa::swap ),
            ( boost::python::arg("other") )
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusTaxa::* )( std::ostream & ) const )( &::genesis::utils::NexusTaxa::to_stream ),
            ( boost::python::arg("os") )
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::NexusTaxa::begin, &::genesis::utils::NexusTaxa::end )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_io_nexus_taxa_export, "utils")
{

    boost::python::def(
        "swap",
        ( void ( * )( NexusTaxa &, NexusTaxa & ))( &::genesis::utils::swap ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );
}
