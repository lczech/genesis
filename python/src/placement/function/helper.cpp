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
        "has_correct_edge_nums",
        ( bool ( * )( PlacementTree const & ))( &::genesis::placement::has_correct_edge_nums ),
            pybind11::arg("tree")
    );

    scope.def(
        "validate",
        ( bool ( * )( Sample const &, bool, bool ))( &::genesis::placement::validate ),
            pybind11::arg("smp"),
            pybind11::arg("check_values")=(bool)(false),
            pybind11::arg("break_on_values")=(bool)(false)
    );

    scope.def(
        "edge_num_to_edge_map",
        ( std::unordered_map< int, PlacementTreeEdge * > ( * )( PlacementTree const & ))( &::genesis::placement::edge_num_to_edge_map ),
            pybind11::arg("tree")
    );

    scope.def(
        "edge_num_to_edge_map",
        ( std::unordered_map< int, PlacementTreeEdge * > ( * )( Sample const & ))( &::genesis::placement::edge_num_to_edge_map ),
            pybind11::arg("smp")
    );

    scope.def(
        "placements_per_edge",
        ( std::unordered_map< size_t, std::vector< PqueryPlacement const * > > ( * )( Sample const & ))( &::genesis::placement::placements_per_edge ),
            pybind11::arg("smp")
    );

    scope.def(
        "placements_per_edge",
        ( std::vector< PqueryPlacement const * > ( * )( Sample const &, PlacementTreeEdge const & ))( &::genesis::placement::placements_per_edge ),
            pybind11::arg("smp"),
            pybind11::arg("edge")
    );

    scope.def(
        "plain_queries",
        ( std::vector< PqueryPlain > ( * )( Sample const & ))( &::genesis::placement::plain_queries ),
            pybind11::arg("smp")
    );

    scope.def(
        "placement_weight_per_edge",
        ( std::vector< double > ( * )( Sample const & ))( &::genesis::placement::placement_weight_per_edge ),
            pybind11::arg("sample")
    );

    scope.def(
        "placement_count_per_edge",
        ( std::vector< size_t > ( * )( Sample const & ))( &::genesis::placement::placement_count_per_edge ),
            pybind11::arg("sample")
    );

    scope.def(
        "reset_edge_nums",
        ( void ( * )( PlacementTree & ))( &::genesis::placement::reset_edge_nums ),
            pybind11::arg("tree")
    );
}
