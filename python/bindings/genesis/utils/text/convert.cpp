#include <genesis/utils/text/convert.hpp>
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

void bind_genesis_utils_text_convert(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::text::convert_to_bool(const std::string &, bool &)
    // file:genesis/utils/text/convert.hpp line:187 function-signature: bool
    // genesis::utils::text::convert_to_bool(const std::string &, bool &)(const std::string &, bool
    // &) file:genesis/utils/text/convert.hpp line:187
    M( "genesis::utils::text" )
        .def(
            "convert_to_bool",
            ( bool ( * )( const std::string&, bool& ) ) & genesis::utils::text::convert_to_bool,
            "Convert a string to bool, store the result in \n and return whether the conversion as "
            "successful.\n\n  std::string const& )\n\nC++: "
            "genesis::utils::text::convert_to_bool(const std::string &, bool &) --> bool",
            pybind11::arg( "str" ),
            pybind11::arg( "result" )
        );

    // genesis::utils::text::convert_to_bool(const std::string &)
    // file:genesis/utils/text/convert.hpp line:195 function-signature: bool
    // genesis::utils::text::convert_to_bool(const std::string &)(const std::string &)
    // file:genesis/utils/text/convert.hpp line:195
    M( "genesis::utils::text" )
        .def(
            "convert_to_bool",
            ( bool ( * )( const std::string& ) ) & genesis::utils::text::convert_to_bool,
            "Convert a string to bool, return the result, and throw an exception when the\n "
            "conversion was not successful.\n\n  std::string const& )\n\nC++: "
            "genesis::utils::text::convert_to_bool(const std::string &) --> bool",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::is_convertible_to_bool(const std::string &)
    // file:genesis/utils/text/convert.hpp line:203 function-signature: bool
    // genesis::utils::text::is_convertible_to_bool(const std::string &)(const std::string &)
    // file:genesis/utils/text/convert.hpp line:203
    M( "genesis::utils::text" )
        .def(
            "is_convertible_to_bool",
            ( bool ( * )( const std::string& ) ) & genesis::utils::text::is_convertible_to_bool,
            "Return whether a string can be converted to bool.\n\n Strings that can be converted "
            "to bool with this function are `true`, `false`, `yes`, `no`, `on`,\n `off`, `1`, `0`, "
            "and the empty string (evaluated as `false`), with all text case insensitive.\n\nC++: "
            "genesis::utils::text::is_convertible_to_bool(const std::string &) --> bool",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::convert_to_bool_double(const std::string &, double &)
    // file:genesis/utils/text/convert.hpp line:247 function-signature: bool
    // genesis::utils::text::convert_to_bool_double(const std::string &, double &)(const std::string
    // &, double &) file:genesis/utils/text/convert.hpp line:247
    M( "genesis::utils::text" )
        .def(
            "convert_to_bool_double",
            ( bool ( * )( const std::string&, double& ) ) &
                genesis::utils::text::convert_to_bool_double,
            "Convert a string to bool, but store the result as a double in \n and return whether "
            "the conversion as successful.\n\n  std::string const& )\n\nC++: "
            "genesis::utils::text::convert_to_bool_double(const std::string &, double &) --> bool",
            pybind11::arg( "str" ),
            pybind11::arg( "result" )
        );

    // genesis::utils::text::convert_to_bool_double(const std::string &)
    // file:genesis/utils/text/convert.hpp line:255 function-signature: double
    // genesis::utils::text::convert_to_bool_double(const std::string &)(const std::string &)
    // file:genesis/utils/text/convert.hpp line:255
    M( "genesis::utils::text" )
        .def(
            "convert_to_bool_double",
            ( double ( * )( const std::string& ) ) & genesis::utils::text::convert_to_bool_double,
            "Convert a string to bool, but return it as a double,\n and throw an exception when "
            "the conversion was not successful.\n\n  std::string const& )\n\nC++: "
            "genesis::utils::text::convert_to_bool_double(const std::string &) --> double",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::is_convertible_to_bool_double(const std::string &)
    // file:genesis/utils/text/convert.hpp line:266 function-signature: bool
    // genesis::utils::text::is_convertible_to_bool_double(const std::string &)(const std::string &)
    // file:genesis/utils/text/convert.hpp line:266
    M( "genesis::utils::text" )
        .def(
            "is_convertible_to_bool_double",
            ( bool ( * )( const std::string& ) ) &
                genesis::utils::text::is_convertible_to_bool_double,
            "Return whether a string can be converted to bool (and stored as a double).\n\n This "
            "conversion is useful to work with dataframes that are all double,\n for example to "
            "work with our Generalized Linear Model implementation, see glm_fit().\n\n Strings "
            "that can be converted to bool with this function are `true`, `false`, `yes`, `no`, "
            "`on`,\n `off`, `1`, `0`, and the empty string (evaluated as `quiet_NaN`), with all "
            "text case insensitive.\n\nC++: "
            "genesis::utils::text::is_convertible_to_bool_double(const std::string &) --> bool",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::convert_to_double(const std::string &, double &)
    // file:genesis/utils/text/convert.hpp line:308 function-signature: bool
    // genesis::utils::text::convert_to_double(const std::string &, double &)(const std::string &,
    // double &) file:genesis/utils/text/convert.hpp line:308
    M( "genesis::utils::text" )
        .def(
            "convert_to_double",
            ( bool ( * )( const std::string&, double& ) ) & genesis::utils::text::convert_to_double,
            "Convert a string to double, store the result in \n and return whether the conversion "
            "as successful.\n\nC++: genesis::utils::text::convert_to_double(const std::string &, "
            "double &) --> bool",
            pybind11::arg( "str" ),
            pybind11::arg( "result" )
        );

    // genesis::utils::text::convert_to_double(const std::string &)
    // file:genesis/utils/text/convert.hpp line:314 function-signature: double
    // genesis::utils::text::convert_to_double(const std::string &)(const std::string &)
    // file:genesis/utils/text/convert.hpp line:314
    M( "genesis::utils::text" )
        .def(
            "convert_to_double",
            ( double ( * )( const std::string& ) ) & genesis::utils::text::convert_to_double,
            "Convert a string to double, return the result, and throw an exception when the\n "
            "conversion was not successful.\n\nC++: genesis::utils::text::convert_to_double(const "
            "std::string &) --> double",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::is_convertible_to_double(const std::string &)
    // file:genesis/utils/text/convert.hpp line:319 function-signature: bool
    // genesis::utils::text::is_convertible_to_double(const std::string &)(const std::string &)
    // file:genesis/utils/text/convert.hpp line:319
    M( "genesis::utils::text" )
        .def(
            "is_convertible_to_double",
            ( bool ( * )( const std::string& ) ) & genesis::utils::text::is_convertible_to_double,
            "Return whether a string can be converted to double.\n\nC++: "
            "genesis::utils::text::is_convertible_to_double(const std::string &) --> bool",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::convert_to_signed_integer(const std::string &, long long &)
    // file:genesis/utils/text/convert.hpp line:361 function-signature: bool
    // genesis::utils::text::convert_to_signed_integer(const std::string &, long long &)(const
    // std::string &, long long &) file:genesis/utils/text/convert.hpp line:361
    M( "genesis::utils::text" )
        .def(
            "convert_to_signed_integer",
            ( bool ( * )( const std::string&, long long& ) ) &
                genesis::utils::text::convert_to_signed_integer,
            "Convert a string to signed integer, store the result in \n and return whether the "
            "conversion as successful.\n\nC++: "
            "genesis::utils::text::convert_to_signed_integer(const std::string &, long long &) --> "
            "bool",
            pybind11::arg( "str" ),
            pybind11::arg( "result" )
        );

    // genesis::utils::text::convert_to_signed_integer(const std::string &)
    // file:genesis/utils/text/convert.hpp line:367 function-signature: long long
    // genesis::utils::text::convert_to_signed_integer(const std::string &)(const std::string &)
    // file:genesis/utils/text/convert.hpp line:367
    M( "genesis::utils::text" )
        .def(
            "convert_to_signed_integer",
            ( long long ( * )( const std::string& ) ) &
                genesis::utils::text::convert_to_signed_integer,
            "Convert a string to signed integer, return the result, and throw an exception when "
            "the\n conversion was not successful.\n\nC++: "
            "genesis::utils::text::convert_to_signed_integer(const std::string &) --> long long",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::is_convertible_to_signed_integer(const std::string &)
    // file:genesis/utils/text/convert.hpp line:372 function-signature: bool
    // genesis::utils::text::is_convertible_to_signed_integer(const std::string &)(const std::string
    // &) file:genesis/utils/text/convert.hpp line:372
    M( "genesis::utils::text" )
        .def(
            "is_convertible_to_signed_integer",
            ( bool ( * )( const std::string& ) ) &
                genesis::utils::text::is_convertible_to_signed_integer,
            "Return whether a string can be converted to signed integer.\n\nC++: "
            "genesis::utils::text::is_convertible_to_signed_integer(const std::string &) --> bool",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::convert_to_unsigned_integer(const std::string &, unsigned long long &)
    // file:genesis/utils/text/convert.hpp line:414 function-signature: bool
    // genesis::utils::text::convert_to_unsigned_integer(const std::string &, unsigned long long
    // &)(const std::string &, unsigned long long &) file:genesis/utils/text/convert.hpp line:414
    M( "genesis::utils::text" )
        .def(
            "convert_to_unsigned_integer",
            ( bool ( * )( const std::string&, unsigned long long& ) ) &
                genesis::utils::text::convert_to_unsigned_integer,
            "Convert a string to unsigned integer, store the result in \n and return whether the "
            "conversion as successful.\n\nC++: "
            "genesis::utils::text::convert_to_unsigned_integer(const std::string &, unsigned long "
            "long &) --> bool",
            pybind11::arg( "str" ),
            pybind11::arg( "result" )
        );

    // genesis::utils::text::convert_to_unsigned_integer(const std::string &)
    // file:genesis/utils/text/convert.hpp line:420 function-signature: unsigned long long
    // genesis::utils::text::convert_to_unsigned_integer(const std::string &)(const std::string &)
    // file:genesis/utils/text/convert.hpp line:420
    M( "genesis::utils::text" )
        .def(
            "convert_to_unsigned_integer",
            ( unsigned long long ( * )( const std::string& ) ) &
                genesis::utils::text::convert_to_unsigned_integer,
            "Convert a string to unsigned integer, return the result, and throw an exception when "
            "the\n conversion was not successful.\n\nC++: "
            "genesis::utils::text::convert_to_unsigned_integer(const std::string &) --> unsigned "
            "long long",
            pybind11::arg( "str" )
        );

    // genesis::utils::text::is_convertible_to_unsigned_integer(const std::string &)
    // file:genesis/utils/text/convert.hpp line:425 function-signature: bool
    // genesis::utils::text::is_convertible_to_unsigned_integer(const std::string &)(const
    // std::string &) file:genesis/utils/text/convert.hpp line:425
    M( "genesis::utils::text" )
        .def(
            "is_convertible_to_unsigned_integer",
            ( bool ( * )( const std::string& ) ) &
                genesis::utils::text::is_convertible_to_unsigned_integer,
            "Return whether a string can be converted to unsigned integer.\n\nC++: "
            "genesis::utils::text::is_convertible_to_unsigned_integer(const std::string &) --> "
            "bool",
            pybind11::arg( "str" )
        );
}
