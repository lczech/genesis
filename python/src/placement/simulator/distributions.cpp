/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::SimulatorExtraPlacementDistribution, scope )
{

    // -------------------------------------------------------------------
    //     Class SimulatorExtraPlacementDistribution
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::SimulatorExtraPlacementDistribution, std::shared_ptr<::genesis::placement::SimulatorExtraPlacementDistribution> > ( scope, "SimulatorExtraPlacementDistribution" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SimulatorExtraPlacementDistribution const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "dump_edge_proximities",
            ( std::string ( ::genesis::placement::SimulatorExtraPlacementDistribution::* )(  ) const )( &::genesis::placement::SimulatorExtraPlacementDistribution::dump_edge_proximities )
        )
        .def(
            "edge_proximity_maxima",
            ( std::vector< size_t > ( ::genesis::placement::SimulatorExtraPlacementDistribution::* )(  ) const )( &::genesis::placement::SimulatorExtraPlacementDistribution::edge_proximity_maxima )
        )
        .def(
            "generate",
            ( std::vector< size_t > ( ::genesis::placement::SimulatorExtraPlacementDistribution::* )( PlacementTreeEdge const & ))( &::genesis::placement::SimulatorExtraPlacementDistribution::generate ),
            pybind11::arg("edge")
        )
        .def(
            "prepare",
            ( void ( ::genesis::placement::SimulatorExtraPlacementDistribution::* )( Sample const & ))( &::genesis::placement::SimulatorExtraPlacementDistribution::prepare ),
            pybind11::arg("sample")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::placement::SimulatorExtraPlacementDistribution const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::SimulatorPendantLengthDistribution, scope )
{

    // -------------------------------------------------------------------
    //     Class SimulatorPendantLengthDistribution
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::SimulatorPendantLengthDistribution, std::shared_ptr<::genesis::placement::SimulatorPendantLengthDistribution> > ( scope, "SimulatorPendantLengthDistribution" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SimulatorPendantLengthDistribution const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "generate",
            ( double ( ::genesis::placement::SimulatorPendantLengthDistribution::* )( PlacementTreeEdge const & ))( &::genesis::placement::SimulatorPendantLengthDistribution::generate ),
            pybind11::arg("edge"),
            get_docstring("double ::genesis::placement::SimulatorPendantLengthDistribution::generate (PlacementTreeEdge const & edge)")
        )
        .def(
            "prepare",
            ( void ( ::genesis::placement::SimulatorPendantLengthDistribution::* )( Sample const & ))( &::genesis::placement::SimulatorPendantLengthDistribution::prepare ),
            pybind11::arg("sample"),
            get_docstring("void ::genesis::placement::SimulatorPendantLengthDistribution::prepare (Sample const & sample)")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::SimulatorLikeWeightRatioDistribution, scope )
{

    // -------------------------------------------------------------------
    //     Class SimulatorLikeWeightRatioDistribution
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::SimulatorLikeWeightRatioDistribution, std::shared_ptr<::genesis::placement::SimulatorLikeWeightRatioDistribution> > ( scope, "SimulatorLikeWeightRatioDistribution" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SimulatorLikeWeightRatioDistribution const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "generate",
            ( double ( ::genesis::placement::SimulatorLikeWeightRatioDistribution::* )(  ))( &::genesis::placement::SimulatorLikeWeightRatioDistribution::generate ),
            get_docstring("double ::genesis::placement::SimulatorLikeWeightRatioDistribution::generate ()")
        )
        .def(
            "prepare",
            ( void ( ::genesis::placement::SimulatorLikeWeightRatioDistribution::* )( Sample const & ))( &::genesis::placement::SimulatorLikeWeightRatioDistribution::prepare ),
            pybind11::arg("sample"),
            get_docstring("void ::genesis::placement::SimulatorLikeWeightRatioDistribution::prepare (Sample const & sample)")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::placement::SimulatorLikeWeightRatioDistribution const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::SimulatorEdgeDistribution, scope )
{

    // -------------------------------------------------------------------
    //     Class SimulatorEdgeDistribution
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::SimulatorEdgeDistribution, std::shared_ptr<::genesis::placement::SimulatorEdgeDistribution> > ( scope, "SimulatorEdgeDistribution" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SimulatorEdgeDistribution const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "generate",
            ( size_t ( ::genesis::placement::SimulatorEdgeDistribution::* )(  ))( &::genesis::placement::SimulatorEdgeDistribution::generate ),
            get_docstring("size_t ::genesis::placement::SimulatorEdgeDistribution::generate ()")
        )
        .def(
            "prepare",
            ( void ( ::genesis::placement::SimulatorEdgeDistribution::* )( Sample const & ))( &::genesis::placement::SimulatorEdgeDistribution::prepare ),
            pybind11::arg("sample"),
            get_docstring("void ::genesis::placement::SimulatorEdgeDistribution::prepare (Sample const & sample)")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::placement::SimulatorEdgeDistribution const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::SimulatorProximalLengthDistribution, scope )
{

    // -------------------------------------------------------------------
    //     Class SimulatorProximalLengthDistribution
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::SimulatorProximalLengthDistribution, std::shared_ptr<::genesis::placement::SimulatorProximalLengthDistribution> > ( scope, "SimulatorProximalLengthDistribution" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SimulatorProximalLengthDistribution const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "generate",
            ( double ( ::genesis::placement::SimulatorProximalLengthDistribution::* )( PlacementTreeEdge const & ))( &::genesis::placement::SimulatorProximalLengthDistribution::generate ),
            pybind11::arg("edge"),
            get_docstring("double ::genesis::placement::SimulatorProximalLengthDistribution::generate (PlacementTreeEdge const & edge)")
        )
        .def(
            "prepare",
            ( void ( ::genesis::placement::SimulatorProximalLengthDistribution::* )( Sample const & ))( &::genesis::placement::SimulatorProximalLengthDistribution::prepare ),
            pybind11::arg("sample"),
            get_docstring("void ::genesis::placement::SimulatorProximalLengthDistribution::prepare (Sample const & sample)")
        )
    ;
}
