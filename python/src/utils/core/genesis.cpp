/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis;

PYTHON_EXPORT_FUNCTIONS(utils_core_genesis_export, "")
{

    boost::python::def(
        "ee",
        ( std::string ( * )( int ))( &::genesis::ee ),
        ( boost::python::arg("r") )
    );

    boost::python::def(
        "genesis_header",
        ( std::string ( * )(  ))( &::genesis::genesis_header ),
        get_docstring("std::string ::genesis::genesis_header ()")
    );

    boost::python::def(
        "genesis_version",
        ( std::string ( * )(  ))( &::genesis::genesis_version ),
        get_docstring("std::string ::genesis::genesis_version ()")
    );
}
