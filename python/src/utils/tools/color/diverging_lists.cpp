/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_diverging_lists_export, ::genesis::utils, scope )
{

    scope.def(
        "color_list_brbg",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_brbg ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_brbg ()")
    );

    scope.def(
        "color_list_piyg",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_piyg ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_piyg ()")
    );

    scope.def(
        "color_list_prgn",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_prgn ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_prgn ()")
    );

    scope.def(
        "color_list_puor",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_puor ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_puor ()")
    );

    scope.def(
        "color_list_rdbu",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_rdbu ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_rdbu ()")
    );

    scope.def(
        "color_list_rdgy",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_rdgy ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_rdgy ()")
    );

    scope.def(
        "color_list_rdylbu",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_rdylbu ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_rdylbu ()")
    );

    scope.def(
        "color_list_rdylgn",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_rdylgn ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_rdylgn ()")
    );

    scope.def(
        "color_list_spectral",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_spectral ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_spectral ()")
    );

    scope.def(
        "diverging_color_list",
        ( std::vector< Color > const & ( * )( DivergingColorList ))( &::genesis::utils::diverging_color_list ),
            pybind11::arg("palette")
    );

    scope.def(
        "diverging_color_list",
        ( std::vector< Color > const & ( * )( std::string const & ))( &::genesis::utils::diverging_color_list ),
            pybind11::arg("palette")
    );

    scope.def(
        "diverging_color_list_names",
        ( std::vector< std::string > ( * )(  ))( &::genesis::utils::diverging_color_list_names )
    );
}
