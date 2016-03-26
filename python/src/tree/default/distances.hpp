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
void python_export_function_tree_default_distances_Tree ()
{
    boost::python::def(
        "deepest_distance",
        ( double ( * )( const Tree & ))( &::genesis::tree::deepest_distance ),
        ( boost::python::arg("tree") ),
        get_docstring("double ::genesis::tree::deepest_distance (const Tree & tree)")
    );

    boost::python::def(
        "depth",
        ( double ( * )( const Tree & ))( &::genesis::tree::depth ),
        ( boost::python::arg("tree") )
    );

    boost::python::def(
        "height",
        ( double ( * )( const Tree & ))( &::genesis::tree::height ),
        ( boost::python::arg("tree") ),
        get_docstring("double ::genesis::tree::height (const Tree & tree)")
    );

    boost::python::def(
        "length",
        ( double ( * )( const Tree & ))( &::genesis::tree::length ),
        ( boost::python::arg("tree") ),
        get_docstring("double ::genesis::tree::length (const Tree & tree)")
    );

    boost::python::def(
        "edge_distance_vector",
        ( std::vector< double > ( * )( const Tree &, const typename Tree::EdgeType * ))( &::genesis::tree::edge_distance_vector ),
        ( boost::python::arg("tree"), boost::python::arg("edge")=(const typename Tree::EdgeType *)(nullptr) )
    );

    boost::python::def(
        "node_distance_vector",
        ( std::vector< double > ( * )( const Tree &, const typename Tree::NodeType * ))( &::genesis::tree::node_distance_vector ),
        ( boost::python::arg("tree"), boost::python::arg("node")=(const typename Tree::NodeType *)(nullptr) ),
        get_docstring("std::vector< double > ::genesis::tree::node_distance_vector (const Tree & tree, const typename Tree::NodeType * node=nullptr)")
    );

    boost::python::def(
        "closest_leaf_distance_vector",
        ( std::vector< std::pair< const typename Tree::NodeType *, double > > ( * )( const Tree & ))( &::genesis::tree::closest_leaf_distance_vector ),
        ( boost::python::arg("tree") ),
        get_docstring("std::vector< std::pair< const typename Tree::NodeType *, double > > ::genesis::tree::closest_leaf_distance_vector (const Tree & tree)")
    );

    boost::python::def(
        "edge_distance_matrix",
        ( utils::Matrix< double > ( * )( const Tree & ))( &::genesis::tree::edge_distance_matrix ),
        ( boost::python::arg("tree") )
    );

    boost::python::def(
        "node_distance_matrix",
        ( utils::Matrix< double > ( * )( const Tree & ))( &::genesis::tree::node_distance_matrix ),
        ( boost::python::arg("tree") ),
        get_docstring("utils::Matrix< double > ::genesis::tree::node_distance_matrix (const Tree & tree)")
    );
}

