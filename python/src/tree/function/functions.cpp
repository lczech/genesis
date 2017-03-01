/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_function_functions_export, ::genesis::tree, scope )
{

    scope.def(
        "lowest_common_ancestor",
        ( TreeNode & ( * )( TreeNode &, TreeNode & ))( &::genesis::tree::lowest_common_ancestor ),
            pybind11::arg("node_a"),
            pybind11::arg("node_b")
    );

    scope.def(
        "lowest_common_ancestor",
        ( TreeNode const & ( * )( TreeNode const &, TreeNode const & ))( &::genesis::tree::lowest_common_ancestor ),
            pybind11::arg("node_a"),
            pybind11::arg("node_b")
    );

    scope.def(
        "is_bifurcating",
        ( bool ( * )( Tree const & ))( &::genesis::tree::is_bifurcating ),
            pybind11::arg("tree")
    );

    scope.def(
        "max_rank",
        ( int ( * )( Tree const & ))( &::genesis::tree::max_rank ),
            pybind11::arg("tree")
    );

    scope.def(
        "inner_node_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::inner_node_count ),
            pybind11::arg("tree")
    );

    scope.def(
        "leaf_node_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::leaf_node_count ),
            pybind11::arg("tree")
    );

    scope.def(
        "subtree_max_path_height",
        ( size_t ( * )( Tree const &, TreeLink const & ))( &::genesis::tree::subtree_max_path_height ),
            pybind11::arg("tree"),
            pybind11::arg("link")
    );

    scope.def(
        "subtree_size",
        ( size_t ( * )( Tree const &, TreeLink const & ))( &::genesis::tree::subtree_size ),
            pybind11::arg("tree"),
            pybind11::arg("link")
    );

    scope.def(
        "path_to_root",
        ( std::vector< TreeLink const * > ( * )( TreeNode const & ))( &::genesis::tree::path_to_root ),
            pybind11::arg("node")
    );

    scope.def(
        "subtree_max_path_heights",
        ( std::vector< size_t > ( * )( Tree const & ))( &::genesis::tree::subtree_max_path_heights ),
            pybind11::arg("tree")
    );

    scope.def(
        "subtree_max_path_heights",
        ( std::vector< size_t > ( * )( Tree const &, TreeNode const & ))( &::genesis::tree::subtree_max_path_heights ),
            pybind11::arg("tree"),
            pybind11::arg("node")
    );

    scope.def(
        "subtree_sizes",
        ( std::vector< size_t > ( * )( Tree const & ))( &::genesis::tree::subtree_sizes ),
            pybind11::arg("tree")
    );

    scope.def(
        "subtree_sizes",
        ( std::vector< size_t > ( * )( Tree const &, TreeNode const & ))( &::genesis::tree::subtree_sizes ),
            pybind11::arg("tree"),
            pybind11::arg("node")
    );
}
