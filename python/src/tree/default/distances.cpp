/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_default_distances_export, ::genesis::tree, scope )
{

    scope.def(
        "deepest_distance",
        ( double ( * )( Tree const & ))( &::genesis::tree::deepest_distance ),
            pybind11::arg("tree")
    );

    scope.def(
        "edge_branch_length_distance_vector",
        ( std::vector< double > ( * )( Tree const &, TreeEdge const * ))( &::genesis::tree::edge_branch_length_distance_vector ),
            pybind11::arg("tree"),
            pybind11::arg("edge")=(TreeEdge const *)(nullptr)
    );

    scope.def(
        "node_branch_length_distance_vector",
        ( std::vector< double > ( * )( Tree const &, TreeNode const * ))( &::genesis::tree::node_branch_length_distance_vector ),
            pybind11::arg("tree"),
            pybind11::arg("node")=(TreeNode const *)(nullptr)
    );

    scope.def(
        "closest_leaf_distance_vector",
        ( std::vector< std::pair< TreeNode const *, double > > ( * )( Tree const & ))( &::genesis::tree::closest_leaf_distance_vector ),
            pybind11::arg("tree")
    );

    scope.def(
        "edge_branch_length_distance_matrix",
        ( utils::Matrix< double > ( * )( Tree const & ))( &::genesis::tree::edge_branch_length_distance_matrix ),
            pybind11::arg("tree")
    );

    scope.def(
        "node_branch_length_distance_matrix",
        ( utils::Matrix< double > ( * )( Tree const & ))( &::genesis::tree::node_branch_length_distance_matrix ),
            pybind11::arg("tree")
    );
}
