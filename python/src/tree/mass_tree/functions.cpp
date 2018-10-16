/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_mass_tree_functions_export, ::genesis::tree, scope )
{

    scope.def(
        "mass_tree_merge_trees",
        ( MassTree ( * )( MassTree const &, MassTree const &, double const, double const ))( &::genesis::tree::mass_tree_merge_trees ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
            pybind11::arg("scaler_lhs")=(double const)(1.0),
            pybind11::arg("scaler_rhs")=(double const)(1.0),
        get_docstring("MassTree ::genesis::tree::mass_tree_merge_trees (MassTree const & lhs, MassTree const & rhs, double const scaler_lhs=1.0, double const scaler_rhs=1.0)")
    );

    scope.def(
        "mass_tree_all_identical_topology",
        ( bool ( * )( std::vector< MassTree > const & ))( &::genesis::tree::mass_tree_all_identical_topology ),
            pybind11::arg("mass_trees"),
        get_docstring("bool ::genesis::tree::mass_tree_all_identical_topology (std::vector< MassTree > const & mass_trees)")
    );

    scope.def(
        "mass_tree_validate",
        ( bool ( * )( MassTree const &, double ))( &::genesis::tree::mass_tree_validate ),
            pybind11::arg("tree"),
            pybind11::arg("valid_total_mass_difference")=(double)(-1.0),
        get_docstring("bool ::genesis::tree::mass_tree_validate (MassTree const & tree, double valid_total_mass_difference=-1.0)")
    );

    scope.def(
        "mass_tree_binify_masses",
        ( double ( * )( MassTree &, size_t ))( &::genesis::tree::mass_tree_binify_masses ),
            pybind11::arg("tree"),
            pybind11::arg("number_of_bins"),
        get_docstring("double ::genesis::tree::mass_tree_binify_masses (MassTree & tree, size_t number_of_bins)")
    );

    scope.def(
        "mass_tree_center_masses_on_branches",
        ( double ( * )( MassTree & ))( &::genesis::tree::mass_tree_center_masses_on_branches ),
            pybind11::arg("tree"),
        get_docstring("double ::genesis::tree::mass_tree_center_masses_on_branches (MassTree & tree)")
    );

    scope.def(
        "mass_tree_center_masses_on_branches_averaged",
        ( double ( * )( MassTree & ))( &::genesis::tree::mass_tree_center_masses_on_branches_averaged ),
            pybind11::arg("tree"),
        get_docstring("double ::genesis::tree::mass_tree_center_masses_on_branches_averaged (MassTree & tree)")
    );

    scope.def(
        "mass_tree_sum_of_masses",
        ( double ( * )( MassTree const & ))( &::genesis::tree::mass_tree_sum_of_masses ),
            pybind11::arg("tree"),
        get_docstring("double ::genesis::tree::mass_tree_sum_of_masses (MassTree const & tree)")
    );

    scope.def(
        "mass_tree_mass_per_edge",
        ( std::vector< double > ( * )( MassTree const & ))( &::genesis::tree::mass_tree_mass_per_edge ),
            pybind11::arg("tree"),
        get_docstring("std::vector< double > ::genesis::tree::mass_tree_mass_per_edge (MassTree const & tree)")
    );

    scope.def(
        "mass_tree_clear_masses",
        ( void ( * )( MassTree & ))( &::genesis::tree::mass_tree_clear_masses ),
            pybind11::arg("tree"),
        get_docstring("void ::genesis::tree::mass_tree_clear_masses (MassTree & tree)")
    );

    scope.def(
        "mass_tree_merge_trees_inplace",
        ( void ( * )( MassTree &, MassTree const &, double const, double const ))( &::genesis::tree::mass_tree_merge_trees_inplace ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
            pybind11::arg("scaler_lhs")=(double const)(1.0),
            pybind11::arg("scaler_rhs")=(double const)(1.0),
        get_docstring("void ::genesis::tree::mass_tree_merge_trees_inplace (MassTree & lhs, MassTree const & rhs, double const scaler_lhs=1.0, double const scaler_rhs=1.0)")
    );

    scope.def(
        "mass_tree_normalize_masses",
        ( void ( * )( MassTree & ))( &::genesis::tree::mass_tree_normalize_masses ),
            pybind11::arg("tree"),
        get_docstring("void ::genesis::tree::mass_tree_normalize_masses (MassTree & tree)")
    );

    scope.def(
        "mass_tree_reverse_signs",
        ( void ( * )( MassTree & ))( &::genesis::tree::mass_tree_reverse_signs ),
            pybind11::arg("tree"),
        get_docstring("void ::genesis::tree::mass_tree_reverse_signs (MassTree & tree)")
    );

    scope.def(
        "mass_tree_scale_masses",
        ( void ( * )( MassTree &, double ))( &::genesis::tree::mass_tree_scale_masses ),
            pybind11::arg("tree"),
            pybind11::arg("factor"),
        get_docstring("void ::genesis::tree::mass_tree_scale_masses (MassTree & tree, double factor)")
    );

    scope.def(
        "mass_tree_transform_to_unit_branch_lengths",
        ( void ( * )( MassTree & ))( &::genesis::tree::mass_tree_transform_to_unit_branch_lengths ),
            pybind11::arg("tree"),
        get_docstring("void ::genesis::tree::mass_tree_transform_to_unit_branch_lengths (MassTree & tree)")
    );

    scope.def(
        "mass_trees_make_average_branch_lengths",
        ( void ( * )( std::vector< MassTree > & ))( &::genesis::tree::mass_trees_make_average_branch_lengths ),
            pybind11::arg("mass_trees"),
        get_docstring("void ::genesis::tree::mass_trees_make_average_branch_lengths (std::vector< MassTree > & mass_trees)")
    );
}
