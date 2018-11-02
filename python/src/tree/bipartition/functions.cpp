/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_bipartition_functions_export, ::genesis::tree, scope )
{

    scope.def(
        "find_smallest_subtree",
        ( Bipartition ( * )( Tree const &, std::vector< Bipartition > const &, std::vector< TreeNode const * > ))( &::genesis::tree::find_smallest_subtree ),
            pybind11::arg("tree"),
            pybind11::arg("bipartitions"),
            pybind11::arg("nodes"),
        get_docstring("Bipartition ::genesis::tree::find_smallest_subtree (Tree const & tree, std::vector< Bipartition > const & bipartitions, std::vector< TreeNode const * > nodes)")
    );

    scope.def(
        "bipartition_set",
        ( std::vector< Bipartition > ( * )( Tree const & ))( &::genesis::tree::bipartition_set ),
            pybind11::arg("tree")
    );

    scope.def(
        "find_monophyletic_subtree_edges",
        ( std::vector< size_t > ( * )( Tree const &, std::vector< Bipartition > const &, std::vector< TreeNode const * > ))( &::genesis::tree::find_monophyletic_subtree_edges ),
            pybind11::arg("tree"),
            pybind11::arg("bipartitions"),
            pybind11::arg("nodes"),
        get_docstring("std::vector< size_t > ::genesis::tree::find_monophyletic_subtree_edges (Tree const & tree, std::vector< Bipartition > const & bipartitions, std::vector< TreeNode const * > nodes)")
    );

    scope.def(
        "get_clade_edges",
        ( std::vector< size_t > ( * )( Tree const &, std::vector< std::string > const & ))( &::genesis::tree::get_clade_edges ),
            pybind11::arg("tree"),
            pybind11::arg("node_names")
    );

    scope.def(
        "get_clade_edges",
        ( std::vector< size_t > ( * )( Tree const &, std::vector< tree::TreeNode const * > const & ))( &::genesis::tree::get_clade_edges ),
            pybind11::arg("tree"),
            pybind11::arg("nodes")
    );

    scope.def(
        "get_subtree_edges",
        ( std::vector< size_t > ( * )( TreeLink const & ))( &::genesis::tree::get_subtree_edges ),
            pybind11::arg("subtree")
    );

    scope.def(
        "node_to_leaf_map",
        ( std::vector< size_t > ( * )( Tree const & ))( &::genesis::tree::node_to_leaf_map ),
            pybind11::arg("tree")
    );

    scope.def(
        "leaf_node_bitvector",
        ( utils::Bitvector ( * )( Tree const &, std::vector< TreeNode const * > ))( &::genesis::tree::leaf_node_bitvector ),
            pybind11::arg("tree"),
            pybind11::arg("leaf_nodes"),
        get_docstring("utils::Bitvector ::genesis::tree::leaf_node_bitvector (Tree const & tree, std::vector< TreeNode const * > leaf_nodes)")
    );
}
