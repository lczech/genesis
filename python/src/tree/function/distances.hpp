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

template<class Tree>
void python_export_function_tree_function_distances_Tree ()
{
    boost::python::def(
        "edge_path_length_vector",
        ( std::vector< int > ( * )( const Tree &, const typename Tree::EdgeType * ))( &::genesis::tree::edge_path_length_vector ),
        ( boost::python::arg("tree"), boost::python::arg("edge")=(const typename Tree::EdgeType *)(nullptr) )
    );

    boost::python::def(
        "node_path_length_vector",
        ( std::vector< int > ( * )( const Tree &, const typename Tree::NodeType * ))( &::genesis::tree::node_path_length_vector ),
        ( boost::python::arg("tree"), boost::python::arg("node")=(const typename Tree::NodeType *)(nullptr) ),
        get_docstring("std::vector< int > ::genesis::tree::node_path_length_vector (const Tree & tree, const typename Tree::NodeType * node=nullptr)")
    );

    boost::python::def(
        "closest_leaf_depth_vector",
        ( std::vector< std::pair< const typename Tree::NodeType *, int > > ( * )( const Tree & ))( &::genesis::tree::closest_leaf_depth_vector ),
        ( boost::python::arg("tree") ),
        get_docstring("std::vector< std::pair< const typename Tree::NodeType *, int > > ::genesis::tree::closest_leaf_depth_vector (const Tree & tree)")
    );

    boost::python::def(
        "edge_path_length_matrix",
        ( utils::Matrix< int > ( * )( const Tree & ))( &::genesis::tree::edge_path_length_matrix ),
        ( boost::python::arg("tree") )
    );

    boost::python::def(
        "node_path_length_matrix",
        ( utils::Matrix< int > ( * )( const Tree & ))( &::genesis::tree::node_path_length_matrix ),
        ( boost::python::arg("tree") ),
        get_docstring("utils::Matrix< int > ::genesis::tree::node_path_length_matrix (const Tree & tree)")
    );
}
