/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_default_edge_color_export, ::genesis::tree, scope )
{

    scope.def(
        "edge_color_branch_length_gradient",
        ( std::vector< utils::Color > ( * )( Tree const &, bool ))( &::genesis::tree::edge_color_branch_length_gradient ),
            pybind11::arg("tree"),
            pybind11::arg("zero_based")=(bool)(false)
    );
}
