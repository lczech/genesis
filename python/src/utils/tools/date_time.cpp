/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_tools_date_time_export, "utils")
{

    boost::python::def(
        "current_date",
        ( std::string ( * )(  ))( &::genesis::utils::current_date ),
        get_docstring("std::string ::genesis::utils::current_date ()")
    );

    boost::python::def(
        "current_time",
        ( std::string ( * )(  ))( &::genesis::utils::current_time ),
        get_docstring("std::string ::genesis::utils::current_time ()")
    );
}
