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

template <class NodeDataType, class EdgeDataType>
void PythonExportClass_TreeEdge(std::string name)
{

    // -------------------------------------------------------------------
    //     Class TreeEdge
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using TreeEdgeType = TreeEdge<class NodeDataType, class EdgeDataType>;

    boost::python::class_< TreeEdgeType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< size_t, LinkType *, LinkType * >(( boost::python::arg("index"), boost::python::arg("primary_link"), boost::python::arg("secondary_link") )) )
        .def( boost::python::init< TreeEdge const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::dump ),
            get_docstring("std::string ::genesis::tree::TreeEdge::dump () const")
        )
        .def(
            "index",
            ( size_t ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::index ),
            get_docstring("size_t ::genesis::tree::TreeEdge::index () const")
        )
        .def(
            "primary_link",
            ( LinkType & ( TreeEdgeType::* )(  ))( &TreeEdgeType::primary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::TreeEdge::primary_link ()")
        )
        .def(
            "primary_link",
            ( LinkType const & ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::primary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::TreeEdge::primary_link () const")
        )
        .def(
            "primary_node",
            ( NodeType & ( TreeEdgeType::* )(  ))( &TreeEdgeType::primary_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType & ::genesis::tree::TreeEdge::primary_node ()")
        )
        .def(
            "primary_node",
            ( NodeType const & ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::primary_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType const & ::genesis::tree::TreeEdge::primary_node () const")
        )
        .def(
            "reset_index",
            ( TreeEdge & ( TreeEdgeType::* )( size_t ))( &TreeEdgeType::reset_index ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "reset_primary_link",
            ( TreeEdge & ( TreeEdgeType::* )( LinkType * ))( &TreeEdgeType::reset_primary_link ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "reset_secondary_link",
            ( TreeEdge & ( TreeEdgeType::* )( LinkType * ))( &TreeEdgeType::reset_secondary_link ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "secondary_link",
            ( LinkType & ( TreeEdgeType::* )(  ))( &TreeEdgeType::secondary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::TreeEdge::secondary_link ()")
        )
        .def(
            "secondary_link",
            ( LinkType const & ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::secondary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::TreeEdge::secondary_link () const")
        )
        .def(
            "secondary_node",
            ( NodeType & ( TreeEdgeType::* )(  ))( &TreeEdgeType::secondary_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType & ::genesis::tree::TreeEdge::secondary_node ()")
        )
        .def(
            "secondary_node",
            ( NodeType const & ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::secondary_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType const & ::genesis::tree::TreeEdge::secondary_node () const")
        )
    ;
}
