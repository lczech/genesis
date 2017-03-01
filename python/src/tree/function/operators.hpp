/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

template<class NodeDataType, class EdgeDataType>
void python_export_function_tree_function_operators_NodeDataType_EdgeDataType ()
{
    scope.def(
        "tree_data_is",
        ( bool ( * )( Tree const & ))( &::genesis::tree::tree_data_is ),
            pybind11::arg("tree"),
        get_docstring("bool ::genesis::tree::tree_data_is (Tree const & tree)")
    );

    scope.def(
        "tree_data_is_derived_from",
        ( bool ( * )( Tree const & ))( &::genesis::tree::tree_data_is_derived_from ),
            pybind11::arg("tree"),
        get_docstring("bool ::genesis::tree::tree_data_is_derived_from (Tree const & tree)")
    );
}
