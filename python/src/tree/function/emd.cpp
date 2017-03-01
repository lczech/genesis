/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_function_emd_export, ::genesis::tree, scope )
{

    scope.def(
        "validate_emd_tree",
        ( bool ( * )( EmdTree const &, double ))( &::genesis::tree::validate_emd_tree ),
            pybind11::arg("tree"),
            pybind11::arg("valid_total_mass_difference")=(double)(0.00001)
    );

    scope.def(
        "center_masses_on_branches",
        ( double ( * )( EmdTree & ))( &::genesis::tree::center_masses_on_branches ),
            pybind11::arg("tree")
    );

    scope.def(
        "earth_movers_distance",
        ( double ( * )( EmdTree const & ))( &::genesis::tree::earth_movers_distance ),
            pybind11::arg("tree")
    );

    scope.def(
        "sum_of_masses",
        ( double ( * )( EmdTree const & ))( &::genesis::tree::sum_of_masses ),
            pybind11::arg("tree")
    );

    scope.def(
        "transform_to_unit_branch_lengths",
        ( void ( * )( EmdTree & ))( &::genesis::tree::transform_to_unit_branch_lengths ),
            pybind11::arg("tree")
    );
}
