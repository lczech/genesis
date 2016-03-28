/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_text_char_export, "utils")
{

    boost::python::def(
        "char_is_digit",
        ( bool ( * )( const char ))( &::genesis::utils::char_is_digit ),
        ( boost::python::arg("c") ),
        get_docstring("bool ::genesis::utils::char_is_digit (const char c)")
    );

    boost::python::def(
        "char_is_sign",
        ( bool ( * )( const char ))( &::genesis::utils::char_is_sign ),
        ( boost::python::arg("c") ),
        get_docstring("bool ::genesis::utils::char_is_sign (const char c)")
    );

    boost::python::def(
        "char_match_ci",
        ( bool ( * )( const char, const char ))( &::genesis::utils::char_match_ci ),
        ( boost::python::arg("c1"), boost::python::arg("c2") ),
        get_docstring("bool ::genesis::utils::char_match_ci (const char c1, const char c2)")
    );
}
