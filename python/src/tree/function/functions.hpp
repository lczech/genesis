/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

template<class Tree>
void python_export_function_tree_function_functions_Tree ()
{
    boost::python::def(
        "is_bifurcating",
        ( bool ( * )( Tree const & ))( &::genesis::tree::is_bifurcating ),
        ( boost::python::arg("tree") ),
        get_docstring("bool ::genesis::tree::is_bifurcating (Tree const & tree)")
    );

    boost::python::def(
        "max_rank",
        ( int ( * )( Tree const & ))( &::genesis::tree::max_rank ),
        ( boost::python::arg("tree") ),
        get_docstring("int ::genesis::tree::max_rank (Tree const & tree)")
    );

    boost::python::def(
        "inner_nodes_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::inner_nodes_count ),
        ( boost::python::arg("tree") ),
        get_docstring("size_t ::genesis::tree::inner_nodes_count (Tree const & tree)")
    );

    boost::python::def(
        "leaf_nodes_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::leaf_nodes_count ),
        ( boost::python::arg("tree") ),
        get_docstring("size_t ::genesis::tree::leaf_nodes_count (Tree const & tree)")
    );
}

