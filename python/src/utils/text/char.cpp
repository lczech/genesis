/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_text_char_export, ::genesis::utils, scope )
{

    scope.def(
        "char_is_digit",
        ( bool ( * )( const char ))( &::genesis::utils::char_is_digit ),
            pybind11::arg("c"),
        get_docstring("bool ::genesis::utils::char_is_digit (const char c)")
    );

    scope.def(
        "char_is_number_part",
        ( bool ( * )( char const ))( &::genesis::utils::char_is_number_part ),
            pybind11::arg("c"),
        get_docstring("bool ::genesis::utils::char_is_number_part (char const c)")
    );

    scope.def(
        "char_is_sign",
        ( bool ( * )( const char ))( &::genesis::utils::char_is_sign ),
            pybind11::arg("c"),
        get_docstring("bool ::genesis::utils::char_is_sign (const char c)")
    );

    scope.def(
        "char_match_ci",
        ( bool ( * )( const char, const char ))( &::genesis::utils::char_match_ci ),
            pybind11::arg("c1"),
            pybind11::arg("c2"),
        get_docstring("bool ::genesis::utils::char_match_ci (const char c1, const char c2)")
    );
}
