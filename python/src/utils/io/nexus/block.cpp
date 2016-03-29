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
PYTHON_EXPORT_CLASS (NexusBlock, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusBlock
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusBlock > ( "NexusBlock", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusBlock const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "block_name",
            ( std::string ( ::genesis::utils::NexusBlock::* )(  ) const )( &::genesis::utils::NexusBlock::block_name )
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusBlock::* )( std::ostream & ) const )( &::genesis::utils::NexusBlock::to_stream ),
            ( boost::python::arg("os") )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_io_nexus_block_export, "utils")
{

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, NexusBlock const & ))( &::genesis::utils::operator<< ),
        ( boost::python::arg("os"), boost::python::arg("block") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>()
    );
}
*/
