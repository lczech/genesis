/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_date_time_export, ::genesis::utils, scope )
{

    scope.def(
        "current_date",
        ( std::string ( * )(  ))( &::genesis::utils::current_date ),
        get_docstring("std::string ::genesis::utils::current_date ()")
    );

    scope.def(
        "current_time",
        ( std::string ( * )(  ))( &::genesis::utils::current_time ),
        get_docstring("std::string ::genesis::utils::current_time ()")
    );
}
