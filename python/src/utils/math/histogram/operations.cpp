/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_math_histogram_operations_export, "utils")
{

    boost::python::def(
        "normalize",
        ( void ( * )( Histogram &, double ))( &::genesis::utils::normalize ),
        ( boost::python::arg("h"), boost::python::arg("total") )
    );

    boost::python::def(
        "offset",
        ( void ( * )( Histogram &, double ))( &::genesis::utils::offset ),
        ( boost::python::arg("h"), boost::python::arg("value") )
    );

    boost::python::def(
        "scale",
        ( void ( * )( Histogram &, double ))( &::genesis::utils::scale ),
        ( boost::python::arg("h"), boost::python::arg("factor") )
    );
}
