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
        "edpl",
        ( double ( * )( Pquery const &, utils::Matrix< double > const & ))( &::genesis::placement::edpl ),
            pybind11::arg("pquery"),
            pybind11::arg("node_distances"),
        get_docstring("double ::genesis::placement::edpl (Pquery const & pquery, utils::Matrix< double > const & node_distances)")
    );

    scope.def(
        "edpl",
        ( double ( * )( Sample const &, Pquery const & ))( &::genesis::placement::edpl ),
            pybind11::arg("sample"),
            pybind11::arg("pquery"),
        get_docstring("double ::genesis::placement::edpl (Sample const & sample, Pquery const & pquery)")
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
        "variance",
        ( double ( * )( const Sample &, bool ))( &::genesis::placement::variance ),
            pybind11::arg("smp"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::variance (const Sample & smp, bool with_pendant_length=false)")
    );

    scope.def(
        "edpl",
        ( std::vector< double > ( * )( Sample const & ))( &::genesis::placement::edpl ),
            pybind11::arg("sample"),
        get_docstring("std::vector< double > ::genesis::placement::edpl (Sample const & sample)")
    );

    scope.def(
        "edpl",
        ( std::vector< double > ( * )( Sample const &, utils::Matrix< double > const & ))( &::genesis::placement::edpl ),
            pybind11::arg("sample"),
            pybind11::arg("node_distances"),
        get_docstring("std::vector< double > ::genesis::placement::edpl (Sample const & sample, utils::Matrix< double > const & node_distances)")
    );
}
