/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_common_tree_operators_export, ::genesis::tree, scope )
{

    scope.def(
        "convert_to_common_tree",
        ( CommonTree ( * )( Tree const & ))( &::genesis::tree::convert_to_common_tree ),
            pybind11::arg("source_tree"),
        get_docstring("CommonTree ::genesis::tree::convert_to_common_tree (Tree const & source_tree)")
    );

    scope.def(
        "equal_common_trees",
        ( bool ( * )( Tree const &, Tree const &, bool, bool ))( &::genesis::tree::equal_common_trees ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
            pybind11::arg("compare_node_names")=(bool)(true),
            pybind11::arg("compare_branch_lengths")=(bool)(true),
        get_docstring("bool ::genesis::tree::equal_common_trees (Tree const & lhs, Tree const & rhs, bool compare_node_names=true, bool compare_branch_lengths=true)")
    );
}
