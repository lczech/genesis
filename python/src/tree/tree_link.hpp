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
void PythonExportClass_TreeLink(std::string name)
{

    // -------------------------------------------------------------------
    //     Class TreeLink
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using TreeLinkType = TreeLink<class NodeDataType, class EdgeDataType>;

    boost::python::class_< TreeLinkType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< size_t, LinkType *, LinkType *, NodeType *, EdgeType * >(( boost::python::arg("index"), boost::python::arg("next"), boost::python::arg("outer"), boost::python::arg("node"), boost::python::arg("edge") )) )
        .def( boost::python::init< TreeLink const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( TreeLinkType::* )(  ) const )( &TreeLinkType::dump ),
            get_docstring("std::string ::genesis::tree::TreeLink::dump () const")
        )
        .def(
            "edge",
            ( EdgeType & ( TreeLinkType::* )(  ))( &TreeLinkType::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("EdgeType & ::genesis::tree::TreeLink::edge ()")
        )
        .def(
            "edge",
            ( EdgeType const & ( TreeLinkType::* )(  ) const )( &TreeLinkType::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("EdgeType const & ::genesis::tree::TreeLink::edge () const")
        )
        .def(
            "index",
            ( size_t ( TreeLinkType::* )(  ) const )( &TreeLinkType::index ),
            get_docstring("size_t ::genesis::tree::TreeLink::index () const")
        )
        .def(
            "is_inner",
            ( bool ( TreeLinkType::* )(  ) const )( &TreeLinkType::is_inner ),
            get_docstring("bool ::genesis::tree::TreeLink::is_inner () const")
        )
        .def(
            "is_leaf",
            ( bool ( TreeLinkType::* )(  ) const )( &TreeLinkType::is_leaf ),
            get_docstring("bool ::genesis::tree::TreeLink::is_leaf () const")
        )
        .def(
            "next",
            ( LinkType & ( TreeLinkType::* )(  ))( &TreeLinkType::next ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::TreeLink::next ()")
        )
        .def(
            "next",
            ( LinkType const & ( TreeLinkType::* )(  ) const )( &TreeLinkType::next ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::TreeLink::next () const")
        )
        .def(
            "node",
            ( NodeType & ( TreeLinkType::* )(  ))( &TreeLinkType::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType & ::genesis::tree::TreeLink::node ()")
        )
        .def(
            "node",
            ( NodeType const & ( TreeLinkType::* )(  ) const )( &TreeLinkType::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType const & ::genesis::tree::TreeLink::node () const")
        )
        .def(
            "outer",
            ( LinkType & ( TreeLinkType::* )(  ))( &TreeLinkType::outer ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::TreeLink::outer ()")
        )
        .def(
            "outer",
            ( LinkType const & ( TreeLinkType::* )(  ) const )( &TreeLinkType::outer ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::TreeLink::outer () const")
        )
        .def(
            "prev",
            ( LinkType & ( TreeLinkType::* )(  ))( &TreeLinkType::prev ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::TreeLink::prev ()")
        )
        .def(
            "prev",
            ( LinkType const & ( TreeLinkType::* )(  ) const )( &TreeLinkType::prev ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::TreeLink::prev () const")
        )
        .def(
            "reset_edge",
            ( TreeLink & ( TreeLinkType::* )( EdgeType * ))( &TreeLinkType::reset_edge ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "reset_index",
            ( TreeLink & ( TreeLinkType::* )( size_t ))( &TreeLinkType::reset_index ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "reset_next",
            ( TreeLink & ( TreeLinkType::* )( LinkType * ))( &TreeLinkType::reset_next ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "reset_node",
            ( TreeLink & ( TreeLinkType::* )( NodeType * ))( &TreeLinkType::reset_node ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "reset_outer",
            ( TreeLink & ( TreeLinkType::* )( LinkType * ))( &TreeLinkType::reset_outer ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
    ;
}
