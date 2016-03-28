/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_math_histogram_distances_export, "utils")
{

    boost::python::def(
        "earth_movers_distance",
        ( double ( * )( const Histogram &, const Histogram &, bool ))( &::genesis::utils::earth_movers_distance ),
        ( boost::python::arg("h1"), boost::python::arg("h2"), boost::python::arg("norm") )
    );

    // boost::python::def(
    //     "earth_movers_distance_simple",
    //     ( double ( * )( const Histogram &, const Histogram & ))( &::genesis::utils::earth_movers_distance_simple ),
    //     ( boost::python::arg("h1"), boost::python::arg("h2") )
    // );
}
