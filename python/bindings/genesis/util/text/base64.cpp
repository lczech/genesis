#include <genesis/util/text/base64.hpp>
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

void bind_genesis_util_text_base64(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::text::base64_encode(const class std::vector<unsigned char> &, unsigned long)
    // file:genesis/util/text/base64.hpp line:46 function-signature: std::string
    // genesis::util::text::base64_encode(const class std::vector<unsigned char> &, unsigned
    // long)(const class std::vector<unsigned char> &, unsigned long)
    // file:genesis/util/text/base64.hpp line:46
    M( "genesis::util::text" )
        .def(
            "base64_encode",
            []( const class std::vector<unsigned char>& a0 ) -> std::string {
                return genesis::util::text::base64_encode( a0 );
            },
            "",
            pybind11::arg( "input" )
        );
    M( "genesis::util::text" )
        .def(
            "base64_encode",
            ( std::string( * )( const class std::vector<unsigned char>&, unsigned long ) ) &
                genesis::util::text::base64_encode,
            "C++: genesis::util::text::base64_encode(const class std::vector<unsigned char> &, "
            "unsigned long) --> std::string",
            pybind11::arg( "input" ),
            pybind11::arg( "line_length" )
        );

    // genesis::util::text::base64_encode(const std::string &, unsigned long)
    // file:genesis/util/text/base64.hpp line:47 function-signature: std::string
    // genesis::util::text::base64_encode(const std::string &, unsigned long)(const std::string &,
    // unsigned long) file:genesis/util/text/base64.hpp line:47
    M( "genesis::util::text" )
        .def(
            "base64_encode",
            []( const std::string& a0 ) -> std::string {
                return genesis::util::text::base64_encode( a0 );
            },
            "",
            pybind11::arg( "input" )
        );
    M( "genesis::util::text" )
        .def(
            "base64_encode",
            ( std::string( * )( const std::string&, unsigned long ) ) &
                genesis::util::text::base64_encode,
            "C++: genesis::util::text::base64_encode(const std::string &, unsigned long) --> "
            "std::string",
            pybind11::arg( "input" ),
            pybind11::arg( "line_length" )
        );

    // genesis::util::text::base64_decode_uint8(const std::string &)
    // file:genesis/util/text/base64.hpp line:49 function-signature: class std::vector<unsigned
    // char> genesis::util::text::base64_decode_uint8(const std::string &)(const std::string &)
    // file:genesis/util/text/base64.hpp line:49
    M( "genesis::util::text" )
        .def(
            "base64_decode_uint8",
            ( class std::vector<unsigned char>( * )( const std::string& ) ) &
                genesis::util::text::base64_decode_uint8,
            "C++: genesis::util::text::base64_decode_uint8(const std::string &) --> class "
            "std::vector<unsigned char>",
            pybind11::arg( "input" )
        );

    // genesis::util::text::base64_decode_string(const std::string &)
    // file:genesis/util/text/base64.hpp line:50 function-signature: std::string
    // genesis::util::text::base64_decode_string(const std::string &)(const std::string &)
    // file:genesis/util/text/base64.hpp line:50
    M( "genesis::util::text" )
        .def(
            "base64_decode_string",
            ( std::string( * )( const std::string& ) ) & genesis::util::text::base64_decode_string,
            "C++: genesis::util::text::base64_decode_string(const std::string &) --> std::string",
            pybind11::arg( "input" )
        );
}
