/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_emd_export, ::genesis::placement, scope )
{

    scope.def(
        "earth_movers_distance",
        ( double ( * )( Sample const &, Sample const &, double const, bool const ))( &::genesis::placement::earth_movers_distance ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
            pybind11::arg("p")=(double const)(1.0),
            pybind11::arg("with_pendant_length")=(bool const)(false),
        get_docstring("double ::genesis::placement::earth_movers_distance (Sample const & lhs, Sample const & rhs, double const p=1.0, bool const with_pendant_length=false)")
    );

    scope.def(
        "earth_movers_distance",
        ( utils::Matrix< double > ( * )( SampleSet const &, double const, bool const ))( &::genesis::placement::earth_movers_distance ),
            pybind11::arg("sample_set"),
            pybind11::arg("p")=(double const)(1.0),
            pybind11::arg("with_pendant_length")=(bool const)(false),
        get_docstring("utils::Matrix< double > ::genesis::placement::earth_movers_distance (SampleSet const & sample_set, double const p=1.0, bool const with_pendant_length=false)")
    );
}
