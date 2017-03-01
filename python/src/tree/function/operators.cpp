/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_function_operators_export, ::genesis::tree, scope )
{

    scope.def(
        "convert",
        ( Tree ( * )( Tree const &, std::function< std::unique_ptr< BaseNodeData >(BaseNodeData const &node_data)>, std::function< std::unique_ptr< BaseEdgeData >(BaseEdgeData const &edge_data)> ))( &::genesis::tree::convert ),
            pybind11::arg("source"),
            pybind11::arg("node_data_converter"),
            pybind11::arg("edge_data_converter")
    );

    scope.def(
        "edge_between",
        ( TreeEdge * ( * )( TreeNode &, TreeNode & ))( &::genesis::tree::edge_between ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "edge_between",
        ( TreeEdge const * ( * )( TreeNode const &, TreeNode const & ))( &::genesis::tree::edge_between ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "belongs_to",
        ( bool ( * )( Tree const &, TreeEdge const & ))( &::genesis::tree::belongs_to ),
            pybind11::arg("tree"),
            pybind11::arg("edge")
    );

    scope.def(
        "belongs_to",
        ( bool ( * )( Tree const &, TreeLink const & ))( &::genesis::tree::belongs_to ),
            pybind11::arg("tree"),
            pybind11::arg("link")
    );

    scope.def(
        "belongs_to",
        ( bool ( * )( Tree const &, TreeNode const & ))( &::genesis::tree::belongs_to ),
            pybind11::arg("tree"),
            pybind11::arg("node")
    );

    scope.def(
        "belongs_to",
        ( bool ( * )( TreeEdge const &, Tree const & ))( &::genesis::tree::belongs_to ),
            pybind11::arg("edge"),
            pybind11::arg("tree")
    );

    scope.def(
        "belongs_to",
        ( bool ( * )( TreeLink const &, Tree const & ))( &::genesis::tree::belongs_to ),
            pybind11::arg("link"),
            pybind11::arg("tree")
    );

    scope.def(
        "belongs_to",
        ( bool ( * )( TreeNode const &, Tree const & ))( &::genesis::tree::belongs_to ),
            pybind11::arg("node"),
            pybind11::arg("tree")
    );

    scope.def(
        "equal",
        ( bool ( * )( Tree const &, Tree const &, std::function< bool(TreeNode const &, TreeNode const &) >, std::function< bool(TreeEdge const &, TreeEdge const &) > ))( &::genesis::tree::equal ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
            pybind11::arg("node_comparator"),
            pybind11::arg("edge_comparator")
    );

    scope.def(
        "identical_topology",
        ( bool ( * )( Tree const &, Tree const & ))( &::genesis::tree::identical_topology ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "validate_topology",
        ( bool ( * )( Tree const & ))( &::genesis::tree::validate_topology ),
            pybind11::arg("tree")
    );
}
