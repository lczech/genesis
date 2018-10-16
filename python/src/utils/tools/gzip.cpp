/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_gzip_export, ::genesis::utils, scope )
{

    scope.def(
        "is_gzip_compressed_file",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_gzip_compressed_file ),
            pybind11::arg("file_name")
    );
}
