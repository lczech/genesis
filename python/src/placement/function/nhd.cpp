/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::NodeDistanceHistogram, scope )
{

    // -------------------------------------------------------------------
    //     Class NodeDistanceHistogram
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::NodeDistanceHistogram, std::shared_ptr<::genesis::placement::NodeDistanceHistogram> > ( scope, "NodeDistanceHistogram" )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::NodeDistanceHistogramSet, scope )
{

    // -------------------------------------------------------------------
    //     Class NodeDistanceHistogramSet
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::NodeDistanceHistogramSet, std::shared_ptr<::genesis::placement::NodeDistanceHistogramSet> > ( scope, "NodeDistanceHistogramSet" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( placement_function_nhd_export, ::genesis::placement, scope )
{

    scope.def(
        "node_distance_histogram_set",
        ( NodeDistanceHistogramSet ( * )( Sample const &, utils::Matrix< double > const &, utils::Matrix< signed char > const &, size_t const ))( &::genesis::placement::node_distance_histogram_set ),
            pybind11::arg("sample"),
            pybind11::arg("node_distances"),
            pybind11::arg("node_sides"),
            pybind11::arg("histogram_bins"),
        get_docstring("NodeDistanceHistogramSet ::genesis::placement::node_distance_histogram_set (Sample const & sample, utils::Matrix< double > const & node_distances, utils::Matrix< signed char > const & node_sides, size_t const histogram_bins)")
    );

    scope.def(
        "node_histogram_distance",
        ( double ( * )( NodeDistanceHistogramSet const &, NodeDistanceHistogramSet const & ))( &::genesis::placement::node_histogram_distance ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("double ::genesis::placement::node_histogram_distance (NodeDistanceHistogramSet const & lhs, NodeDistanceHistogramSet const & rhs)")
    );

    scope.def(
        "node_histogram_distance",
        ( double ( * )( Sample const &, Sample const &, size_t const ))( &::genesis::placement::node_histogram_distance ),
            pybind11::arg("sample_a"),
            pybind11::arg("sample_b"),
            pybind11::arg("histogram_bins")=(size_t const)(25),
        get_docstring("double ::genesis::placement::node_histogram_distance (Sample const & sample_a, Sample const & sample_b, size_t const histogram_bins=25)")
    );

    scope.def(
        "node_histogram_distance",
        ( utils::Matrix< double > ( * )( SampleSet const &, size_t const ))( &::genesis::placement::node_histogram_distance ),
            pybind11::arg("sample_set"),
            pybind11::arg("histogram_bins")=(size_t const)(25),
        get_docstring("utils::Matrix< double > ::genesis::placement::node_histogram_distance (SampleSet const & sample_set, size_t const histogram_bins=25)")
    );

    scope.def(
        "node_histogram_distance",
        ( utils::Matrix< double > ( * )( std::vector< NodeDistanceHistogramSet > const & ))( &::genesis::placement::node_histogram_distance ),
            pybind11::arg("histogram_sets"),
        get_docstring("utils::Matrix< double > ::genesis::placement::node_histogram_distance (std::vector< NodeDistanceHistogramSet > const & histogram_sets)")
    );
}
