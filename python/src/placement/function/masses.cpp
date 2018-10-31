/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_masses_export, ::genesis::placement, scope )
{

    scope.def(
        "total_multiplicity",
        ( double ( * )( Pquery const & ))( &::genesis::placement::total_multiplicity ),
            pybind11::arg("pqry"),
        get_docstring("double ::genesis::placement::total_multiplicity (Pquery const & pqry)")
    );

    scope.def(
        "total_multiplicity",
        ( double ( * )( Sample const & ))( &::genesis::placement::total_multiplicity ),
            pybind11::arg("sample"),
        get_docstring("double ::genesis::placement::total_multiplicity (Sample const & sample)")
    );

    scope.def(
        "total_placement_mass_with_multiplicities",
        ( double ( * )( Sample const & ))( &::genesis::placement::total_placement_mass_with_multiplicities ),
            pybind11::arg("smp"),
        get_docstring("double ::genesis::placement::total_placement_mass_with_multiplicities (Sample const & smp)")
    );

    scope.def(
        "total_placement_mass_without_multiplicities",
        ( double ( * )( Sample const & ))( &::genesis::placement::total_placement_mass_without_multiplicities ),
            pybind11::arg("smp"),
        get_docstring("double ::genesis::placement::total_placement_mass_without_multiplicities (Sample const & smp)")
    );

    scope.def(
        "placement_mass_per_edge_without_multiplicities",
        ( std::vector< double > ( * )( Sample const & ))( &::genesis::placement::placement_mass_per_edge_without_multiplicities ),
            pybind11::arg("sample"),
        get_docstring("std::vector< double > ::genesis::placement::placement_mass_per_edge_without_multiplicities (Sample const & sample)")
    );

    scope.def(
        "placement_mass_per_edges_with_multiplicities",
        ( std::vector< double > ( * )( Sample const & ))( &::genesis::placement::placement_mass_per_edges_with_multiplicities ),
            pybind11::arg("sample"),
        get_docstring("std::vector< double > ::genesis::placement::placement_mass_per_edges_with_multiplicities (Sample const & sample)")
    );

    scope.def(
        "placement_mass_per_edge_without_multiplicities",
        ( utils::Matrix< double > ( * )( SampleSet const & ))( &::genesis::placement::placement_mass_per_edge_without_multiplicities ),
            pybind11::arg("sample_set"),
        get_docstring("utils::Matrix< double > ::genesis::placement::placement_mass_per_edge_without_multiplicities (SampleSet const & sample_set)")
    );

    scope.def(
        "placement_mass_per_edges_with_multiplicities",
        ( utils::Matrix< double > ( * )( SampleSet const & ))( &::genesis::placement::placement_mass_per_edges_with_multiplicities ),
            pybind11::arg("sample_set"),
        get_docstring("utils::Matrix< double > ::genesis::placement::placement_mass_per_edges_with_multiplicities (SampleSet const & sample_set)")
    );
}
