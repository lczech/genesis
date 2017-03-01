/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_function_tree_set_export, ::genesis::tree, scope )
{

    scope.def(
        "find_tree",
        ( Tree * ( * )( TreeSet &, std::string const & ))( &::genesis::tree::find_tree ),
            pybind11::arg("tset"),
            pybind11::arg("name")
    );

    scope.def(
        "average_branch_length_tree",
        ( Tree ( * )( TreeSet const & ))( &::genesis::tree::average_branch_length_tree ),
            pybind11::arg("tset")
    );

    scope.def(
        "find_tree",
        ( Tree const * ( * )( TreeSet const &, std::string const & ))( &::genesis::tree::find_tree ),
            pybind11::arg("tset"),
            pybind11::arg("name")
    );

    scope.def(
        "all_equal",
        ( bool ( * )( TreeSet const &, std::function< bool(TreeNode const &, TreeNode const &)>, std::function< bool(TreeEdge const &, TreeEdge const &)> ))( &::genesis::tree::all_equal ),
            pybind11::arg("tset"),
            pybind11::arg("node_comparator"),
            pybind11::arg("edge_comparator")
    );

    scope.def(
        "all_identical_topology",
        ( bool ( * )( TreeSet const & ))( &::genesis::tree::all_identical_topology ),
            pybind11::arg("tset")
    );
}
