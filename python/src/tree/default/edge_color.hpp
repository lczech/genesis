/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

template<class TreeType>
void python_export_function_tree_default_edge_color_TreeType ()
{
    boost::python::def(
        "edge_color_branch_length_gradient",
        ( std::vector< utils::Color > ( * )( TreeType const &, bool ))( &::genesis::tree::edge_color_branch_length_gradient ),
        ( boost::python::arg("tree"), boost::python::arg("zero_based")=(bool)(false) )
    );
}

