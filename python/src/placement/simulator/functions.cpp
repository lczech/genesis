/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_simulator_functions_export, ::genesis::placement, scope )
{

    scope.def(
        "set_random_subtree_weights",
        ( size_t ( * )( Sample const &, SimulatorEdgeDistribution & ))( &::genesis::placement::set_random_subtree_weights ),
            pybind11::arg("sample"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "learn_like_weight_ratio_distribution",
        ( void ( * )( Sample const &, SimulatorLikeWeightRatioDistribution &, size_t ))( &::genesis::placement::learn_like_weight_ratio_distribution ),
            pybind11::arg("sample"),
            pybind11::arg("lwr_distib"),
            pybind11::arg("number_of_intervals")=(size_t)(10)
    );

    scope.def(
        "learn_per_edge_weights",
        ( void ( * )( Sample const &, SimulatorEdgeDistribution & ))( &::genesis::placement::learn_per_edge_weights ),
            pybind11::arg("sample"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "learn_placement_number_weights",
        ( void ( * )( Sample const &, SimulatorExtraPlacementDistribution & ))( &::genesis::placement::learn_placement_number_weights ),
            pybind11::arg("sample"),
            pybind11::arg("p_distib")
    );

    scope.def(
        "learn_placement_path_length_weights",
        ( void ( * )( Sample const &, SimulatorExtraPlacementDistribution & ))( &::genesis::placement::learn_placement_path_length_weights ),
            pybind11::arg("sample"),
            pybind11::arg("p_distib")
    );

    scope.def(
        "set_depths_distributed_weights",
        ( void ( * )( Sample const &, SimulatorEdgeDistribution & ))( &::genesis::placement::set_depths_distributed_weights ),
            pybind11::arg("sample"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "set_depths_distributed_weights",
        ( void ( * )( Sample const &, std::vector< double > const &, SimulatorEdgeDistribution & ))( &::genesis::placement::set_depths_distributed_weights ),
            pybind11::arg("sample"),
            pybind11::arg("depth_weights"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "set_random_edges",
        ( void ( * )( Sample const &, SimulatorEdgeDistribution & ))( &::genesis::placement::set_random_edges ),
            pybind11::arg("sample"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "set_random_edges",
        ( void ( * )( size_t, SimulatorEdgeDistribution & ))( &::genesis::placement::set_random_edges ),
            pybind11::arg("edge_count"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "set_random_weights",
        ( void ( * )( Sample const &, SimulatorEdgeDistribution & ))( &::genesis::placement::set_random_weights ),
            pybind11::arg("sample"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "set_random_weights",
        ( void ( * )( size_t, SimulatorEdgeDistribution & ))( &::genesis::placement::set_random_weights ),
            pybind11::arg("edge_count"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "set_subtree_weights",
        ( void ( * )( Sample const &, size_t, SimulatorEdgeDistribution & ))( &::genesis::placement::set_subtree_weights ),
            pybind11::arg("sample"),
            pybind11::arg("link_index"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "set_uniform_weights",
        ( void ( * )( Sample const &, SimulatorEdgeDistribution & ))( &::genesis::placement::set_uniform_weights ),
            pybind11::arg("sample"),
            pybind11::arg("edge_distrib")
    );

    scope.def(
        "set_uniform_weights",
        ( void ( * )( size_t, SimulatorEdgeDistribution & ))( &::genesis::placement::set_uniform_weights ),
            pybind11::arg("edge_count"),
            pybind11::arg("edge_distrib")
    );
}
