/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_sequential_lists_export, ::genesis::utils, scope )
{

    scope.def(
        "color_list_blues",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_blues ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_blues ()")
    );

    scope.def(
        "color_list_bugn",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_bugn ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_bugn ()")
    );

    scope.def(
        "color_list_bupu",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_bupu ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_bupu ()")
    );

    scope.def(
        "color_list_bupubk",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_bupubk ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_bupubk ()")
    );

    scope.def(
        "color_list_gnbu",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_gnbu ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_gnbu ()")
    );

    scope.def(
        "color_list_greens",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_greens ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_greens ()")
    );

    scope.def(
        "color_list_greys",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_greys ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_greys ()")
    );

    scope.def(
        "color_list_inferno",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_inferno ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_inferno ()")
    );

    scope.def(
        "color_list_magma",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_magma ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_magma ()")
    );

    scope.def(
        "color_list_oranges",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_oranges ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_oranges ()")
    );

    scope.def(
        "color_list_orrd",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_orrd ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_orrd ()")
    );

    scope.def(
        "color_list_plasma",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_plasma ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_plasma ()")
    );

    scope.def(
        "color_list_pubu",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_pubu ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_pubu ()")
    );

    scope.def(
        "color_list_pubugn",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_pubugn ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_pubugn ()")
    );

    scope.def(
        "color_list_purd",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_purd ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_purd ()")
    );

    scope.def(
        "color_list_purples",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_purples ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_purples ()")
    );

    scope.def(
        "color_list_rdpu",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_rdpu ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_rdpu ()")
    );

    scope.def(
        "color_list_reds",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_reds ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_reds ()")
    );

    scope.def(
        "color_list_viridis",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_viridis ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_viridis ()")
    );

    scope.def(
        "color_list_ylgn",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_ylgn ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_ylgn ()")
    );

    scope.def(
        "color_list_ylgnbu",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_ylgnbu ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_ylgnbu ()")
    );

    scope.def(
        "color_list_ylorbr",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_ylorbr ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_ylorbr ()")
    );

    scope.def(
        "color_list_ylorrd",
        ( std::vector< Color > const & ( * )(  ))( &::genesis::utils::color_list_ylorrd ),
        get_docstring("std::vector< Color > const & ::genesis::utils::color_list_ylorrd ()")
    );

    scope.def(
        "sequential_color_list",
        ( std::vector< Color > const & ( * )( SequentialColorList ))( &::genesis::utils::sequential_color_list ),
            pybind11::arg("palette")
    );

    scope.def(
        "sequential_color_list",
        ( std::vector< Color > const & ( * )( std::string const & ))( &::genesis::utils::sequential_color_list ),
            pybind11::arg("palette")
    );

    scope.def(
        "sequential_color_list_names",
        ( std::vector< std::string > ( * )(  ))( &::genesis::utils::sequential_color_list_names )
    );
}
