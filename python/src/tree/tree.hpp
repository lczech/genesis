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
void PythonExportClass_Tree(std::string name)
{

    // -------------------------------------------------------------------
    //     Class Tree
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using TreeType = Tree<class NodeDataType, class EdgeDataType>;

    boost::python::class_< TreeType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< const TreeType & >(( boost::python::arg("other") )) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( TreeType::* )(  ))( &TreeType::clear ),
            get_docstring("void ::genesis::tree::Tree::clear ()")
        )
        .def(
            "edge_at",
            ( EdgeType & ( TreeType::* )( size_t ))( &TreeType::edge_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("EdgeType & ::genesis::tree::Tree::edge_at (size_t index)")
        )
        .def(
            "edge_at",
            ( EdgeType const & ( TreeType::* )( size_t ) const )( &TreeType::edge_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("EdgeType const & ::genesis::tree::Tree::edge_at (size_t index) const")
        )
        .def(
            "edge_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::edge_count ),
            get_docstring("size_t ::genesis::tree::Tree::edge_count () const")
        )
        .def(
            "empty",
            ( bool ( TreeType::* )(  ) const )( &TreeType::empty ),
            get_docstring("bool ::genesis::tree::Tree::empty () const")
        )
        .def(
            "export_content",
            ( void ( TreeType::* )( LinkContainer &, NodeContainer &, EdgeContainer & ))( &TreeType::export_content ),
            ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") ),
            get_docstring("void ::genesis::tree::Tree::export_content (LinkContainer & links, NodeContainer & nodes, EdgeContainer & edges)")
        )
        .def(
            "import_content",
            ( void ( TreeType::* )( LinkContainer &, NodeContainer &, EdgeContainer & ))( &TreeType::import_content ),
            ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") ),
            get_docstring("void ::genesis::tree::Tree::import_content (LinkContainer & links, NodeContainer & nodes, EdgeContainer & edges)")
        )
        .def(
            "link_at",
            ( LinkType & ( TreeType::* )( size_t ))( &TreeType::link_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::Tree::link_at (size_t index)")
        )
        .def(
            "link_at",
            ( LinkType const & ( TreeType::* )( size_t ) const )( &TreeType::link_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::Tree::link_at (size_t index) const")
        )
        .def(
            "link_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::link_count ),
            get_docstring("size_t ::genesis::tree::Tree::link_count () const")
        )
        .def(
            "node_at",
            ( NodeType & ( TreeType::* )( size_t ))( &TreeType::node_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType & ::genesis::tree::Tree::node_at (size_t index)")
        )
        .def(
            "node_at",
            ( NodeType const & ( TreeType::* )( size_t ) const )( &TreeType::node_at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType const & ::genesis::tree::Tree::node_at (size_t index) const")
        )
        .def(
            "node_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::node_count ),
            get_docstring("size_t ::genesis::tree::Tree::node_count () const")
        )
        .def(
            "root_link",
            ( LinkType & ( TreeType::* )(  ))( &TreeType::root_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::Tree::root_link ()")
        )
        .def(
            "root_link",
            ( LinkType const & ( TreeType::* )(  ) const )( &TreeType::root_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::Tree::root_link () const")
        )
        .def(
            "root_node",
            ( NodeType & ( TreeType::* )(  ))( &TreeType::root_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType & ::genesis::tree::Tree::root_node ()")
        )
        .def(
            "root_node",
            ( NodeType const & ( TreeType::* )(  ) const )( &TreeType::root_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType const & ::genesis::tree::Tree::root_node () const")
        )
        .def(
            "swap",
            ( void ( TreeType::* )( TreeType & ))( &TreeType::swap ),
            ( boost::python::arg("other") ),
            get_docstring("void ::genesis::tree::Tree::swap (TreeType & other)")
        )

        // Iterators

        .add_property(
            "edges",
            boost::python::range ( &TreeType::begin_edges, &TreeType::end_edges )
        )
        .add_property(
            "links",
            boost::python::range ( &TreeType::begin_links, &TreeType::end_links )
        )
        .add_property(
            "nodes",
            boost::python::range ( &TreeType::begin_nodes, &TreeType::end_nodes )
        )
    ;
}
