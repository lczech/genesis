/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_qualitative_lists_export, ::genesis::utils, scope )
{

    scope.def(
        "color_list_accent",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_accent ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_accent ()")
    );

    scope.def(
        "color_list_dark2",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_dark2 ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_dark2 ()")
    );

    scope.def(
        "color_list_paired",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_paired ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_paired ()")
    );

    scope.def(
        "color_list_pastel1",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_pastel1 ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_pastel1 ()")
    );

    scope.def(
        "color_list_pastel2",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_pastel2 ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_pastel2 ()")
    );

    scope.def(
        "color_list_set1",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_set1 ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_set1 ()")
    );

    scope.def(
        "color_list_set2",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_set2 ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_set2 ()")
    );

    scope.def(
        "color_list_set3",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_set3 ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_set3 ()")
    );

    scope.def(
        "qualitative_color_list",
        ( std::vector< Color > const & ( * )( QualitativeColorList ))( &::genesis::utils::qualitative_color_list ),
            pybind11::arg("palette")
    );

    scope.def(
        "qualitative_color_list",
        ( std::vector< Color > const & ( * )( std::string const & ))( &::genesis::utils::qualitative_color_list ),
            pybind11::arg("palette")
    );

    scope.def(
        "qualitative_color_list_names",
        ( std::vector< std::string > ( * )(  ))( &::genesis::utils::qualitative_color_list_names )
    );
}
