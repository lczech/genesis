/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis;

PYTHON_EXPORT_FUNCTIONS( utils_core_version_export, ::genesis, scope )
{

    scope.def(
        "ee",
        ( std::string ( * )( int ))( &::genesis::ee ),
            pybind11::arg("r")
    );

    scope.def(
        "genesis_header",
        ( std::string ( * )(  ))( &::genesis::genesis_header ),
        get_docstring("std::string ::genesis::genesis_header ()")
    );

    scope.def(
        "genesis_license",
        ( std::string ( * )(  ))( &::genesis::genesis_license ),
        get_docstring("std::string ::genesis::genesis_license ()")
    );

    scope.def(
        "genesis_url",
        ( std::string ( * )(  ))( &::genesis::genesis_url ),
        get_docstring("std::string ::genesis::genesis_url ()")
    );

    scope.def(
        "genesis_version",
        ( std::string ( * )(  ))( &::genesis::genesis_version ),
        get_docstring("std::string ::genesis::genesis_version ()")
    );

    scope.def(
        "genesis_version_name",
        ( std::string ( * )(  ))( &::genesis::genesis_version_name ),
        get_docstring("std::string ::genesis::genesis_version_name ()")
    );
}
