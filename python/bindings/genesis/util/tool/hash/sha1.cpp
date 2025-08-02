#include <array>
#include <genesis/util/io/base_input_source.hpp>
#include <genesis/util/tool/hash/sha1.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <streambuf>
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

void bind_genesis_util_tool_hash_sha1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::SHA1 file:genesis/util/tool/hash/sha1.hpp line:64
        pybind11::class_<genesis::util::SHA1, std::shared_ptr<genesis::util::SHA1>> cl(
            M( "genesis::util" ),
            "SHA1",
            "Calculate SHA1 hashes for strings and files.\n\n After creating an object of this "
            "type, call update() with your input data as argument, as often\n as needed. Then, "
            "call final_hex() or final_digest() to obtain the hash and reset the object\n for "
            "reuse.\n\n If you simply need the hash for a string or content of a file, use "
            "read_hex() or\n read_digest(), which are static shortcuts for the above using an "
            "input source.\n Use functions such as genesis::util::io::from_file() and "
            "genesis::util::io::from_string() to conveniently\n get an input source that can be "
            "used here.\n\n The implementation is based on "
            "[https://github.com/vog/sha1](https://github.com/vog/sha1),\n which is 100% Public "
            "Domain, see also\n \n\n"
        );
        // function-signature: genesis::util::SHA1::SHA1()() file:genesis/util/tool/hash/sha1.hpp
        // line:93
        cl.def( pybind11::init( []() { return new genesis::util::SHA1(); } ) );
        // function-signature: genesis::util::SHA1::SHA1(const class genesis::util::SHA1 &)(const
        // class genesis::util::SHA1 &) file:genesis/util/tool/hash/sha1.hpp line:96
        cl.def( pybind11::init( []( genesis::util::SHA1 const& o ) {
            return new genesis::util::SHA1( o );
        } ) );
        // function-signature: class genesis::util::SHA1 & genesis::util::SHA1::operator=(const
        // class genesis::util::SHA1 &)(const class genesis::util::SHA1 &)
        // file:genesis/util/tool/hash/sha1.hpp line:99
        cl.def(
            "assign",
            ( class genesis::util::SHA1 &
              (genesis::util::SHA1::*)(const class genesis::util::SHA1&)) &
                genesis::util::SHA1::operator=,
            "C++: genesis::util::SHA1::operator=(const class genesis::util::SHA1 &) --> class "
            "genesis::util::SHA1 &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: std::string genesis::util::SHA1::read_hex(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>) file:genesis/util/tool/hash/sha1.hpp line:109
        cl.def_static(
            "read_hex",
            ( std::string( * )( class std::shared_ptr<class genesis::util::io::BaseInputSource> )
            ) & genesis::util::SHA1::read_hex,
            "Calculate the checksum for the content of an input source.\n\nC++: "
            "genesis::util::SHA1::read_hex(class std::shared_ptr<class "
            "genesis::util::io::BaseInputSource>) --> std::string",
            pybind11::arg( "source" )
        );
        // function-signature: struct std::array<unsigned int, 5>
        // genesis::util::SHA1::read_digest(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>) file:genesis/util/tool/hash/sha1.hpp line:114
        cl.def_static(
            "read_digest",
            ( struct std::array<unsigned int, 5>( * )( class std::shared_ptr<
                                                       class genesis::util::io::BaseInputSource> )
            ) & genesis::util::SHA1::read_digest,
            "Calculate the hash digest for the content of an input source.\n\nC++: "
            "genesis::util::SHA1::read_digest(class std::shared_ptr<class "
            "genesis::util::io::BaseInputSource>) --> struct std::array<unsigned int, 5>",
            pybind11::arg( "source" )
        );
        // function-signature: std::string genesis::util::SHA1::digest_to_hex(const struct
        // std::array<unsigned int, 5> &)(const struct std::array<unsigned int, 5> &)
        // file:genesis/util/tool/hash/sha1.hpp line:116
        cl.def_static(
            "digest_to_hex",
            ( std::string( * )( const struct std::array<unsigned int, 5>& ) ) &
                genesis::util::SHA1::digest_to_hex,
            "C++: genesis::util::SHA1::digest_to_hex(const struct std::array<unsigned int, 5> &) "
            "--> std::string",
            pybind11::arg( "digest" )
        );
        // function-signature: struct std::array<unsigned int, 5>
        // genesis::util::SHA1::hex_to_digest(const std::string &)(const std::string &)
        // file:genesis/util/tool/hash/sha1.hpp line:117
        cl.def_static(
            "hex_to_digest",
            ( struct std::array<unsigned int, 5>( * )( const std::string& ) ) &
                genesis::util::SHA1::hex_to_digest,
            "C++: genesis::util::SHA1::hex_to_digest(const std::string &) --> struct "
            "std::array<unsigned int, 5>",
            pybind11::arg( "hex" )
        );
        // function-signature: void genesis::util::SHA1::clear()()
        // file:genesis/util/tool/hash/sha1.hpp line:126
        cl.def(
            "clear",
            ( void( genesis::util::SHA1::* )() ) & genesis::util::SHA1::clear,
            "Reset to initial state, that is, delete any intermediate input from update() "
            "calls.\n\nC++: genesis::util::SHA1::clear() --> void"
        );
        // function-signature: void genesis::util::SHA1::update(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>) file:genesis/util/tool/hash/sha1.hpp line:128
        cl.def(
            "update",
            ( void( genesis::util::SHA1::* )( class std::shared_ptr<
                                              class genesis::util::io::BaseInputSource> ) ) &
                genesis::util::SHA1::update,
            "C++: genesis::util::SHA1::update(class std::shared_ptr<class "
            "genesis::util::io::BaseInputSource>) --> void",
            pybind11::arg( "source" )
        );
        // function-signature: void genesis::util::SHA1::update(const std::string &)(const
        // std::string &) file:genesis/util/tool/hash/sha1.hpp line:133
        cl.def(
            "update",
            ( void( genesis::util::SHA1::* )( const std::string& ) ) & genesis::util::SHA1::update,
            "Add the contents of a string to the hash digest.\n\nC++: "
            "genesis::util::SHA1::update(const std::string &) --> void",
            pybind11::arg( "s" )
        );
        // function-signature: std::string genesis::util::SHA1::final_hex()()
        // file:genesis/util/tool/hash/sha1.hpp line:143
        cl.def(
            "final_hex",
            ( std::string( genesis::util::SHA1::* )() ) & genesis::util::SHA1::final_hex,
            "Finish the calculation, prepare the object for next use, and return the hash.\n\nC++: "
            "genesis::util::SHA1::final_hex() --> std::string"
        );
        // function-signature: struct std::array<unsigned int, 5>
        // genesis::util::SHA1::final_digest()() file:genesis/util/tool/hash/sha1.hpp line:148
        cl.def(
            "final_digest",
            ( struct std::array<unsigned int, 5>( genesis::util::SHA1::* )() ) &
                genesis::util::SHA1::final_digest,
            "Finish the calculation, prepare the object for next use, and return the "
            "digest.\n\nC++: genesis::util::SHA1::final_digest() --> struct std::array<unsigned "
            "int, 5>"
        );
    }
}
