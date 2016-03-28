/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_math_histogram_operators_export, "utils")
{

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, const Histogram & ))( &::genesis::utils::operator<< ),
        ( boost::python::arg("os"), boost::python::arg("h") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>()
    );

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, const HistogramAccumulator & ))( &::genesis::utils::operator<< ),
        ( boost::python::arg("os"), boost::python::arg("h") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>()
    );
}
