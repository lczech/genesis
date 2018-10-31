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
        "convert_common_tree_to_placement_tree",
        ( PlacementTree ( * )( tree::CommonTree const & ))( &::genesis::placement::convert_common_tree_to_placement_tree ),
            pybind11::arg("source_tree"),
        get_docstring("PlacementTree ::genesis::placement::convert_common_tree_to_placement_tree (tree::CommonTree const & source_tree)")
    );

    scope.def(
        "compatible_trees",
        ( bool ( * )( PlacementTree const &, PlacementTree const & ))( &::genesis::placement::compatible_trees ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("bool ::genesis::placement::compatible_trees (PlacementTree const & lhs, PlacementTree const & rhs)")
    );

    scope.def(
        "compatible_trees",
        ( bool ( * )( Sample const &, Sample const & ))( &::genesis::placement::compatible_trees ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("bool ::genesis::placement::compatible_trees (Sample const & lhs, Sample const & rhs)")
    );

    scope.def(
        "add_sample_to_mass_tree",
        ( double ( * )( Sample const &, double const, double const, tree::MassTree & ))( &::genesis::placement::add_sample_to_mass_tree ),
            pybind11::arg("smp"),
            pybind11::arg("sign"),
            pybind11::arg("scaler"),
            pybind11::arg("target"),
        get_docstring("double ::genesis::placement::add_sample_to_mass_tree (Sample const & smp, double const sign, double const scaler, tree::MassTree & target)")
    );

    scope.def(
        "convert_sample_set_to_mass_trees",
        ( std::pair< std::vector< tree::MassTree >, std::vector< double > > ( * )( SampleSet const & ))( &::genesis::placement::convert_sample_set_to_mass_trees ),
            pybind11::arg("sample_set"),
        get_docstring("std::pair< std::vector< tree::MassTree >, std::vector< double > > ::genesis::placement::convert_sample_set_to_mass_trees (SampleSet const & sample_set)")
    );

    scope.def(
        "convert_sample_to_mass_tree",
        ( std::pair< tree::MassTree, double > ( * )( Sample const & ))( &::genesis::placement::convert_sample_to_mass_tree ),
            pybind11::arg("sample"),
        get_docstring("std::pair< tree::MassTree, double > ::genesis::placement::convert_sample_to_mass_tree (Sample const & sample)")
    );

    scope.def(
        "print_tree",
        ( std::string ( * )( Sample const & ))( &::genesis::placement::print_tree ),
            pybind11::arg("smp"),
        get_docstring("std::string ::genesis::placement::print_tree (Sample const & smp)")
    );
}
