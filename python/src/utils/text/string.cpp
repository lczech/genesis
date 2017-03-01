/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_text_string_export, ::genesis::utils, scope )
{

    scope.def(
        "ends_with",
        ( bool ( * )( std::string const &, std::string const & ))( &::genesis::utils::ends_with ),
            pybind11::arg("text"),
            pybind11::arg("ending")
    );

    scope.def(
        "equals_ci",
        ( bool ( * )( std::string const &, std::string const & ))( &::genesis::utils::equals_ci ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "starts_with",
        ( bool ( * )( std::string const &, std::string const & ))( &::genesis::utils::starts_with ),
            pybind11::arg("text"),
            pybind11::arg("start")
    );

    scope.def(
        "deescape",
        ( char ( * )( char ))( &::genesis::utils::deescape ),
            pybind11::arg("c")
    );

    scope.def(
        "to_lower_ascii",
        ( char ( * )( char ))( &::genesis::utils::to_lower_ascii ),
            pybind11::arg("c")
    );

    scope.def(
        "to_upper_ascii",
        ( char ( * )( char ))( &::genesis::utils::to_upper_ascii ),
            pybind11::arg("c")
    );

    scope.def(
        "count_substring_occurrences",
        ( size_t ( * )( std::string const &, std::string const & ))( &::genesis::utils::count_substring_occurrences ),
            pybind11::arg("str"),
            pybind11::arg("sub")
    );

    scope.def(
        "deescape",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::deescape ),
            pybind11::arg("text")
    );

    scope.def(
        "escape",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::escape ),
            pybind11::arg("text")
    );

    scope.def(
        "head",
        ( std::string ( * )( std::string const &, size_t ))( &::genesis::utils::head ),
            pybind11::arg("text"),
            pybind11::arg("lines")=(size_t)(10)
    );

    scope.def(
        "indent",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::indent ),
            pybind11::arg("text"),
            pybind11::arg("indentation")=(std::string const &)("    ")
    );

    scope.def(
        "repeat",
        ( std::string ( * )( std::string const &, size_t ))( &::genesis::utils::repeat ),
            pybind11::arg("word"),
            pybind11::arg("times")
    );

    scope.def(
        "replace_all",
        ( std::string ( * )( std::string const &, std::string const &, std::string const & ))( &::genesis::utils::replace_all ),
            pybind11::arg("text"),
            pybind11::arg("search"),
            pybind11::arg("replace")
    );

    scope.def(
        "replace_all_chars",
        ( std::string ( * )( std::string const &, std::string const &, char ))( &::genesis::utils::replace_all_chars ),
            pybind11::arg("text"),
            pybind11::arg("search_chars"),
            pybind11::arg("replace")
    );

    scope.def(
        "tail",
        ( std::string ( * )( std::string const &, size_t ))( &::genesis::utils::tail ),
            pybind11::arg("text"),
            pybind11::arg("lines")=(size_t)(10)
    );

    scope.def(
        "to_lower",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_lower ),
            pybind11::arg("str")
    );

    scope.def(
        "to_lower_ascii",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_lower_ascii ),
            pybind11::arg("str")
    );

    scope.def(
        "to_string_precise",
        ( std::string ( * )( double, const int ))( &::genesis::utils::to_string_precise ),
            pybind11::arg("value"),
            pybind11::arg("precision")=(const int)(6)
    );

    scope.def(
        "to_string_rounded",
        ( std::string ( * )( double, const int ))( &::genesis::utils::to_string_rounded ),
            pybind11::arg("value"),
            pybind11::arg("precision")=(const int)(6)
    );

    scope.def(
        "to_upper",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_upper ),
            pybind11::arg("str")
    );

    scope.def(
        "to_upper_ascii",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_upper_ascii ),
            pybind11::arg("str")
    );

    scope.def(
        "trim",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim ),
            pybind11::arg("s"),
            pybind11::arg("delimiters")=(std::string const &)(" \f\n\r\t\v")
    );

    scope.def(
        "trim_left",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim_left ),
            pybind11::arg("s"),
            pybind11::arg("delimiters")=(std::string const &)(" \f\n\r\t\v")
    );

    scope.def(
        "trim_right",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim_right ),
            pybind11::arg("s"),
            pybind11::arg("delimiters")=(std::string const &)(" \f\n\r\t\v")
    );

    scope.def(
        "split",
        ( std::vector< std::string > ( * )( std::string const &, std::string const &, const bool ))( &::genesis::utils::split ),
            pybind11::arg("str"),
            pybind11::arg("delimiters")=(std::string const &)(" "),
            pybind11::arg("trim_empty")=(const bool)(true)
    );

    scope.def(
        "to_lower_ascii_inplace",
        ( void ( * )( std::string & ))( &::genesis::utils::to_lower_ascii_inplace ),
            pybind11::arg("str")
    );

    scope.def(
        "to_lower_inplace",
        ( void ( * )( std::string & ))( &::genesis::utils::to_lower_inplace ),
            pybind11::arg("str")
    );

    scope.def(
        "to_upper_ascii_inplace",
        ( void ( * )( std::string & ))( &::genesis::utils::to_upper_ascii_inplace ),
            pybind11::arg("str")
    );

    scope.def(
        "to_upper_inplace",
        ( void ( * )( std::string & ))( &::genesis::utils::to_upper_inplace ),
            pybind11::arg("str")
    );
}
