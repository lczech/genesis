/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_measures_export, ::genesis::placement, scope )
{

    scope.def(
        "center_of_gravity_distance",
        ( double ( * )( const Sample &, const Sample &, bool ))( &::genesis::placement::center_of_gravity_distance ),
            pybind11::arg("smp_a"),
            pybind11::arg("smp_b"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::center_of_gravity_distance (const Sample & smp_a, const Sample & smp_b, bool with_pendant_length=false)")
    );

    scope.def(
        "center_of_gravity_variance",
        ( double ( * )( const Sample &, bool ))( &::genesis::placement::center_of_gravity_variance ),
            pybind11::arg("smp"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::center_of_gravity_variance (const Sample & smp, bool with_pendant_length=false)")
    );

    scope.def(
        "earth_movers_distance",
        ( double ( * )( const Sample &, const Sample &, bool ))( &::genesis::placement::earth_movers_distance ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::earth_movers_distance (const Sample & lhs, const Sample & rhs, bool with_pendant_length=false)")
    );

    scope.def(
        "edpl",
        ( double ( * )( Sample const &, Pquery const & ))( &::genesis::placement::edpl ),
            pybind11::arg("sample"),
            pybind11::arg("pquery"),
        get_docstring("double ::genesis::placement::edpl (Sample const & sample, Pquery const & pquery)")
    );

    scope.def(
        "expected_distance_between_placement_locations",
        ( double ( * )( Sample const &, Pquery const & ))( &::genesis::placement::expected_distance_between_placement_locations ),
            pybind11::arg("sample"),
            pybind11::arg("pquery"),
        get_docstring("double ::genesis::placement::expected_distance_between_placement_locations (Sample const & sample, Pquery const & pquery)")
    );

    scope.def(
        "node_histogram_distance",
        ( double ( * )( Sample const &, Sample const &, size_t ))( &::genesis::placement::node_histogram_distance ),
            pybind11::arg("sample_a"),
            pybind11::arg("sample_b"),
            pybind11::arg("histogram_bins")=(size_t)(25),
        get_docstring("double ::genesis::placement::node_histogram_distance (Sample const & sample_a, Sample const & sample_b, size_t histogram_bins=25)")
    );

    scope.def(
        "pairwise_distance",
        ( double ( * )( const Sample &, const Sample &, bool ))( &::genesis::placement::pairwise_distance ),
            pybind11::arg("smp_a"),
            pybind11::arg("smp_b"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::pairwise_distance (const Sample & smp_a, const Sample & smp_b, bool with_pendant_length=false)")
    );

    scope.def(
        "placement_distance",
        ( double ( * )( PqueryPlacement const &, PqueryPlacement const &, utils::Matrix< double > const & ))( &::genesis::placement::placement_distance ),
            pybind11::arg("place_a"),
            pybind11::arg("place_b"),
            pybind11::arg("node_distances")
    );

    scope.def(
        "pquery_distance",
        ( double ( * )( const PqueryPlain &, const PqueryPlain &, const utils::Matrix< double > &, bool ))( &::genesis::placement::pquery_distance ),
            pybind11::arg("pqry_a"),
            pybind11::arg("pqry_b"),
            pybind11::arg("node_distances"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::pquery_distance (const PqueryPlain & pqry_a, const PqueryPlain & pqry_b, const utils::Matrix< double > & node_distances, bool with_pendant_length=false)")
    );

    scope.def(
        "variance",
        ( double ( * )( const Sample &, bool ))( &::genesis::placement::variance ),
            pybind11::arg("smp"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::variance (const Sample & smp, bool with_pendant_length=false)")
    );

    scope.def(
        "center_of_gravity",
        ( std::pair< PlacementTreeEdge const *, double > ( * )( const Sample &, bool ))( &::genesis::placement::center_of_gravity ),
            pybind11::arg("smp"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("std::pair< PlacementTreeEdge const *, double > ::genesis::placement::center_of_gravity (const Sample & smp, bool with_pendant_length=false)")
    );

    scope.def(
        "edpl",
        ( std::vector< double > ( * )( Sample const & ))( &::genesis::placement::edpl ),
            pybind11::arg("sample"),
        get_docstring("std::vector< double > ::genesis::placement::edpl (Sample const & sample)")
    );

    scope.def(
        "expected_distance_between_placement_locations",
        ( std::vector< double > ( * )( Sample const & ))( &::genesis::placement::expected_distance_between_placement_locations ),
            pybind11::arg("sample"),
        get_docstring("std::vector< double > ::genesis::placement::expected_distance_between_placement_locations (Sample const & sample)")
    );

    scope.def(
        "earth_movers_distance",
        ( utils::Matrix< double > ( * )( SampleSet const &, bool ))( &::genesis::placement::earth_movers_distance ),
            pybind11::arg("sample_set"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("utils::Matrix< double > ::genesis::placement::earth_movers_distance (SampleSet const & sample_set, bool with_pendant_length=false)")
    );

    scope.def(
        "node_histogram_distance",
        ( utils::Matrix< double > ( * )( SampleSet const &, size_t ))( &::genesis::placement::node_histogram_distance ),
            pybind11::arg("sample_set"),
            pybind11::arg("histogram_bins")=(size_t)(25),
        get_docstring("utils::Matrix< double > ::genesis::placement::node_histogram_distance (SampleSet const & sample_set, size_t histogram_bins=25)")
    );
}
