/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_function_distances_export, ::genesis::tree, scope )
{

    scope.def(
        "edge_path_length_vector",
        ( std::vector< size_t > ( * )( Tree const &, TreeEdge const & ))( &::genesis::tree::edge_path_length_vector ),
            pybind11::arg("tree"),
            pybind11::arg("edge")
    );

    scope.def(
        "node_path_length_vector",
        ( std::vector< size_t > ( * )( Tree const & ))( &::genesis::tree::node_path_length_vector ),
            pybind11::arg("tree"),
        get_docstring("std::vector< size_t > ::genesis::tree::node_path_length_vector (Tree const & tree)")
    );

    scope.def(
        "node_path_length_vector",
        ( std::vector< size_t > ( * )( Tree const &, TreeNode const & ))( &::genesis::tree::node_path_length_vector ),
            pybind11::arg("tree"),
            pybind11::arg("node"),
        get_docstring("std::vector< size_t > ::genesis::tree::node_path_length_vector (Tree const & tree, TreeNode const & node)")
    );

    scope.def(
        "closest_leaf_depth_vector",
        ( std::vector< std::pair< TreeNode const *, size_t > > ( * )( Tree const & ))( &::genesis::tree::closest_leaf_depth_vector ),
            pybind11::arg("tree"),
        get_docstring("std::vector< std::pair< TreeNode const *, size_t > > ::genesis::tree::closest_leaf_depth_vector (Tree const & tree)")
    );

    scope.def(
        "edge_path_length_matrix",
        ( utils::Matrix< size_t > ( * )( Tree const & ))( &::genesis::tree::edge_path_length_matrix ),
            pybind11::arg("tree")
    );

    scope.def(
        "node_path_length_matrix",
        ( utils::Matrix< size_t > ( * )( Tree const & ))( &::genesis::tree::node_path_length_matrix ),
            pybind11::arg("tree"),
        get_docstring("utils::Matrix< size_t > ::genesis::tree::node_path_length_matrix (Tree const & tree)")
    );
}
