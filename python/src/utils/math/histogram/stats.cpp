/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_math_histogram_stats_export, "utils")
{

    boost::python::def(
        "max_value",
        ( double ( * )( const Histogram & ))( &::genesis::utils::max_value ),
        ( boost::python::arg("h") )
    );

    boost::python::def(
        "mean",
        ( double ( * )( const Histogram & ))( &::genesis::utils::mean ),
        ( boost::python::arg("h") ),
        get_docstring("double ::genesis::utils::mean (const Histogram & h)")
    );

    boost::python::def(
        "median",
        ( double ( * )( const Histogram & ))( &::genesis::utils::median ),
        ( boost::python::arg("h") )
    );

    boost::python::def(
        "min_value",
        ( double ( * )( const Histogram & ))( &::genesis::utils::min_value ),
        ( boost::python::arg("h") )
    );

    boost::python::def(
        "sigma",
        ( double ( * )( const Histogram & ))( &::genesis::utils::sigma ),
        ( boost::python::arg("h") ),
        get_docstring("double ::genesis::utils::sigma (const Histogram & h)")
    );

    boost::python::def(
        "sum",
        ( double ( * )( const Histogram & ))( &::genesis::utils::sum ),
        ( boost::python::arg("h") )
    );

    boost::python::def(
        "max_bin",
        ( size_t ( * )( const Histogram & ))( &::genesis::utils::max_bin ),
        ( boost::python::arg("h") )
    );

    boost::python::def(
        "min_bin",
        ( size_t ( * )( const Histogram & ))( &::genesis::utils::min_bin ),
        ( boost::python::arg("h") )
    );
}
