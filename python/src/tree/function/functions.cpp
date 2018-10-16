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
            pybind11::arg("node_b"),
        get_docstring("TreeNode & ::genesis::tree::lowest_common_ancestor (TreeNode & node_a, TreeNode & node_b)")
    );

    scope.def(
        "lowest_common_ancestor",
        ( TreeNode const & ( * )( TreeNode const &, TreeNode const & ))( &::genesis::tree::lowest_common_ancestor ),
            pybind11::arg("node_a"),
            pybind11::arg("node_b"),
        get_docstring("TreeNode const & ::genesis::tree::lowest_common_ancestor (TreeNode const & node_a, TreeNode const & node_b)")
    );

    scope.def(
        "is_bifurcating",
        ( bool ( * )( Tree const &, bool ))( &::genesis::tree::is_bifurcating ),
            pybind11::arg("tree"),
            pybind11::arg("strict")=(bool)(false),
        get_docstring("bool ::genesis::tree::is_bifurcating (Tree const & tree, bool strict=false)")
    );

    scope.def(
        "is_inner",
        ( bool ( * )( TreeEdge const & ))( &::genesis::tree::is_inner ),
            pybind11::arg("edge"),
        get_docstring("bool ::genesis::tree::is_inner (TreeEdge const & edge)")
    );

    scope.def(
        "is_inner",
        ( bool ( * )( TreeLink const & ))( &::genesis::tree::is_inner ),
            pybind11::arg("link"),
        get_docstring("bool ::genesis::tree::is_inner (TreeLink const & link)")
    );

    scope.def(
        "is_inner",
        ( bool ( * )( TreeNode const & ))( &::genesis::tree::is_inner ),
            pybind11::arg("node"),
        get_docstring("bool ::genesis::tree::is_inner (TreeNode const & node)")
    );

    scope.def(
        "is_leaf",
        ( bool ( * )( TreeEdge const & ))( &::genesis::tree::is_leaf ),
            pybind11::arg("edge"),
        get_docstring("bool ::genesis::tree::is_leaf (TreeEdge const & edge)")
    );

    scope.def(
        "is_leaf",
        ( bool ( * )( TreeLink const & ))( &::genesis::tree::is_leaf ),
            pybind11::arg("link"),
        get_docstring("bool ::genesis::tree::is_leaf (TreeLink const & link)")
    );

    scope.def(
        "is_leaf",
        ( bool ( * )( TreeNode const & ))( &::genesis::tree::is_leaf ),
            pybind11::arg("node"),
        get_docstring("bool ::genesis::tree::is_leaf (TreeNode const & node)")
    );

    scope.def(
        "is_root",
        ( bool ( * )( TreeNode const & ))( &::genesis::tree::is_root ),
            pybind11::arg("node"),
        get_docstring("bool ::genesis::tree::is_root (TreeNode const & node)")
    );

    scope.def(
        "is_rooted",
        ( bool ( * )( Tree const & ))( &::genesis::tree::is_rooted ),
            pybind11::arg("tree"),
        get_docstring("bool ::genesis::tree::is_rooted (Tree const & tree)")
    );

    scope.def(
        "degree",
        ( size_t ( * )( TreeNode const & ))( &::genesis::tree::degree ),
            pybind11::arg("node"),
        get_docstring("size_t ::genesis::tree::degree (TreeNode const & node)")
    );

    scope.def(
        "edge_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::edge_count ),
            pybind11::arg("tree"),
        get_docstring("size_t ::genesis::tree::edge_count (Tree const & tree)")
    );

    scope.def(
        "inner_edge_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::inner_edge_count ),
            pybind11::arg("tree"),
        get_docstring("size_t ::genesis::tree::inner_edge_count (Tree const & tree)")
    );

    scope.def(
        "inner_node_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::inner_node_count ),
            pybind11::arg("tree"),
        get_docstring("size_t ::genesis::tree::inner_node_count (Tree const & tree)")
    );

    scope.def(
        "leaf_edge_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::leaf_edge_count ),
            pybind11::arg("tree"),
        get_docstring("size_t ::genesis::tree::leaf_edge_count (Tree const & tree)")
    );

    scope.def(
        "leaf_node_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::leaf_node_count ),
            pybind11::arg("tree"),
        get_docstring("size_t ::genesis::tree::leaf_node_count (Tree const & tree)")
    );

    scope.def(
        "max_degree",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::max_degree ),
            pybind11::arg("tree"),
        get_docstring("size_t ::genesis::tree::max_degree (Tree const & tree)")
    );

    scope.def(
        "node_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::node_count ),
            pybind11::arg("tree"),
        get_docstring("size_t ::genesis::tree::node_count (Tree const & tree)")
    );

    scope.def(
        "subtree_max_path_height",
        ( size_t ( * )( Tree const &, TreeLink const & ))( &::genesis::tree::subtree_max_path_height ),
            pybind11::arg("tree"),
            pybind11::arg("link"),
        get_docstring("size_t ::genesis::tree::subtree_max_path_height (Tree const & tree, TreeLink const & link)")
    );

    scope.def(
        "subtree_size",
        ( size_t ( * )( Tree const &, TreeLink const & ))( &::genesis::tree::subtree_size ),
            pybind11::arg("tree"),
            pybind11::arg("link"),
        get_docstring("size_t ::genesis::tree::subtree_size (Tree const & tree, TreeLink const & link)")
    );

    scope.def(
        "path_to_root",
        ( std::vector< TreeLink const * > ( * )( TreeNode const & ))( &::genesis::tree::path_to_root ),
            pybind11::arg("node"),
        get_docstring("std::vector< TreeLink const * > ::genesis::tree::path_to_root (TreeNode const & node)")
    );

    scope.def(
        "inner_edge_indices",
        ( std::vector< size_t > ( * )( Tree const & ))( &::genesis::tree::inner_edge_indices ),
            pybind11::arg("tree"),
        get_docstring("std::vector< size_t > ::genesis::tree::inner_edge_indices (Tree const & tree)")
    );

    scope.def(
        "inner_node_indices",
        ( std::vector< size_t > ( * )( Tree const & ))( &::genesis::tree::inner_node_indices ),
            pybind11::arg("tree"),
        get_docstring("std::vector< size_t > ::genesis::tree::inner_node_indices (Tree const & tree)")
    );

    scope.def(
        "leaf_edge_indices",
        ( std::vector< size_t > ( * )( Tree const & ))( &::genesis::tree::leaf_edge_indices ),
            pybind11::arg("tree"),
        get_docstring("std::vector< size_t > ::genesis::tree::leaf_edge_indices (Tree const & tree)")
    );

    scope.def(
        "leaf_node_indices",
        ( std::vector< size_t > ( * )( Tree const & ))( &::genesis::tree::leaf_node_indices ),
            pybind11::arg("tree"),
        get_docstring("std::vector< size_t > ::genesis::tree::leaf_node_indices (Tree const & tree)")
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
            pybind11::arg("tree"),
        get_docstring("std::vector< size_t > ::genesis::tree::subtree_sizes (Tree const & tree)")
    );

    scope.def(
        "subtree_sizes",
        ( std::vector< size_t > ( * )( Tree const &, TreeNode const & ))( &::genesis::tree::subtree_sizes ),
            pybind11::arg("tree"),
            pybind11::arg("node"),
        get_docstring("std::vector< size_t > ::genesis::tree::subtree_sizes (Tree const & tree, TreeNode const & node)")
    );

    scope.def(
        "edge_sides",
        ( utils::Matrix< signed char > ( * )( Tree const & ))( &::genesis::tree::edge_sides ),
            pybind11::arg("tree"),
        get_docstring("utils::Matrix< signed char > ::genesis::tree::edge_sides (Tree const & tree)")
    );

    scope.def(
        "node_root_direction_matrix",
        ( utils::Matrix< signed char > ( * )( Tree const & ))( &::genesis::tree::node_root_direction_matrix ),
            pybind11::arg("tree"),
        get_docstring("utils::Matrix< signed char > ::genesis::tree::node_root_direction_matrix (Tree const & tree)")
    );

    scope.def(
        "lowest_common_ancestors",
        ( utils::Matrix< size_t > ( * )( Tree const & ))( &::genesis::tree::lowest_common_ancestors ),
            pybind11::arg("tree"),
        get_docstring("utils::Matrix< size_t > ::genesis::tree::lowest_common_ancestors (Tree const & tree)")
    );
}
