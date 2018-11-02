/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_common_tree_functions_export, ::genesis::tree, scope )
{

    scope.def(
        "average_branch_length_tree",
        ( Tree ( * )( TreeSet const & ))( &::genesis::tree::average_branch_length_tree ),
            pybind11::arg("tset"),
        get_docstring("Tree ::genesis::tree::average_branch_length_tree (TreeSet const & tset)")
    );

    scope.def(
        "find_node",
        ( TreeNode * ( * )( Tree &, std::string const &, bool ))( &::genesis::tree::find_node ),
            pybind11::arg("tree"),
            pybind11::arg("name"),
            pybind11::arg("replace_underscores")=(bool)(false),
        get_docstring("TreeNode * ::genesis::tree::find_node (Tree & tree, std::string const & name, bool replace_underscores=false)")
    );

    scope.def(
        "find_node",
        ( TreeNode const * ( * )( Tree const &, std::string const &, bool ))( &::genesis::tree::find_node ),
            pybind11::arg("tree"),
            pybind11::arg("name"),
            pybind11::arg("replace_underscores")=(bool)(false),
        get_docstring("TreeNode const * ::genesis::tree::find_node (Tree const & tree, std::string const & name, bool replace_underscores=false)")
    );

    scope.def(
        "diameter",
        ( double ( * )( Tree const & ))( &::genesis::tree::diameter ),
            pybind11::arg("tree"),
        get_docstring("double ::genesis::tree::diameter (Tree const & tree)")
    );

    scope.def(
        "height",
        ( double ( * )( Tree const & ))( &::genesis::tree::height ),
            pybind11::arg("tree"),
        get_docstring("double ::genesis::tree::height (Tree const & tree)")
    );

    scope.def(
        "length",
        ( double ( * )( Tree const & ))( &::genesis::tree::length ),
            pybind11::arg("tree"),
        get_docstring("double ::genesis::tree::length (Tree const & tree)")
    );

    scope.def(
        "node_names",
        ( std::unordered_set< std::string > ( * )( Tree const &, bool ))( &::genesis::tree::node_names ),
            pybind11::arg("tree"),
            pybind11::arg("leaves_only")=(bool)(false),
        get_docstring("std::unordered_set< std::string > ::genesis::tree::node_names (Tree const & tree, bool leaves_only=false)")
    );

    scope.def(
        "node_names",
        ( std::unordered_set< std::string > ( * )( TreeSet const &, bool ))( &::genesis::tree::node_names ),
            pybind11::arg("tree_set"),
            pybind11::arg("leaves_only")=(bool)(false),
        get_docstring("std::unordered_set< std::string > ::genesis::tree::node_names (TreeSet const & tree_set, bool leaves_only=false)")
    );

    scope.def(
        "branch_lengths",
        ( std::vector< double > ( * )( Tree const & ))( &::genesis::tree::branch_lengths ),
            pybind11::arg("tree"),
        get_docstring("std::vector< double > ::genesis::tree::branch_lengths (Tree const & tree)")
    );

    scope.def(
        "node_names_sorted",
        ( utils::SortedVector< std::string > ( * )( Tree const &, bool ))( &::genesis::tree::node_names_sorted ),
            pybind11::arg("tree"),
            pybind11::arg("leaves_only")=(bool)(false),
        get_docstring("utils::SortedVector< std::string > ::genesis::tree::node_names_sorted (Tree const & tree, bool leaves_only=false)")
    );

    scope.def(
        "node_names_sorted",
        ( utils::SortedVector< std::string > ( * )( TreeSet const &, bool ))( &::genesis::tree::node_names_sorted ),
            pybind11::arg("tree_set"),
            pybind11::arg("leaves_only")=(bool)(false),
        get_docstring("utils::SortedVector< std::string > ::genesis::tree::node_names_sorted (TreeSet const & tree_set, bool leaves_only=false)")
    );

    scope.def(
        "scale_all_branch_lengths",
        ( void ( * )( Tree &, double ))( &::genesis::tree::scale_all_branch_lengths ),
            pybind11::arg("tree"),
            pybind11::arg("factor")=(double)(1.0),
        get_docstring("void ::genesis::tree::scale_all_branch_lengths (Tree & tree, double factor=1.0)")
    );

    scope.def(
        "set_all_branch_lengths",
        ( void ( * )( Tree &, double ))( &::genesis::tree::set_all_branch_lengths ),
            pybind11::arg("tree"),
            pybind11::arg("length")=(double)(1.0),
        get_docstring("void ::genesis::tree::set_all_branch_lengths (Tree & tree, double length=1.0)")
    );
}
