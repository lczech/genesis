/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_helper_export, ::genesis::placement, scope )
{

    scope.def(
        "has_consecutive_edge_nums",
        ( bool ( * )( PlacementTree const & ))( &::genesis::placement::has_consecutive_edge_nums ),
            pybind11::arg("tree"),
        get_docstring("bool ::genesis::placement::has_consecutive_edge_nums (PlacementTree const & tree)")
    );

    scope.def(
        "has_correct_edge_nums",
        ( bool ( * )( PlacementTree const & ))( &::genesis::placement::has_correct_edge_nums ),
            pybind11::arg("tree"),
        get_docstring("bool ::genesis::placement::has_correct_edge_nums (PlacementTree const & tree)")
    );

    scope.def(
        "validate",
        ( bool ( * )( Sample const &, bool, bool ))( &::genesis::placement::validate ),
            pybind11::arg("smp"),
            pybind11::arg("check_values")=(bool)(false),
            pybind11::arg("break_on_values")=(bool)(false),
        get_docstring("bool ::genesis::placement::validate (Sample const & smp, bool check_values=false, bool break_on_values=false)")
    );

    scope.def(
        "edge_num_to_edge_map",
        ( std::unordered_map< int, PlacementTreeEdge * > ( * )( PlacementTree const & ))( &::genesis::placement::edge_num_to_edge_map ),
            pybind11::arg("tree"),
        get_docstring("std::unordered_map< int, PlacementTreeEdge * > ::genesis::placement::edge_num_to_edge_map (PlacementTree const & tree)")
    );

    scope.def(
        "edge_num_to_edge_map",
        ( std::unordered_map< int, PlacementTreeEdge * > ( * )( Sample const & ))( &::genesis::placement::edge_num_to_edge_map ),
            pybind11::arg("smp"),
        get_docstring("std::unordered_map< int, PlacementTreeEdge * > ::genesis::placement::edge_num_to_edge_map (Sample const & smp)")
    );

    scope.def(
        "placements_per_edge",
        ( std::vector< PqueryPlacement const * > ( * )( Sample const &, PlacementTreeEdge const & ))( &::genesis::placement::placements_per_edge ),
            pybind11::arg("smp"),
            pybind11::arg("edge"),
        get_docstring("std::vector< PqueryPlacement const * > ::genesis::placement::placements_per_edge (Sample const & smp, PlacementTreeEdge const & edge)")
    );

    scope.def(
        "plain_queries",
        ( std::vector< PqueryPlain > ( * )( Sample const & ))( &::genesis::placement::plain_queries ),
            pybind11::arg("smp"),
        get_docstring("std::vector< PqueryPlain > ::genesis::placement::plain_queries (Sample const & smp)")
    );

    scope.def(
        "placement_count_per_edge",
        ( std::vector< size_t > ( * )( Sample const & ))( &::genesis::placement::placement_count_per_edge ),
            pybind11::arg("sample"),
        get_docstring("std::vector< size_t > ::genesis::placement::placement_count_per_edge (Sample const & sample)")
    );

    scope.def(
        "pqueries_per_edge",
        ( std::vector< std::vector< Pquery const * > > ( * )( Sample const &, bool ))( &::genesis::placement::pqueries_per_edge ),
            pybind11::arg("sample"),
            pybind11::arg("only_max_lwr_placements")=(bool)(false),
        get_docstring("std::vector< std::vector< Pquery const * > > ::genesis::placement::pqueries_per_edge (Sample const & sample, bool only_max_lwr_placements=false)")
    );

    scope.def(
        "placements_per_edge",
        ( std::vector< std::vector< PqueryPlacement const * > > ( * )( Sample const &, bool ))( &::genesis::placement::placements_per_edge ),
            pybind11::arg("smp"),
            pybind11::arg("only_max_lwr_placements")=(bool)(false),
        get_docstring("std::vector< std::vector< PqueryPlacement const * > > ::genesis::placement::placements_per_edge (Sample const & smp, bool only_max_lwr_placements=false)")
    );

    scope.def(
        "placement_count_per_edge",
        ( utils::Matrix< size_t > ( * )( SampleSet const & ))( &::genesis::placement::placement_count_per_edge ),
            pybind11::arg("sample_set")
    );

    scope.def(
        "rectify_values",
        ( void ( * )( Sample & ))( &::genesis::placement::rectify_values ),
            pybind11::arg("sample"),
        get_docstring("void ::genesis::placement::rectify_values (Sample & sample)")
    );

    scope.def(
        "rectify_values",
        ( void ( * )( SampleSet & ))( &::genesis::placement::rectify_values ),
            pybind11::arg("sset"),
        get_docstring("void ::genesis::placement::rectify_values (SampleSet & sset)")
    );

    scope.def(
        "reset_edge_nums",
        ( void ( * )( PlacementTree & ))( &::genesis::placement::reset_edge_nums ),
            pybind11::arg("tree"),
        get_docstring("void ::genesis::placement::reset_edge_nums (PlacementTree & tree)")
    );
}
