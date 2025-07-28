#include <genesis/utils/text/string.hpp>
#include <iterator>
#include <memory>
#include <string>

#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

#ifndef BINDER_PYBIND11_TYPE_CASTER
#    define BINDER_PYBIND11_TYPE_CASTER
PYBIND11_DECLARE_HOLDER_TYPE( T, std::shared_ptr<T>, false )
PYBIND11_DECLARE_HOLDER_TYPE( T, T*, false )
PYBIND11_MAKE_OPAQUE( std::shared_ptr<void> )
#endif

void bind_genesis_utils_text_string_2(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::text::indent(const std::string &, unsigned long)
    // file:genesis/utils/text/string.hpp line:601 function-signature: std::string
    // genesis::utils::text::indent(const std::string &, unsigned long)(const std::string &,
    // unsigned long) file:genesis/utils/text/string.hpp line:601
    M( "genesis::utils::text" )
        .def(
            "indent",
            ( std::string( * )( const std::string&, unsigned long ) ) &
                genesis::utils::text::indent,
            "Indent each line of `text` with  many spaces and return the result.\n\nC++: "
            "genesis::utils::text::indent(const std::string &, unsigned long) --> std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "num_spaces" )
        );

    // genesis::utils::text::replace_all(const std::string &, const std::string &, const std::string
    // &) file:genesis/utils/text/string.hpp line:610 function-signature: std::string
    // genesis::utils::text::replace_all(const std::string &, const std::string &, const std::string
    // &)(const std::string &, const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:610
    M( "genesis::utils::text" )
        .def(
            "replace_all",
            ( std::string( * )( const std::string&, const std::string&, const std::string& ) ) &
                genesis::utils::text::replace_all,
            "Return a copy of a string, where all occurrences of a search string\n are replaced by "
            "a replace string.\n\nC++: genesis::utils::text::replace_all(const std::string &, "
            "const std::string &, const std::string &) --> std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "search" ),
            pybind11::arg( "replace" )
        );

    // genesis::utils::text::remove_all(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:619 function-signature: std::string
    // genesis::utils::text::remove_all(const std::string &, const std::string &)(const std::string
    // &, const std::string &) file:genesis/utils/text/string.hpp line:619
    M( "genesis::utils::text" )
        .def(
            "remove_all",
            ( std::string( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::remove_all,
            "Return a copy of a string, where all occurrences of a search string are "
            "removed.\n\nC++: genesis::utils::text::remove_all(const std::string &, const "
            "std::string &) --> std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "search" )
        );

    // genesis::utils::text::replace_all_chars(const std::string &, const std::string &, char)
    // file:genesis/utils/text/string.hpp line:627 function-signature: std::string
    // genesis::utils::text::replace_all_chars(const std::string &, const std::string &, char)(const
    // std::string &, const std::string &, char) file:genesis/utils/text/string.hpp line:627
    M( "genesis::utils::text" )
        .def(
            "replace_all_chars",
            ( std::string( * )( const std::string&, const std::string&, char ) ) &
                genesis::utils::text::replace_all_chars,
            "Replace all occurrences of the  in  by the  char.\n\nC++: "
            "genesis::utils::text::replace_all_chars(const std::string &, const std::string &, "
            "char) --> std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "search_chars" ),
            pybind11::arg( "replace" )
        );

    // genesis::utils::text::remove_all_chars(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:655 function-signature: std::string
    // genesis::utils::text::remove_all_chars(const std::string &, const std::string &)(const
    // std::string &, const std::string &) file:genesis/utils/text/string.hpp line:655
    M( "genesis::utils::text" )
        .def(
            "remove_all_chars",
            ( std::string( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::remove_all_chars,
            "Remove all occurrences of the  in \n \n\nC++: "
            "genesis::utils::text::remove_all_chars(const std::string &, const std::string &) --> "
            "std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "search_chars" )
        );

    // genesis::utils::text::remove_all_non_alnum(const std::string &)
    // file:genesis/utils/text/string.hpp line:676 function-signature: std::string
    // genesis::utils::text::remove_all_non_alnum(const std::string &)(const std::string &)
    // file:genesis/utils/text/string.hpp line:676
    M( "genesis::utils::text" )
        .def(
            "remove_all_non_alnum",
            ( std::string( * )( const std::string& ) ) & genesis::utils::text::remove_all_non_alnum,
            "Remove all non-alphanumerical characters from a string.\n\nC++: "
            "genesis::utils::text::remove_all_non_alnum(const std::string &) --> std::string",
            pybind11::arg( "text" )
        );

    // genesis::utils::text::trim_right(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:682 function-signature: std::string
    // genesis::utils::text::trim_right(const std::string &, const std::string &)(const std::string
    // &, const std::string &) file:genesis/utils/text/string.hpp line:682
    M( "genesis::utils::text" )
        .def(
            "trim_right",
            []( const std::string& a0 ) -> std::string {
                return genesis::utils::text::trim_right( a0 );
            },
            "",
            pybind11::arg( "s" )
        );
    M( "genesis::utils::text" )
        .def(
            "trim_right",
            ( std::string( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::trim_right,
            "Return a copy of the input string, with left trimmed white spaces\n (or any other \n "
            "\n\nC++: genesis::utils::text::trim_right(const std::string &, const std::string &) "
            "--> std::string",
            pybind11::arg( "s" ),
            pybind11::arg( "delimiters" )
        );

    // genesis::utils::text::trim_left(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:691 function-signature: std::string
    // genesis::utils::text::trim_left(const std::string &, const std::string &)(const std::string
    // &, const std::string &) file:genesis/utils/text/string.hpp line:691
    M( "genesis::utils::text" )
        .def(
            "trim_left",
            []( const std::string& a0 ) -> std::string {
                return genesis::utils::text::trim_left( a0 );
            },
            "",
            pybind11::arg( "s" )
        );
    M( "genesis::utils::text" )
        .def(
            "trim_left",
            ( std::string( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::trim_left,
            "Return a copy of the input string, with right trimmed white spaces\n (or any other \n "
            "\n\nC++: genesis::utils::text::trim_left(const std::string &, const std::string &) "
            "--> std::string",
            pybind11::arg( "s" ),
            pybind11::arg( "delimiters" )
        );

    // genesis::utils::text::trim(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:700 function-signature: std::string
    // genesis::utils::text::trim(const std::string &, const std::string &)(const std::string &,
    // const std::string &) file:genesis/utils/text/string.hpp line:700
    M( "genesis::utils::text" )
        .def(
            "trim",
            []( const std::string& a0 ) -> std::string { return genesis::utils::text::trim( a0 ); },
            "",
            pybind11::arg( "s" )
        );
    M( "genesis::utils::text" )
        .def(
            "trim",
            ( std::string( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::trim,
            "Return a copy of the input string, with trimmed white spaces\n (or any other \n "
            "\n\nC++: genesis::utils::text::trim(const std::string &, const std::string &) --> "
            "std::string",
            pybind11::arg( "s" ),
            pybind11::arg( "delimiters" )
        );

    // genesis::utils::text::to_lower(const std::string &) file:genesis/utils/text/string.hpp
    // line:712 function-signature: std::string genesis::utils::text::to_lower(const std::string
    // &)(const std::string &) file:genesis/utils/text/string.hpp line:712
    M( "genesis::utils::text" )
        .def(
            "to_lower",
            ( std::string( * )( const std::string& ) ) & genesis::utils::text::to_lower,
            "Return an all-lowercase copy of the given string, locale-aware.\n\nC++: "
            "genesis::utils::text::to_lower(const std::string &) --> std::string",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::to_lower_inplace(std::string &) file:genesis/utils/text/string.hpp
    // line:726 function-signature: void genesis::utils::text::to_lower_inplace(std::string
    // &)(std::string &) file:genesis/utils/text/string.hpp line:726
    M( "genesis::utils::text" )
        .def(
            "to_lower_inplace",
            ( void ( * )( std::string& ) ) & genesis::utils::text::to_lower_inplace,
            "Turn the given string to all-lowercase, locale-aware.\n\nC++: "
            "genesis::utils::text::to_lower_inplace(std::string &) --> void",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::to_upper(const std::string &) file:genesis/utils/text/string.hpp
    // line:736 function-signature: std::string genesis::utils::text::to_upper(const std::string
    // &)(const std::string &) file:genesis/utils/text/string.hpp line:736
    M( "genesis::utils::text" )
        .def(
            "to_upper",
            ( std::string( * )( const std::string& ) ) & genesis::utils::text::to_upper,
            "Return an all-uppercase copy of the given string, locale-aware.\n\nC++: "
            "genesis::utils::text::to_upper(const std::string &) --> std::string",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::to_upper_inplace(std::string &) file:genesis/utils/text/string.hpp
    // line:748 function-signature: void genesis::utils::text::to_upper_inplace(std::string
    // &)(std::string &) file:genesis/utils/text/string.hpp line:748
    M( "genesis::utils::text" )
        .def(
            "to_upper_inplace",
            ( void ( * )( std::string& ) ) & genesis::utils::text::to_upper_inplace,
            "Turn the given string to all-uppercase, locale-aware.\n\nC++: "
            "genesis::utils::text::to_upper_inplace(std::string &) --> void",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::to_lower_ascii_inplace(std::string &)
    // file:genesis/utils/text/string.hpp line:758 function-signature: void
    // genesis::utils::text::to_lower_ascii_inplace(std::string &)(std::string &)
    // file:genesis/utils/text/string.hpp line:758
    M( "genesis::utils::text" )
        .def(
            "to_lower_ascii_inplace",
            ( void ( * )( std::string& ) ) & genesis::utils::text::to_lower_ascii_inplace,
            "Turn the given string to all-lowercase, ASCII-only.\n\nC++: "
            "genesis::utils::text::to_lower_ascii_inplace(std::string &) --> void",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::to_lower_ascii(const std::string &) file:genesis/utils/text/string.hpp
    // line:763 function-signature: std::string genesis::utils::text::to_lower_ascii(const
    // std::string &)(const std::string &) file:genesis/utils/text/string.hpp line:763
    M( "genesis::utils::text" )
        .def(
            "to_lower_ascii",
            ( std::string( * )( const std::string& ) ) & genesis::utils::text::to_lower_ascii,
            "Return an all-lowercase copy of the given string, ASCII-only.\n\nC++: "
            "genesis::utils::text::to_lower_ascii(const std::string &) --> std::string",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::to_upper_ascii_inplace(std::string &)
    // file:genesis/utils/text/string.hpp line:770 function-signature: void
    // genesis::utils::text::to_upper_ascii_inplace(std::string &)(std::string &)
    // file:genesis/utils/text/string.hpp line:770
    M( "genesis::utils::text" )
        .def(
            "to_upper_ascii_inplace",
            ( void ( * )( std::string& ) ) & genesis::utils::text::to_upper_ascii_inplace,
            "Turn the given string to all-uppercase, ASCII-only, inline.\n\n Uses AVX2 if "
            "available.\n\nC++: genesis::utils::text::to_upper_ascii_inplace(std::string &) --> "
            "void",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::to_upper_ascii(const std::string &) file:genesis/utils/text/string.hpp
    // line:775 function-signature: std::string genesis::utils::text::to_upper_ascii(const
    // std::string &)(const std::string &) file:genesis/utils/text/string.hpp line:775
    M( "genesis::utils::text" )
        .def(
            "to_upper_ascii",
            ( std::string( * )( const std::string& ) ) & genesis::utils::text::to_upper_ascii,
            "Return an all-uppercase copy of the given string, ASCII-only.\n\nC++: "
            "genesis::utils::text::to_upper_ascii(const std::string &) --> std::string",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::escape(const std::string &) file:genesis/utils/text/string.hpp line:788
    // function-signature: std::string genesis::utils::text::escape(const std::string &)(const
    // std::string &) file:genesis/utils/text/string.hpp line:788
    M( "genesis::utils::text" )
        .def(
            "escape",
            ( std::string( * )( const std::string& ) ) & genesis::utils::text::escape,
            "Return a string where special chars are replaces by their escape sequence.\n\n All "
            "new lines are transformed into either \\r or \\n, tabs into \\t.\n Double quotation "
            "marks are preceeded by a backslash, also the backslash itself will be escaped,\n so "
            "that `\"` becomes `\\\"` and `\\` becomes `\\`.\n\nC++: "
            "genesis::utils::text::escape(const std::string &) --> std::string",
            pybind11::arg( "text" )
        );

    // genesis::utils::text::deescape(const std::string &) file:genesis/utils/text/string.hpp
    // line:801 function-signature: std::string genesis::utils::text::deescape(const std::string
    // &)(const std::string &) file:genesis/utils/text/string.hpp line:801
    M( "genesis::utils::text" )
        .def(
            "deescape",
            ( std::string( * )( const std::string& ) ) & genesis::utils::text::deescape,
            "Return a string where backslash-escaped characters are transformed into\n their "
            "respective string form.\n\n All occurrences of `backslash + char` in the string are "
            "de-escaped. That is, all ``, `` and\n `` are turned into their respective control "
            "sequences, while all other chars folloing a\n backslash are translated into the char "
            "itself (so that e.g., quotation marks or backslashes\n themself can be escaped).\n\n "
            "Also see deescape( char c ).\n\nC++: genesis::utils::text::deescape(const std::string "
            "&) --> std::string",
            pybind11::arg( "text" )
        );

    // genesis::utils::text::deescape(char) file:genesis/utils/text/string.hpp line:811
    // function-signature: char genesis::utils::text::deescape(char)(char)
    // file:genesis/utils/text/string.hpp line:811
    M( "genesis::utils::text" )
        .def(
            "deescape",
            ( char ( * )( char ) ) & genesis::utils::text::deescape,
            "Return the de-escaped char for a backslash-escaped char.\n\n The function takes the "
            "char that follows a backslash in an escaped string and returns its\n de-escaped char. "
            "That is, `n` is turned into a new line (``), `t` is turned into a tab (``)\n and `r` "
            "is turned into a carrier return (``). All other chars (e.g., quotation marks or\n the "
            "backslash itself) are simply returned as-is.\n\nC++: "
            "genesis::utils::text::deescape(char) --> char",
            pybind11::arg( "c" )
        );

    // genesis::utils::text::repeat(const std::string &, unsigned long)
    // file:genesis/utils/text/string.hpp line:820 function-signature: std::string
    // genesis::utils::text::repeat(const std::string &, unsigned long)(const std::string &,
    // unsigned long) file:genesis/utils/text/string.hpp line:820
    M( "genesis::utils::text" )
        .def(
            "repeat",
            ( std::string( * )( const std::string&, unsigned long ) ) &
                genesis::utils::text::repeat,
            "Take a string and repeat it a given number of times.\n\nC++: "
            "genesis::utils::text::repeat(const std::string &, unsigned long) --> std::string",
            pybind11::arg( "word" ),
            pybind11::arg( "times" )
        );

    // genesis::utils::text::to_string_leading_zeros(unsigned long, unsigned long)
    // file:genesis/utils/text/string.hpp line:828 function-signature: std::string
    // genesis::utils::text::to_string_leading_zeros(unsigned long, unsigned long)(unsigned long,
    // unsigned long) file:genesis/utils/text/string.hpp line:828
    M( "genesis::utils::text" )
        .def(
            "to_string_leading_zeros",
            []( unsigned long const& a0 ) -> std::string {
                return genesis::utils::text::to_string_leading_zeros( a0 );
            },
            "",
            pybind11::arg( "value" )
        );
    M( "genesis::utils::text" )
        .def(
            "to_string_leading_zeros",
            ( std::string( * )( unsigned long, unsigned long ) ) &
                genesis::utils::text::to_string_leading_zeros,
            "Return a string representation of a `size_t`  with a fixed length, that is,\n by "
            "adding leading zeros.\n\n If  is already longer than  the result will also be "
            "longer.\n\nC++: genesis::utils::text::to_string_leading_zeros(unsigned long, unsigned "
            "long) --> std::string",
            pybind11::arg( "value" ),
            pybind11::arg( "length" )
        );

    // genesis::utils::text::to_string_precise(double, int) file:genesis/utils/text/string.hpp
    // line:841 function-signature: std::string genesis::utils::text::to_string_precise(double,
    // int)(double, int) file:genesis/utils/text/string.hpp line:841
    M( "genesis::utils::text" )
        .def(
            "to_string_precise",
            []( double const& a0 ) -> std::string {
                return genesis::utils::text::to_string_precise( a0 );
            },
            "",
            pybind11::arg( "value" )
        );
    M( "genesis::utils::text" )
        .def(
            "to_string_precise",
            ( std::string( * )( double, int ) ) & genesis::utils::text::to_string_precise,
            "Return a precise string representation of the input value, using the provided "
            "precision\n value (determining its decimal places).\n\n This function rounds the "
            "value to the given precision, and then returns its string representation\n with "
            "possible trailing zeros. Thus, it uses fixed precision. This is useful for e.g., "
            "output\n in a table format.\n\n For a version of this function that truncates "
            "trailing zeros, see to_string_rounded().\n Also, see to_string() if you simply want "
            "to output a string representation of a double.\n\nC++: "
            "genesis::utils::text::to_string_precise(double, int) --> std::string",
            pybind11::arg( "value" ),
            pybind11::arg( "precision" )
        );

    // genesis::utils::text::to_string_rounded(double, int) file:genesis/utils/text/string.hpp
    // line:854 function-signature: std::string genesis::utils::text::to_string_rounded(double,
    // int)(double, int) file:genesis/utils/text/string.hpp line:854
    M( "genesis::utils::text" )
        .def(
            "to_string_rounded",
            []( double const& a0 ) -> std::string {
                return genesis::utils::text::to_string_rounded( a0 );
            },
            "",
            pybind11::arg( "value" )
        );
    M( "genesis::utils::text" )
        .def(
            "to_string_rounded",
            ( std::string( * )( double, int ) ) & genesis::utils::text::to_string_rounded,
            "Return a string representation of the input value, using the provided precision "
            "value\n (determining its decimal places) to round, and truncate trailing zeros.\n\n "
            "This function rounds the value to the given precision, and then returns its string "
            "representation\n without trailing zeros. This is useful for output that keeps a "
            "certain amount of significant\n decimal digits, while making the output as short as "
            "possible.\n\n If you want to round, but also keep trailing zeros, see "
            "to_string_precise().\n Also, see to_string() if you simply want to output a string "
            "representation of a double.\n\nC++: genesis::utils::text::to_string_rounded(double, "
            "int) --> std::string",
            pybind11::arg( "value" ),
            pybind11::arg( "precision" )
        );
}
