/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_cog_export, ::genesis::placement, scope )
{

    scope.def(
        "center_of_gravity_distance",
        ( double ( * )( Sample const &, Sample const &, bool const ))( &::genesis::placement::center_of_gravity_distance ),
            pybind11::arg("smp_a"),
            pybind11::arg("smp_b"),
            pybind11::arg("with_pendant_length")=(bool const)(false),
        get_docstring("double ::genesis::placement::center_of_gravity_distance (Sample const & smp_a, Sample const & smp_b, bool const with_pendant_length=false)")
    );

    scope.def(
        "center_of_gravity_variance",
        ( double ( * )( Sample const &, bool const ))( &::genesis::placement::center_of_gravity_variance ),
            pybind11::arg("smp"),
            pybind11::arg("with_pendant_length")=(bool const)(false),
        get_docstring("double ::genesis::placement::center_of_gravity_variance (Sample const & smp, bool const with_pendant_length=false)")
    );

    scope.def(
        "center_of_gravity",
        ( std::pair< PlacementTreeEdge const *, double > ( * )( Sample const &, bool const ))( &::genesis::placement::center_of_gravity ),
            pybind11::arg("smp"),
            pybind11::arg("with_pendant_length")=(bool const)(false),
        get_docstring("std::pair< PlacementTreeEdge const *, double > ::genesis::placement::center_of_gravity (Sample const & smp, bool const with_pendant_length=false)")
    );
}
