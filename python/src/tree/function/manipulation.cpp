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
