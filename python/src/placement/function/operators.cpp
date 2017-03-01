/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_operators_export, ::genesis::placement, scope )
{

    scope.def(
        "convert_to_placement_tree",
        ( PlacementTree ( * )( tree::DefaultTree const & ))( &::genesis::placement::convert_to_placement_tree ),
            pybind11::arg("source_tree")
    );

    scope.def(
        "compatible_trees",
        ( bool ( * )( PlacementTree const &, PlacementTree const & ))( &::genesis::placement::compatible_trees ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "compatible_trees",
        ( bool ( * )( Sample const &, Sample const & ))( &::genesis::placement::compatible_trees ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "print_tree",
        ( std::string ( * )( Sample const & ))( &::genesis::placement::print_tree ),
            pybind11::arg("smp")
    );
}
