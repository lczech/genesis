#include <functional>
#include <genesis/utils/text/string.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

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

void bind_genesis_utils_text_string_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::text::compare_natural(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:226 function-signature: int
    // genesis::utils::text::compare_natural(const std::string &, const std::string &)(const
    // std::string &, const std::string &) file:genesis/utils/text/string.hpp line:226
    M( "genesis::utils::text" )
        .def(
            "compare_natural",
            ( int ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::compare_natural,
            "Compare two strings with natural human sorting, that is \"A1\", \"A2\", \"A100\", "
            "instead of the\n standard sort by ASCII value \"A1\", \"A100\", \"A2\".\n\n Returns "
            "negavie if  <  `0` if they are equal, and positive if  > \n \n\nC++: "
            "genesis::utils::text::compare_natural(const std::string &, const std::string &) --> "
            "int",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );

    // genesis::utils::text::head(const std::string &, unsigned long)
    // file:genesis/utils/text/string.hpp line:278 function-signature: std::string
    // genesis::utils::text::head(const std::string &, unsigned long)(const std::string &, unsigned
    // long) file:genesis/utils/text/string.hpp line:278
    M( "genesis::utils::text" )
        .def(
            "head",
            []( const std::string& a0 ) -> std::string { return genesis::utils::text::head( a0 ); },
            "",
            pybind11::arg( "text" )
        );
    M( "genesis::utils::text" )
        .def(
            "head",
            ( std::string( * )( const std::string&, unsigned long ) ) & genesis::utils::text::head,
            "Return the first lines of the text.\n\nC++: genesis::utils::text::head(const "
            "std::string &, unsigned long) --> std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "lines" )
        );

    // genesis::utils::text::tail(const std::string &, unsigned long)
    // file:genesis/utils/text/string.hpp line:283 function-signature: std::string
    // genesis::utils::text::tail(const std::string &, unsigned long)(const std::string &, unsigned
    // long) file:genesis/utils/text/string.hpp line:283
    M( "genesis::utils::text" )
        .def(
            "tail",
            []( const std::string& a0 ) -> std::string { return genesis::utils::text::tail( a0 ); },
            "",
            pybind11::arg( "text" )
        );
    M( "genesis::utils::text" )
        .def(
            "tail",
            ( std::string( * )( const std::string&, unsigned long ) ) & genesis::utils::text::tail,
            "Return the last lines of the text.\n\nC++: genesis::utils::text::tail(const "
            "std::string &, unsigned long) --> std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "lines" )
        );

    // genesis::utils::text::count_substring_occurrences(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:292 function-signature: unsigned long
    // genesis::utils::text::count_substring_occurrences(const std::string &, const std::string
    // &)(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:292
    M( "genesis::utils::text" )
        .def(
            "count_substring_occurrences",
            ( unsigned long ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::count_substring_occurrences,
            "Return the number of (possibly overlapping) occurrences of a substring in a "
            "string.\n\nC++: genesis::utils::text::count_substring_occurrences(const std::string "
            "&, const std::string &) --> unsigned long",
            pybind11::arg( "str" ),
            pybind11::arg( "sub" )
        );

    // genesis::utils::text::split_range_list(const std::string &)
    // file:genesis/utils/text/string.hpp line:300 function-signature: class std::vector<unsigned
    // long> genesis::utils::text::split_range_list(const std::string &)(const std::string &)
    // file:genesis/utils/text/string.hpp line:300
    M( "genesis::utils::text" )
        .def(
            "split_range_list",
            ( class std::vector<unsigned long>( * )( const std::string& ) ) &
                genesis::utils::text::split_range_list,
            "Split a string containing positive interger numbers into its parts and resolve "
            "ranges.\n\n For example, the string `1, 3, 5-7, 10` results in a vector of all listed "
            "numbers and the\n ranges in between, that is `1, 3, 5, 6, 7, 10`. Whitespace is "
            "ignored; the range is sorted.\n\nC++: genesis::utils::text::split_range_list(const "
            "std::string &) --> class std::vector<unsigned long>",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::split_into_vector(const std::string &, const class
    // std::function<unsigned long (const std::string &, unsigned long)> &, const unsigned long,
    // const bool, class std::vector<std::string > &) file:genesis/utils/text/string.hpp line:313
    // function-signature: void
    // genesis::utils::text::split_into_vector<std::string,std::string>(const std::string &, const
    // class std::function<unsigned long (const std::string &, unsigned long)> &, const unsigned
    // long, const bool, class std::vector<std::string > &)(const std::string &, const class
    // std::function<unsigned long (const std::string &, unsigned long)> &, const unsigned long,
    // const bool, class std::vector<std::string > &) file:genesis/utils/text/string.hpp line:313
    M( "genesis::utils::text" )
        .def(
            "split_into_vector",
            ( void ( * )( const std::string&, const class std::function<unsigned long( const std::string&, unsigned long )>&, const unsigned long, const bool, class std::vector<std::string >& )
            ) & genesis::utils::text::split_into_vector<std::string, std::string>,
            "C++: genesis::utils::text::split_into_vector(const std::string &, const class "
            "std::function<unsigned long (const std::string &, unsigned long)> &, const unsigned "
            "long, const bool, class std::vector<std::string > &) --> void",
            pybind11::arg( "str" ),
            pybind11::arg( "find_next_delim_pos" ),
            pybind11::arg( "advance_by" ),
            pybind11::arg( "trim_empty" ),
            pybind11::arg( "target" )
        );

    // genesis::utils::text::split_into_vector(const class std::basic_string_view<char> &, const
    // class std::function<unsigned long (const class std::basic_string_view<char> &, unsigned
    // long)> &, const unsigned long, const bool, class std::vector<class
    // std::basic_string_view<char> > &) file:genesis/utils/text/string.hpp line:313
    // function-signature: void
    // genesis::utils::text::split_into_vector<std::basic_string_view<char>,std::basic_string_view<char>>(const
    // class std::basic_string_view<char> &, const class std::function<unsigned long (const class
    // std::basic_string_view<char> &, unsigned long)> &, const unsigned long, const bool, class
    // std::vector<class std::basic_string_view<char> > &)(const class std::basic_string_view<char>
    // &, const class std::function<unsigned long (const class std::basic_string_view<char> &,
    // unsigned long)> &, const unsigned long, const bool, class std::vector<class
    // std::basic_string_view<char> > &) file:genesis/utils/text/string.hpp line:313
    M( "genesis::utils::text" )
        .def(
            "split_into_vector",
            ( void ( * )( const class std::basic_string_view<char>&, const class std::function<unsigned long( const class std::basic_string_view<char>&, unsigned long )>&, const unsigned long, const bool, class std::vector<class std::basic_string_view<char> >& )
            ) & genesis::utils::text::
                    split_into_vector<std::basic_string_view<char>, std::basic_string_view<char>>,
            "C++: genesis::utils::text::split_into_vector(const class std::basic_string_view<char> "
            "&, const class std::function<unsigned long (const class std::basic_string_view<char> "
            "&, unsigned long)> &, const unsigned long, const bool, class std::vector<class "
            "std::basic_string_view<char> > &) --> void",
            pybind11::arg( "str" ),
            pybind11::arg( "find_next_delim_pos" ),
            pybind11::arg( "advance_by" ),
            pybind11::arg( "trim_empty" ),
            pybind11::arg( "target" )
        );

    // genesis::utils::text::split_into_vector_at_delimiter(const std::string &, const std::string
    // &, const bool, class std::vector<std::string > &) file:genesis/utils/text/string.hpp line:422
    // function-signature: void
    // genesis::utils::text::split_into_vector_at_delimiter<std::string,std::string>(const
    // std::string &, const std::string &, const bool, class std::vector<std::string > &)(const
    // std::string &, const std::string &, const bool, class std::vector<std::string > &)
    // file:genesis/utils/text/string.hpp line:422
    M( "genesis::utils::text" )
        .def(
            "split_into_vector_at_delimiter",
            ( void ( * )( const std::string&, const std::string&, const bool, class std::vector<std::string >& )
            ) & genesis::utils::text::split_into_vector_at_delimiter<std::string, std::string>,
            "C++: genesis::utils::text::split_into_vector_at_delimiter(const std::string &, const "
            "std::string &, const bool, class std::vector<std::string > &) --> void",
            pybind11::arg( "str" ),
            pybind11::arg( "delimiter" ),
            pybind11::arg( "trim_empty" ),
            pybind11::arg( "target" )
        );

    // genesis::utils::text::split_into_vector_at_delimiter(const class std::basic_string_view<char>
    // &, const std::string &, const bool, class std::vector<class std::basic_string_view<char> > &)
    // file:genesis/utils/text/string.hpp line:422 function-signature: void
    // genesis::utils::text::split_into_vector_at_delimiter<std::basic_string_view<char>,std::basic_string_view<char>>(const
    // class std::basic_string_view<char> &, const std::string &, const bool, class
    // std::vector<class std::basic_string_view<char> > &)(const class std::basic_string_view<char>
    // &, const std::string &, const bool, class std::vector<class std::basic_string_view<char> > &)
    // file:genesis/utils/text/string.hpp line:422
    M( "genesis::utils::text" )
        .def(
            "split_into_vector_at_delimiter",
            ( void ( * )( const class std::basic_string_view<char>&, const std::string&, const bool, class std::vector<class std::basic_string_view<char> >& )
            ) &
                genesis::utils::text::split_into_vector_at_delimiter<
                    std::basic_string_view<char>,
                    std::basic_string_view<char>>,
            "C++: genesis::utils::text::split_into_vector_at_delimiter(const class "
            "std::basic_string_view<char> &, const std::string &, const bool, class "
            "std::vector<class std::basic_string_view<char> > &) --> void",
            pybind11::arg( "str" ),
            pybind11::arg( "delimiter" ),
            pybind11::arg( "trim_empty" ),
            pybind11::arg( "target" )
        );

    // genesis::utils::text::split_at(const std::string &, const std::string &, const bool)
    // file:genesis/utils/text/string.hpp line:477 function-signature: class std::vector<std::string
    // > genesis::utils::text::split_at(const std::string &, const std::string &, const bool)(const
    // std::string &, const std::string &, const bool) file:genesis/utils/text/string.hpp line:477
    M( "genesis::utils::text" )
        .def(
            "split_at",
            []( const std::string& a0, const std::string& a1 ) -> std::vector<std::string > {
                return genesis::utils::text::split_at( a0, a1 );
            },
            "",
            pybind11::arg( "str" ),
            pybind11::arg( "delimiter" )
        );
    M( "genesis::utils::text" )
        .def(
            "split_at",
            ( class std::vector<std::string >( * )(
                const std::string&, const std::string&, const bool
            ) ) &
                genesis::utils::text::split_at,
            "Split a  into parts, given a  string.\n\n The  is split where the whole  string is "
            "found, and returned as a vector\n of strings. If `trim_empty` is set, empty strings "
            "resulting from adjacent delimiters are\n excluded from the output.\n\nC++: "
            "genesis::utils::text::split_at(const std::string &, const std::string &, const bool) "
            "--> class std::vector<std::string >",
            pybind11::arg( "str" ),
            pybind11::arg( "delimiter" ),
            pybind11::arg( "trim_empty" )
        );

    // genesis::utils::text::split_at(const std::string &, class std::vector<std::string > &, const
    // std::string &, const bool) file:genesis/utils/text/string.hpp line:503 function-signature:
    // void genesis::utils::text::split_at(const std::string &, class std::vector<std::string > &,
    // const std::string &, const bool)(const std::string &, class std::vector<std::string > &,
    // const std::string &, const bool) file:genesis/utils/text/string.hpp line:503
    M( "genesis::utils::text" )
        .def(
            "split_at",
            []( const std::string& a0, class std::vector<std::string >& a1, const std::string& a2
            ) -> void { return genesis::utils::text::split_at( a0, a1, a2 ); },
            "",
            pybind11::arg( "str" ),
            pybind11::arg( "target" ),
            pybind11::arg( "delimiter" )
        );
    M( "genesis::utils::text" )
        .def(
            "split_at",
            ( void ( * )(
                const std::string&, class std::vector<std::string >&, const std::string&, const bool
            ) ) &
                genesis::utils::text::split_at,
            "C++: genesis::utils::text::split_at(const std::string &, class "
            "std::vector<std::string > &, const std::string &, const bool) --> void",
            pybind11::arg( "str" ),
            pybind11::arg( "target" ),
            pybind11::arg( "delimiter" ),
            pybind11::arg( "trim_empty" )
        );

    // genesis::utils::text::split_view_at(const class std::basic_string_view<char> &, const
    // std::string &, const bool) file:genesis/utils/text/string.hpp line:531 function-signature:
    // class std::vector<class std::basic_string_view<char> >
    // genesis::utils::text::split_view_at(const class std::basic_string_view<char> &, const
    // std::string &, const bool)(const class std::basic_string_view<char> &, const std::string &,
    // const bool) file:genesis/utils/text/string.hpp line:531
    M( "genesis::utils::text" )
        .def(
            "split_view_at",
            []( const class std::basic_string_view<char>& a0,
                const std::string& a1 ) -> std::vector<class std::basic_string_view<char> > {
                return genesis::utils::text::split_view_at( a0, a1 );
            },
            "",
            pybind11::arg( "str" ),
            pybind11::arg( "delimiter" )
        );
    M( "genesis::utils::text" )
        .def(
            "split_view_at",
            ( class std::vector<class std::basic_string_view<char> >( * )(
                const class std::basic_string_view<char>&, const std::string&, const bool
            ) ) &
                genesis::utils::text::split_view_at,
            "C++: genesis::utils::text::split_view_at(const class std::basic_string_view<char> &, "
            "const std::string &, const bool) --> class std::vector<class "
            "std::basic_string_view<char> >",
            pybind11::arg( "str" ),
            pybind11::arg( "delimiter" ),
            pybind11::arg( "trim_empty" )
        );

    // genesis::utils::text::split_view_at(const class std::basic_string_view<char> &, class
    // std::vector<class std::basic_string_view<char> > &, const std::string &, const bool)
    // file:genesis/utils/text/string.hpp line:561 function-signature: void
    // genesis::utils::text::split_view_at(const class std::basic_string_view<char> &, class
    // std::vector<class std::basic_string_view<char> > &, const std::string &, const bool)(const
    // class std::basic_string_view<char> &, class std::vector<class std::basic_string_view<char> >
    // &, const std::string &, const bool) file:genesis/utils/text/string.hpp line:561
    M( "genesis::utils::text" )
        .def(
            "split_view_at",
            []( const class std::basic_string_view<char>& a0,
                class std::vector<class std::basic_string_view<char> >& a1,
                const std::string& a2 ) -> void {
                return genesis::utils::text::split_view_at( a0, a1, a2 );
            },
            "",
            pybind11::arg( "str" ),
            pybind11::arg( "target" ),
            pybind11::arg( "delimiter" )
        );
    M( "genesis::utils::text" )
        .def(
            "split_view_at",
            ( void ( * )(
                const class std::basic_string_view<char>&,
                class std::vector<class std::basic_string_view<char> >&,
                const std::string&,
                const bool
            ) ) &
                genesis::utils::text::split_view_at,
            "C++: genesis::utils::text::split_view_at(const class std::basic_string_view<char> &, "
            "class std::vector<class std::basic_string_view<char> > &, const std::string &, const "
            "bool) --> void",
            pybind11::arg( "str" ),
            pybind11::arg( "target" ),
            pybind11::arg( "delimiter" ),
            pybind11::arg( "trim_empty" )
        );

    // genesis::utils::text::wrap(const std::string &, unsigned long)
    // file:genesis/utils/text/string.hpp line:581 function-signature: std::string
    // genesis::utils::text::wrap(const std::string &, unsigned long)(const std::string &, unsigned
    // long) file:genesis/utils/text/string.hpp line:581
    M( "genesis::utils::text" )
        .def(
            "wrap",
            []( const std::string& a0 ) -> std::string { return genesis::utils::text::wrap( a0 ); },
            "",
            pybind11::arg( "text" )
        );
    M( "genesis::utils::text" )
        .def(
            "wrap",
            ( std::string( * )( const std::string&, unsigned long ) ) & genesis::utils::text::wrap,
            "Wrap a  at a given \n \n\nC++: genesis::utils::text::wrap(const std::string &, "
            "unsigned long) --> std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "line_length" )
        );

    // genesis::utils::text::indent(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:593 function-signature: std::string
    // genesis::utils::text::indent(const std::string &, const std::string &)(const std::string &,
    // const std::string &) file:genesis/utils/text/string.hpp line:593
    M( "genesis::utils::text" )
        .def(
            "indent",
            []( const std::string& a0 ) -> std::string {
                return genesis::utils::text::indent( a0 );
            },
            "",
            pybind11::arg( "text" )
        );
    M( "genesis::utils::text" )
        .def(
            "indent",
            ( std::string( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::indent,
            "Indent each line of `text` with `indentation` and return the result.\n\n By default, "
            "four spaces are used to indent. Whether the text ends with a new line or not is not\n "
            "changed. Any trailing indentation chars are trimmed, in order to not have trailing "
            "whitespaces\n in the result (except for the new line, if the text ends in "
            "one).\n\nC++: genesis::utils::text::indent(const std::string &, const std::string &) "
            "--> std::string",
            pybind11::arg( "text" ),
            pybind11::arg( "indentation" )
        );
}
