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

template <typename Tree>
void PythonExportClass_BipartitionSet(std::string name)
{

    // -------------------------------------------------------------------
    //     Class BipartitionSet
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using BipartitionSetType = BipartitionSet<typename Tree>;

    boost::python::class_< BipartitionSetType > ( name.c_str(), boost::python::init< TreeType & >(( boost::python::arg("tree") )) )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( BipartitionSetType::* )(  ))( &BipartitionSetType::dump )
        )
        .def(
            "find_smallest_subtree",
            ( BipartitionType * ( BipartitionSetType::* )( std::vector< NodeType * > ))( &BipartitionSetType::find_smallest_subtree ),
            ( boost::python::arg("nodes") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("BipartitionType * ::genesis::tree::BipartitionSet::find_smallest_subtree (std::vector< NodeType * > nodes)")
        )
        .def(
            "get_subtree_edges",
            ( std::unordered_set< size_t > ( BipartitionSetType::* )( LinkType * ))( &BipartitionSetType::get_subtree_edges ),
            ( boost::python::arg("subtree") )
        )
        .def(
            "make",
            ( void ( BipartitionSetType::* )(  ))( &BipartitionSetType::make )
        )
        .def(
            "make_index",
            ( void ( BipartitionSetType::* )(  ))( &BipartitionSetType::make_index )
        )
        .def(
            "validate",
            ( bool ( BipartitionSetType::* )(  ))( &BipartitionSetType::validate )
        )
    ;
}
