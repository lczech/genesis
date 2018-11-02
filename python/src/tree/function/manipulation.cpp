/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_function_manipulation_export, ::genesis::tree, scope )
{

    scope.def(
        "add_new_leaf_node",
        ( TreeEdge & ( * )( Tree &, TreeEdge & ))( &::genesis::tree::add_new_leaf_node ),
            pybind11::arg("tree"),
            pybind11::arg("target_edge"),
        get_docstring("TreeEdge & ::genesis::tree::add_new_leaf_node (Tree & tree, TreeEdge & target_edge)")
    );

    scope.def(
        "add_new_node",
        ( TreeEdge & ( * )( Tree &, TreeNode & ))( &::genesis::tree::add_new_node ),
            pybind11::arg("tree"),
            pybind11::arg("target_node"),
        get_docstring("TreeEdge & ::genesis::tree::add_new_node (Tree & tree, TreeNode & target_node)")
    );

    scope.def(
        "add_new_node",
        ( TreeNode & ( * )( Tree &, TreeEdge & ))( &::genesis::tree::add_new_node ),
            pybind11::arg("tree"),
            pybind11::arg("target_edge"),
        get_docstring("TreeNode & ::genesis::tree::add_new_node (Tree & tree, TreeEdge & target_edge)")
    );

    scope.def(
        "add_root_node",
        ( TreeNode & ( * )( Tree &, TreeEdge & ))( &::genesis::tree::add_root_node ),
            pybind11::arg("tree"),
            pybind11::arg("target_edge"),
        get_docstring("TreeNode & ::genesis::tree::add_root_node (Tree & tree, TreeEdge & target_edge)")
    );

    scope.def(
        "ladderize",
        ( void ( * )( Tree &, LadderizeOrder ))( &::genesis::tree::ladderize ),
            pybind11::arg("tree"),
            pybind11::arg("order")
    );

    scope.def(
        "reroot",
        ( void ( * )( Tree &, TreeLink & ))( &::genesis::tree::reroot ),
            pybind11::arg("tree"),
            pybind11::arg("at_link"),
        get_docstring("void ::genesis::tree::reroot (Tree & tree, TreeLink & at_link)")
    );

    scope.def(
        "reroot",
        ( void ( * )( Tree &, TreeNode & ))( &::genesis::tree::reroot ),
            pybind11::arg("tree"),
            pybind11::arg("at_node"),
        get_docstring("void ::genesis::tree::reroot (Tree & tree, TreeNode & at_node)")
    );

    scope.def(
        "reroot_at_node",
        ( void ( * )( Tree &, size_t ))( &::genesis::tree::reroot_at_node ),
            pybind11::arg("tree"),
            pybind11::arg("node_index"),
        get_docstring("void ::genesis::tree::reroot_at_node (Tree & tree, size_t node_index)")
    );
}
