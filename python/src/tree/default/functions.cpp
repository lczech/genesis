/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_default_functions_export, ::genesis::tree, scope )
{

    scope.def(
        "find_node",
        ( TreeNode * ( * )( Tree &, std::string const &, bool ))( &::genesis::tree::find_node ),
            pybind11::arg("tree"),
            pybind11::arg("name"),
            pybind11::arg("replace_underscores")=(bool)(false)
    );

    scope.def(
        "find_node",
        ( TreeNode const * ( * )( Tree const &, std::string const &, bool ))( &::genesis::tree::find_node ),
            pybind11::arg("tree"),
            pybind11::arg("name"),
            pybind11::arg("replace_underscores")=(bool)(false)
    );

    scope.def(
        "diameter",
        ( double ( * )( Tree const & ))( &::genesis::tree::diameter ),
            pybind11::arg("tree")
    );

    scope.def(
        "height",
        ( double ( * )( Tree const & ))( &::genesis::tree::height ),
            pybind11::arg("tree")
    );

    scope.def(
        "length",
        ( double ( * )( Tree const & ))( &::genesis::tree::length ),
            pybind11::arg("tree")
    );

    scope.def(
        "node_names",
        ( std::unordered_set< std::string > ( * )( Tree const &, bool ))( &::genesis::tree::node_names ),
            pybind11::arg("tree"),
            pybind11::arg("leaves_only")=(bool)(false)
    );

    scope.def(
        "node_names",
        ( std::unordered_set< std::string > ( * )( TreeSet const &, bool ))( &::genesis::tree::node_names ),
            pybind11::arg("tree_set"),
            pybind11::arg("leaves_only")=(bool)(false)
    );

    scope.def(
        "branch_lengths",
        ( std::vector< double > ( * )( Tree const & ))( &::genesis::tree::branch_lengths ),
            pybind11::arg("tree")
    );

    scope.def(
        "node_names_sorted",
        ( utils::SortedVector< std::string > ( * )( Tree const &, bool ))( &::genesis::tree::node_names_sorted ),
            pybind11::arg("tree"),
            pybind11::arg("leaves_only")=(bool)(false)
    );

    scope.def(
        "node_names_sorted",
        ( utils::SortedVector< std::string > ( * )( TreeSet const &, bool ))( &::genesis::tree::node_names_sorted ),
            pybind11::arg("tree_set"),
            pybind11::arg("leaves_only")=(bool)(false)
    );

    scope.def(
        "scale_all_branch_lengths",
        ( void ( * )( Tree &, double ))( &::genesis::tree::scale_all_branch_lengths ),
            pybind11::arg("tree"),
            pybind11::arg("factor")=(double)(1.0)
    );

    scope.def(
        "set_all_branch_lengths",
        ( void ( * )( Tree &, double ))( &::genesis::tree::set_all_branch_lengths ),
            pybind11::arg("tree"),
            pybind11::arg("length")=(double)(1.0)
    );
}
