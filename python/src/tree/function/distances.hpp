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
        "edge_depth_vector",
        ( std::vector< int > ( * )( const Tree &, const typename Tree::EdgeType * ))( &::genesis::tree::edge_depth_vector ),
        ( boost::python::arg("tree"), boost::python::arg("edge")=(const typename Tree::EdgeType *)(nullptr) )
    );

    boost::python::def(
        "node_depth_vector",
        ( std::vector< int > ( * )( const Tree &, const typename Tree::NodeType * ))( &::genesis::tree::node_depth_vector ),
        ( boost::python::arg("tree"), boost::python::arg("node")=(const typename Tree::NodeType *)(nullptr) ),
        get_docstring("std::vector< int > ::genesis::tree::node_depth_vector (const Tree & tree, const typename Tree::NodeType * node=nullptr)")
    );

    boost::python::def(
        "closest_leaf_depth_vector",
        ( std::vector< std::pair< const typename Tree::NodeType *, int > > ( * )( const Tree & ))( &::genesis::tree::closest_leaf_depth_vector ),
        ( boost::python::arg("tree") ),
        get_docstring("std::vector< std::pair< const typename Tree::NodeType *, int > > ::genesis::tree::closest_leaf_depth_vector (const Tree & tree)")
    );

    boost::python::def(
        "edge_depth_matrix",
        ( utils::Matrix< int > ( * )( const Tree & ))( &::genesis::tree::edge_depth_matrix ),
        ( boost::python::arg("tree") )
    );

    boost::python::def(
        "node_depth_matrix",
        ( utils::Matrix< int > ( * )( const Tree & ))( &::genesis::tree::node_depth_matrix ),
        ( boost::python::arg("tree") ),
        get_docstring("utils::Matrix< int > ::genesis::tree::node_depth_matrix (const Tree & tree)")
    );
}

