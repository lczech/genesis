/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_formats_xml_helper_export, ::genesis::utils, scope )
{

    scope.def(
        "xml_comment",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::xml_comment ),
            pybind11::arg("content")
    );

    scope.def(
        "xml_deescape",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::xml_deescape ),
            pybind11::arg("content")
    );

    scope.def(
        "xml_escape",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::xml_escape ),
            pybind11::arg("content")
    );
}
