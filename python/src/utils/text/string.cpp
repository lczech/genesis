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
        "contains_ci",
        ( bool ( * )( std::vector< std::string > const &, std::string const & ))( &::genesis::utils::contains_ci ),
            pybind11::arg("haystack"),
            pybind11::arg("needle"),
        get_docstring("bool ::genesis::utils::contains_ci (std::vector< std::string > const & haystack, std::string const & needle)")
    );

    scope.def(
        "ends_with",
        ( bool ( * )( std::string const &, std::string const & ))( &::genesis::utils::ends_with ),
            pybind11::arg("text"),
            pybind11::arg("ending"),
        get_docstring("bool ::genesis::utils::ends_with (std::string const & text, std::string const & ending)")
    );

    scope.def(
        "equals_ci",
        ( bool ( * )( std::string const &, std::string const & ))( &::genesis::utils::equals_ci ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("bool ::genesis::utils::equals_ci (std::string const & lhs, std::string const & rhs)")
    );

    scope.def(
        "starts_with",
        ( bool ( * )( std::string const &, std::string const & ))( &::genesis::utils::starts_with ),
            pybind11::arg("text"),
            pybind11::arg("start"),
        get_docstring("bool ::genesis::utils::starts_with (std::string const & text, std::string const & start)")
    );

    scope.def(
        "deescape",
        ( char ( * )( char ))( &::genesis::utils::deescape ),
            pybind11::arg("c"),
        get_docstring("char ::genesis::utils::deescape (char c)")
    );

    scope.def(
        "count_substring_occurrences",
        ( size_t ( * )( std::string const &, std::string const & ))( &::genesis::utils::count_substring_occurrences ),
            pybind11::arg("str"),
            pybind11::arg("sub"),
        get_docstring("size_t ::genesis::utils::count_substring_occurrences (std::string const & str, std::string const & sub)")
    );

    scope.def(
        "deescape",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::deescape ),
            pybind11::arg("text"),
        get_docstring("std::string ::genesis::utils::deescape (std::string const & text)")
    );

    scope.def(
        "escape",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::escape ),
            pybind11::arg("text"),
        get_docstring("std::string ::genesis::utils::escape (std::string const & text)")
    );

    scope.def(
        "head",
        ( std::string ( * )( std::string const &, size_t ))( &::genesis::utils::head ),
            pybind11::arg("text"),
            pybind11::arg("lines")=(size_t)(10),
        get_docstring("std::string ::genesis::utils::head (std::string const & text, size_t lines=10)")
    );

    scope.def(
        "indent",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::indent ),
            pybind11::arg("text"),
            pybind11::arg("indentation")=(std::string const &)("    "),
        get_docstring("std::string ::genesis::utils::indent (std::string const & text, std::string const & indentation=\"    \")")
    );

    scope.def(
        "repeat",
        ( std::string ( * )( std::string const &, size_t ))( &::genesis::utils::repeat ),
            pybind11::arg("word"),
            pybind11::arg("times"),
        get_docstring("std::string ::genesis::utils::repeat (std::string const & word, size_t times)")
    );

    scope.def(
        "replace_all",
        ( std::string ( * )( std::string const &, std::string const &, std::string const & ))( &::genesis::utils::replace_all ),
            pybind11::arg("text"),
            pybind11::arg("search"),
            pybind11::arg("replace"),
        get_docstring("std::string ::genesis::utils::replace_all (std::string const & text, std::string const & search, std::string const & replace)")
    );

    scope.def(
        "replace_all_chars",
        ( std::string ( * )( std::string const &, std::string const &, char ))( &::genesis::utils::replace_all_chars ),
            pybind11::arg("text"),
            pybind11::arg("search_chars"),
            pybind11::arg("replace"),
        get_docstring("std::string ::genesis::utils::replace_all_chars (std::string const & text, std::string const & search_chars, char replace)")
    );

    scope.def(
        "tail",
        ( std::string ( * )( std::string const &, size_t ))( &::genesis::utils::tail ),
            pybind11::arg("text"),
            pybind11::arg("lines")=(size_t)(10),
        get_docstring("std::string ::genesis::utils::tail (std::string const & text, size_t lines=10)")
    );

    scope.def(
        "to_lower",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_lower ),
            pybind11::arg("str"),
        get_docstring("std::string ::genesis::utils::to_lower (std::string const & str)")
    );

    scope.def(
        "to_lower_ascii",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_lower_ascii ),
            pybind11::arg("str"),
        get_docstring("std::string ::genesis::utils::to_lower_ascii (std::string const & str)")
    );

    scope.def(
        "to_string_leading_zeros",
        ( std::string ( * )( size_t, size_t ))( &::genesis::utils::to_string_leading_zeros ),
            pybind11::arg("value"),
            pybind11::arg("length")=(size_t)(6),
        get_docstring("std::string ::genesis::utils::to_string_leading_zeros (size_t value, size_t length=6)")
    );

    scope.def(
        "to_string_precise",
        ( std::string ( * )( double, int ))( &::genesis::utils::to_string_precise ),
            pybind11::arg("value"),
            pybind11::arg("precision")=(int)(6),
        get_docstring("std::string ::genesis::utils::to_string_precise (double value, int precision=6)")
    );

    scope.def(
        "to_string_rounded",
        ( std::string ( * )( double, int ))( &::genesis::utils::to_string_rounded ),
            pybind11::arg("value"),
            pybind11::arg("precision")=(int)(6),
        get_docstring("std::string ::genesis::utils::to_string_rounded (double value, int precision=6)")
    );

    scope.def(
        "to_upper",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_upper ),
            pybind11::arg("str"),
        get_docstring("std::string ::genesis::utils::to_upper (std::string const & str)")
    );

    scope.def(
        "to_upper_ascii",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::to_upper_ascii ),
            pybind11::arg("str"),
        get_docstring("std::string ::genesis::utils::to_upper_ascii (std::string const & str)")
    );

    scope.def(
        "trim",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim ),
            pybind11::arg("s"),
            pybind11::arg("delimiters")=(std::string const &)(" \f\n\r\t\v"),
        get_docstring("std::string ::genesis::utils::trim (std::string const & s, std::string const & delimiters=\" \\f\\n\\r\\t\\v\")")
    );

    scope.def(
        "trim_left",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim_left ),
            pybind11::arg("s"),
            pybind11::arg("delimiters")=(std::string const &)(" \f\n\r\t\v"),
        get_docstring("std::string ::genesis::utils::trim_left (std::string const & s, std::string const & delimiters=\" \\f\\n\\r\\t\\v\")")
    );

    scope.def(
        "trim_right",
        ( std::string ( * )( std::string const &, std::string const & ))( &::genesis::utils::trim_right ),
            pybind11::arg("s"),
            pybind11::arg("delimiters")=(std::string const &)(" \f\n\r\t\v"),
        get_docstring("std::string ::genesis::utils::trim_right (std::string const & s, std::string const & delimiters=\" \\f\\n\\r\\t\\v\")")
    );

    scope.def(
        "split_range_list",
        ( std::vector< size_t > ( * )( std::string const & ))( &::genesis::utils::split_range_list ),
            pybind11::arg("str"),
        get_docstring("std::vector< size_t > ::genesis::utils::split_range_list (std::string const & str)")
    );

    scope.def(
        "split",
        ( std::vector< std::string > ( * )( std::string const &, std::function< bool(char)>, const bool ))( &::genesis::utils::split ),
            pybind11::arg("str"),
            pybind11::arg("delimiter_predicate"),
            pybind11::arg("trim_empty")=(const bool)(true),
        get_docstring("std::vector< std::string > ::genesis::utils::split (std::string const & str, std::function< bool(char)> delimiter_predicate, const bool trim_empty=true)")
    );

    scope.def(
        "split",
        ( std::vector< std::string > ( * )( std::string const &, std::string const &, const bool ))( &::genesis::utils::split ),
            pybind11::arg("str"),
            pybind11::arg("delimiters")=(std::string const &)(" "),
            pybind11::arg("trim_empty")=(const bool)(true),
        get_docstring("std::vector< std::string > ::genesis::utils::split (std::string const & str, std::string const & delimiters=\" \", const bool trim_empty=true)")
    );

    scope.def(
        "split_at",
        ( std::vector< std::string > ( * )( std::string const &, std::string const &, const bool ))( &::genesis::utils::split_at ),
            pybind11::arg("str"),
            pybind11::arg("delimiter"),
            pybind11::arg("trim_empty")=(const bool)(true),
        get_docstring("std::vector< std::string > ::genesis::utils::split_at (std::string const & str, std::string const & delimiter, const bool trim_empty=true)")
    );

    scope.def(
        "to_lower_ascii_inplace",
        ( void ( * )( std::string & ))( &::genesis::utils::to_lower_ascii_inplace ),
            pybind11::arg("str"),
        get_docstring("void ::genesis::utils::to_lower_ascii_inplace (std::string & str)")
    );

    scope.def(
        "to_lower_inplace",
        ( void ( * )( std::string & ))( &::genesis::utils::to_lower_inplace ),
            pybind11::arg("str"),
        get_docstring("void ::genesis::utils::to_lower_inplace (std::string & str)")
    );

    scope.def(
        "to_upper_ascii_inplace",
        ( void ( * )( std::string & ))( &::genesis::utils::to_upper_ascii_inplace ),
            pybind11::arg("str"),
        get_docstring("void ::genesis::utils::to_upper_ascii_inplace (std::string & str)")
    );

    scope.def(
        "to_upper_inplace",
        ( void ( * )( std::string & ))( &::genesis::utils::to_upper_inplace ),
            pybind11::arg("str"),
        get_docstring("void ::genesis::utils::to_upper_inplace (std::string & str)")
    );
}
