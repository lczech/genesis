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

    scope.def(
        "to_lower_ascii",
        ( char ( * )( char ))( &::genesis::utils::to_lower_ascii ),
            pybind11::arg("c"),
        get_docstring("char ::genesis::utils::to_lower_ascii (char c)")
    );

    scope.def(
        "to_upper_ascii",
        ( char ( * )( char ))( &::genesis::utils::to_upper_ascii ),
            pybind11::arg("c"),
        get_docstring("char ::genesis::utils::to_upper_ascii (char c)")
    );

    scope.def(
        "char_to_hex",
        ( std::string ( * )( char, bool ))( &::genesis::utils::char_to_hex ),
            pybind11::arg("c"),
            pybind11::arg("full")=(bool)(false),
        get_docstring("std::string ::genesis::utils::char_to_hex (char c, bool full=false)")
    );
}
