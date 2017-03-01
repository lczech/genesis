/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::Simulator, scope )
{

    // -------------------------------------------------------------------
    //     Class Simulator
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::Simulator, std::shared_ptr<::genesis::placement::Simulator> > ( scope, "Simulator" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Simulator const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge_distribution",
            ( SimulatorEdgeDistribution & ( ::genesis::placement::Simulator::* )(  ))( &::genesis::placement::Simulator::edge_distribution )
        )
        .def(
            "extra_placement_distribution",
            ( SimulatorExtraPlacementDistribution & ( ::genesis::placement::Simulator::* )(  ))( &::genesis::placement::Simulator::extra_placement_distribution )
        )
        .def(
            "generate",
            ( void ( ::genesis::placement::Simulator::* )( Sample &, size_t ))( &::genesis::placement::Simulator::generate ),
            pybind11::arg("sample"),
            pybind11::arg("n")
        )
        .def(
            "like_weight_ratio_distribution",
            ( SimulatorLikeWeightRatioDistribution & ( ::genesis::placement::Simulator::* )(  ))( &::genesis::placement::Simulator::like_weight_ratio_distribution )
        )
        .def(
            "pendant_length_distribution",
            ( SimulatorPendantLengthDistribution & ( ::genesis::placement::Simulator::* )(  ))( &::genesis::placement::Simulator::pendant_length_distribution )
        )
        .def(
            "proximal_length_distribution",
            ( SimulatorProximalLengthDistribution & ( ::genesis::placement::Simulator::* )(  ))( &::genesis::placement::Simulator::proximal_length_distribution )
        )
    ;
}
