/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::ColorNames, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorNames
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::ColorNames, std::shared_ptr<::genesis::utils::ColorNames> > ( scope, "ColorNames" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_names_export, ::genesis::utils, scope )
{

    scope.def(
        "get_named_color",
        ( Color ( * )( std::string const & ))( &::genesis::utils::get_named_color ),
            pybind11::arg("name")
    );

    scope.def(
        "get_named_color_at",
        ( Color ( * )( size_t ))( &::genesis::utils::get_named_color_at ),
            pybind11::arg("at")
    );

    scope.def(
        "is_named_color",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_named_color ),
            pybind11::arg("name")
    );

    scope.def(
        "get_named_color_index",
        ( size_t ( * )( std::string const & ))( &::genesis::utils::get_named_color_index ),
            pybind11::arg("name")
    );
}
