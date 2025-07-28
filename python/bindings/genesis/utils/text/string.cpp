#include <genesis/utils/text/string.hpp>
#include <iterator>
#include <memory>
#include <string>
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

void bind_genesis_utils_text_string(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::text::contains_ci(const class std::vector<std::string > &, const std::string
    // &) file:genesis/utils/text/string.hpp line:62 function-signature: bool
    // genesis::utils::text::contains_ci(const class std::vector<std::string > &, const std::string
    // &)(const class std::vector<std::string > &, const std::string &)
    // file:genesis/utils/text/string.hpp line:62
    M( "genesis::utils::text" )
        .def(
            "contains_ci",
            ( bool ( * )( const class std::vector<std::string >&, const std::string& ) ) &
                genesis::utils::text::contains_ci,
            "Return whether a vector of strings contains a given string, case insensitive.\n\nC++: "
            "genesis::utils::text::contains_ci(const class std::vector<std::string > &, const "
            "std::string &) --> bool",
            pybind11::arg( "haystack" ),
            pybind11::arg( "needle" )
        );

    // genesis::utils::text::contains_ci_alnum(const class std::vector<std::string > &, const
    // std::string &) file:genesis/utils/text/string.hpp line:68 function-signature: bool
    // genesis::utils::text::contains_ci_alnum(const class std::vector<std::string > &, const
    // std::string &)(const class std::vector<std::string > &, const std::string &)
    // file:genesis/utils/text/string.hpp line:68
    M( "genesis::utils::text" )
        .def(
            "contains_ci_alnum",
            ( bool ( * )( const class std::vector<std::string >&, const std::string& ) ) &
                genesis::utils::text::contains_ci_alnum,
            "Return whether a vector of strings contains a given string, case insensitive,\n and "
            "ignoring all non-alphanumerical characters.\n\nC++: "
            "genesis::utils::text::contains_ci_alnum(const class std::vector<std::string > &, "
            "const std::string &) --> bool",
            pybind11::arg( "haystack" ),
            pybind11::arg( "needle" )
        );

    // genesis::utils::text::strcasecmp(const char *, const char *)
    // file:genesis/utils/text/string.hpp line:80 function-signature: int
    // genesis::utils::text::strcasecmp(const char *, const char *)(const char *, const char *)
    // file:genesis/utils/text/string.hpp line:80
    M( "genesis::utils::text" )
        .def(
            "strcasecmp",
            ( int ( * )( const char*, const char* ) ) & genesis::utils::text::strcasecmp,
            "Compares two strings, ignoring case differences.\n\n Compares the null-terminated "
            "strings  and  returning -1, 0, or 1\n if  is lexicographically less than, equal to, "
            "or greater than  over those characters,\n respectively.\n\n This is equivalent to the "
            "POSIX function of the same name, found in `strings.h`,\n but included here for "
            "portability.\n\nC++: genesis::utils::text::strcasecmp(const char *, const char *) --> "
            "int",
            pybind11::arg( "s1" ),
            pybind11::arg( "s2" )
        );

    // genesis::utils::text::strncasecmp(const char *, const char *, unsigned long)
    // file:genesis/utils/text/string.hpp line:92 function-signature: int
    // genesis::utils::text::strncasecmp(const char *, const char *, unsigned long)(const char *,
    // const char *, unsigned long) file:genesis/utils/text/string.hpp line:92
    M( "genesis::utils::text" )
        .def(
            "strncasecmp",
            ( int ( * )( const char*, const char*, unsigned long ) ) &
                genesis::utils::text::strncasecmp,
            "Compares up to  chars of two strings, ignoring case differences.\n\n Compares up to  "
            "chars of the null-terminated strings  and  returning -1, 0, or 1\n if  is "
            "lexicographically less than, equal to, or greater than  over those characters,\n "
            "respectively.\n\n This is equivalent to the POSIX function of the same name, found in "
            "`strings.h`,\n but included here for portability.\n\nC++: "
            "genesis::utils::text::strncasecmp(const char *, const char *, unsigned long) --> int",
            pybind11::arg( "s1" ),
            pybind11::arg( "s2" ),
            pybind11::arg( "n" )
        );

    // genesis::utils::text::equals_ci(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:97 function-signature: bool
    // genesis::utils::text::equals_ci(const std::string &, const std::string &)(const std::string
    // &, const std::string &) file:genesis/utils/text/string.hpp line:97
    M( "genesis::utils::text" )
        .def(
            "equals_ci",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::equals_ci,
            "Compare two strings, case insensitive.\n\nC++: genesis::utils::text::equals_ci(const "
            "std::string &, const std::string &) --> bool",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );

    // genesis::utils::text::equals_ci_alnum(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:102 function-signature: bool
    // genesis::utils::text::equals_ci_alnum(const std::string &, const std::string &)(const
    // std::string &, const std::string &) file:genesis/utils/text/string.hpp line:102
    M( "genesis::utils::text" )
        .def(
            "equals_ci_alnum",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::equals_ci_alnum,
            "Compare two strings, case insensitive, and ignoring all non-alphanumerical "
            "characters.\n\nC++: genesis::utils::text::equals_ci_alnum(const std::string &, const "
            "std::string &) --> bool",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );

    // genesis::utils::text::starts_with(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:107 function-signature: bool
    // genesis::utils::text::starts_with(const std::string &, const std::string &)(const std::string
    // &, const std::string &) file:genesis/utils/text/string.hpp line:107
    M( "genesis::utils::text" )
        .def(
            "starts_with",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::starts_with,
            "Return whether a string starts with another string, i.e., check for a \n \n\nC++: "
            "genesis::utils::text::starts_with(const std::string &, const std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "prefix" )
        );

    // genesis::utils::text::starts_with(const std::string &, const std::string &, std::string &)
    // file:genesis/utils/text/string.hpp line:116 function-signature: bool
    // genesis::utils::text::starts_with(const std::string &, const std::string &, std::string
    // &)(const std::string &, const std::string &, std::string &)
    // file:genesis/utils/text/string.hpp line:116
    M( "genesis::utils::text" )
        .def(
            "starts_with",
            ( bool ( * )( const std::string&, const std::string&, std::string& ) ) &
                genesis::utils::text::starts_with,
            "Return whether a string starts with another string, i.e., check for a \n\n This "
            "version of the function then stores the  of the matched \n containing all remaining "
            "characters that did not match the  pattern.\n If there is no match, the  is left "
            "unchanged.\n\nC++: genesis::utils::text::starts_with(const std::string &, const "
            "std::string &, std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "suffix" )
        );

    // genesis::utils::text::starts_with_ci(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:122 function-signature: bool
    // genesis::utils::text::starts_with_ci(const std::string &, const std::string &)(const
    // std::string &, const std::string &) file:genesis/utils/text/string.hpp line:122
    M( "genesis::utils::text" )
        .def(
            "starts_with_ci",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::starts_with_ci,
            "Return whether a string starts with another string, i.e., check for a \n case "
            "insensitive.\n\nC++: genesis::utils::text::starts_with_ci(const std::string &, const "
            "std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "prefix" )
        );

    // genesis::utils::text::starts_with_ci(const std::string &, const std::string &, std::string &)
    // file:genesis/utils/text/string.hpp line:130 function-signature: bool
    // genesis::utils::text::starts_with_ci(const std::string &, const std::string &, std::string
    // &)(const std::string &, const std::string &, std::string &)
    // file:genesis/utils/text/string.hpp line:130
    M( "genesis::utils::text" )
        .def(
            "starts_with_ci",
            ( bool ( * )( const std::string&, const std::string&, std::string& ) ) &
                genesis::utils::text::starts_with_ci,
            "Return whether a string starts with another string, i.e., check for a \n case "
            "insensitive.\n\n  std::string const&, std::string const&, std::string& )\n\nC++: "
            "genesis::utils::text::starts_with_ci(const std::string &, const std::string &, "
            "std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "suffix" )
        );

    // genesis::utils::text::starts_with_ci_alnum(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:138 function-signature: bool
    // genesis::utils::text::starts_with_ci_alnum(const std::string &, const std::string &)(const
    // std::string &, const std::string &) file:genesis/utils/text/string.hpp line:138
    M( "genesis::utils::text" )
        .def(
            "starts_with_ci_alnum",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::starts_with_ci_alnum,
            "Return whether a string starts with another string (prefix), comparing "
            "case-independent,\n and ignoring all non-alphanumerical characters.\n\n For example, "
            "`\"REF_CNT.S2-1.sorted:1\"` starts with `\"[ref-cnt]\"`, according to this "
            "function.\n\nC++: genesis::utils::text::starts_with_ci_alnum(const std::string &, "
            "const std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "prefix" )
        );

    // genesis::utils::text::starts_with_ci_alnum(const std::string &, const std::string &,
    // std::string &, bool) file:genesis/utils/text/string.hpp line:152 function-signature: bool
    // genesis::utils::text::starts_with_ci_alnum(const std::string &, const std::string &,
    // std::string &, bool)(const std::string &, const std::string &, std::string &, bool)
    // file:genesis/utils/text/string.hpp line:152
    M( "genesis::utils::text" )
        .def(
            "starts_with_ci_alnum",
            []( const std::string& a0, const std::string& a1, std::string& a2 ) -> bool {
                return genesis::utils::text::starts_with_ci_alnum( a0, a1, a2 );
            },
            "",
            pybind11::arg( "text" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "suffix" )
        );
    M( "genesis::utils::text" )
        .def(
            "starts_with_ci_alnum",
            ( bool ( * )( const std::string&, const std::string&, std::string&, bool ) ) &
                genesis::utils::text::starts_with_ci_alnum,
            "std::string const& , std::string const& )\n\n This version of the function then "
            "stores the  of the matched \n containing all remaining characters that did not match "
            "the  pattern.\n If there is no match, the  is left unchanged.\n\n If additionally  is "
            "set, all leading non-alphanumerical characters of the\n  are removed.\n For example, "
            "`\"REF_CNT.S2-1.sorted:1\"` starts with `\"[ref-cnt]\"`, and then contains the "
            "suffix\n `\".S2-1.sorted:1\"` - with  this is reduced to `\"S2-1.sorted:1\"` "
            "instead.\n\nC++: genesis::utils::text::starts_with_ci_alnum(const std::string &, "
            "const std::string &, std::string &, bool) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "suffix" ),
            pybind11::arg( "trim_suffix" )
        );

    // genesis::utils::text::ends_with(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:162 function-signature: bool
    // genesis::utils::text::ends_with(const std::string &, const std::string &)(const std::string
    // &, const std::string &) file:genesis/utils/text/string.hpp line:162
    M( "genesis::utils::text" )
        .def(
            "ends_with",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::ends_with,
            "Return whether a string ends with another string, i.e., check for a \n \n\nC++: "
            "genesis::utils::text::ends_with(const std::string &, const std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "suffix" )
        );

    // genesis::utils::text::ends_with(const std::string &, const std::string &, std::string &)
    // file:genesis/utils/text/string.hpp line:171 function-signature: bool
    // genesis::utils::text::ends_with(const std::string &, const std::string &, std::string
    // &)(const std::string &, const std::string &, std::string &)
    // file:genesis/utils/text/string.hpp line:171
    M( "genesis::utils::text" )
        .def(
            "ends_with",
            ( bool ( * )( const std::string&, const std::string&, std::string& ) ) &
                genesis::utils::text::ends_with,
            "Return whether a string ends with another string, i.e., check for a \n\n This version "
            "of the function then stores the  of the matched \n containing all leading characters "
            "that did not match the  pattern.\n If there is no match, the  is left "
            "unchanged.\n\nC++: genesis::utils::text::ends_with(const std::string &, const "
            "std::string &, std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "suffix" ),
            pybind11::arg( "prefix" )
        );

    // genesis::utils::text::ends_with_ci(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:177 function-signature: bool
    // genesis::utils::text::ends_with_ci(const std::string &, const std::string &)(const
    // std::string &, const std::string &) file:genesis/utils/text/string.hpp line:177
    M( "genesis::utils::text" )
        .def(
            "ends_with_ci",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::ends_with_ci,
            "Return whether a string ends with another string, i.e., check for a suffix,\n case "
            "insensitive.\n\nC++: genesis::utils::text::ends_with_ci(const std::string &, const "
            "std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "suffix" )
        );

    // genesis::utils::text::ends_with_ci(const std::string &, const std::string &, std::string &)
    // file:genesis/utils/text/string.hpp line:185 function-signature: bool
    // genesis::utils::text::ends_with_ci(const std::string &, const std::string &, std::string
    // &)(const std::string &, const std::string &, std::string &)
    // file:genesis/utils/text/string.hpp line:185
    M( "genesis::utils::text" )
        .def(
            "ends_with_ci",
            ( bool ( * )( const std::string&, const std::string&, std::string& ) ) &
                genesis::utils::text::ends_with_ci,
            "Return whether a string ends with another string, i.e., check for a suffix,\n case "
            "insensitive.\n\n  std::string const&, std::string const&, std::string& )\n\nC++: "
            "genesis::utils::text::ends_with_ci(const std::string &, const std::string &, "
            "std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "suffix" ),
            pybind11::arg( "prefix" )
        );

    // genesis::utils::text::ends_with_ci_alnum(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:193 function-signature: bool
    // genesis::utils::text::ends_with_ci_alnum(const std::string &, const std::string &)(const
    // std::string &, const std::string &) file:genesis/utils/text/string.hpp line:193
    M( "genesis::utils::text" )
        .def(
            "ends_with_ci_alnum",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::ends_with_ci_alnum,
            "Return whether a string ends with another string (suffix), comparing "
            "case-independent,\n and ignoring all non-alphanumerical characters.\n\n For example, "
            "`\"S2-1.sorted:1.REF_CNT\"` ends with `\"[ref-cnt]\"`, according to this "
            "function.\n\nC++: genesis::utils::text::ends_with_ci_alnum(const std::string &, const "
            "std::string &) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "suffix" )
        );

    // genesis::utils::text::ends_with_ci_alnum(const std::string &, const std::string &,
    // std::string &, bool) file:genesis/utils/text/string.hpp line:207 function-signature: bool
    // genesis::utils::text::ends_with_ci_alnum(const std::string &, const std::string &,
    // std::string &, bool)(const std::string &, const std::string &, std::string &, bool)
    // file:genesis/utils/text/string.hpp line:207
    M( "genesis::utils::text" )
        .def(
            "ends_with_ci_alnum",
            []( const std::string& a0, const std::string& a1, std::string& a2 ) -> bool {
                return genesis::utils::text::ends_with_ci_alnum( a0, a1, a2 );
            },
            "",
            pybind11::arg( "text" ),
            pybind11::arg( "suffix" ),
            pybind11::arg( "prefix" )
        );
    M( "genesis::utils::text" )
        .def(
            "ends_with_ci_alnum",
            ( bool ( * )( const std::string&, const std::string&, std::string&, bool ) ) &
                genesis::utils::text::ends_with_ci_alnum,
            "std::string const& , std::string const& )\n\n This version of the function then "
            "stores the  of the matched \n containing all leading characters that did not match "
            "the  pattern.\n If there is no match, the  is left unchanged.\n\n If additionally  is "
            "set, all trailing non-alphanumerical characters of the\n  are removed.\n For example, "
            "`\"S2-1.sorted:1.REF_CNT\"` ends with `\"[ref-cnt]\"`, and then contains the prefix\n "
            "`\"S2-1.sorted:1.\"` - with  this is reduced to `\"S2-1.sorted:1\"` instead.\n\nC++: "
            "genesis::utils::text::ends_with_ci_alnum(const std::string &, const std::string &, "
            "std::string &, bool) --> bool",
            pybind11::arg( "text" ),
            pybind11::arg( "suffix" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "trim_prefix" )
        );

    // genesis::utils::text::match_wildcards(const std::string &, const std::string &)
    // file:genesis/utils/text/string.hpp line:218 function-signature: bool
    // genesis::utils::text::match_wildcards(const std::string &, const std::string &)(const
    // std::string &, const std::string &) file:genesis/utils/text/string.hpp line:218
    M( "genesis::utils::text" )
        .def(
            "match_wildcards",
            ( bool ( * )( const std::string&, const std::string& ) ) &
                genesis::utils::text::match_wildcards,
            "Return whether a string is matched by a wildcard pattern containing `?` and `*`\n for "
            "single and mutliple (0 or more) wildcard characters, respectively.\n\nC++: "
            "genesis::utils::text::match_wildcards(const std::string &, const std::string &) --> "
            "bool",
            pybind11::arg( "str" ),
            pybind11::arg( "pattern" )
        );
}
