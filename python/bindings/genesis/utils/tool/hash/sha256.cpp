#include <array>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/tool/hash/sha256.hpp>
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

void bind_genesis_utils_tool_hash_sha256(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::SHA256 file:genesis/utils/tool/hash/sha256.hpp line:102
        pybind11::class_<genesis::utils::SHA256, std::shared_ptr<genesis::utils::SHA256>> cl(
            M( "genesis::utils" ),
            "SHA256",
            "Calculate SHA256 hashes for strings and files.\n\n After creating an object of this "
            "type, call update() with your input data as argument, as often\n as needed. Then, "
            "call final_hex() or final_digest() to obtain the hash and reset the object\n for "
            "reuse.\n\n If you simply need the hash for a string or content of a file, use "
            "read_hex() or\n read_digest(), which are static shortcuts for the above using an "
            "input source.\n Use functions such as genesis::utils::io::from_file() and "
            "genesis::utils::io::from_string() to conveniently\n get an input source that can be "
            "used here.\n\n The implementation is based on "
            "http://www.zedwood.com/article/cpp-sha256-function,\n which itself is based on "
            "Olivier Gay's version, and was published with a Modified BSD License:\n\n \n\n > "
            "Updated to C++, zedwood.com 2012\n > Based on Olivier Gay's version\n > See Modified "
            "BSD License below:\n >\n > FIPS 180-2 SHA-224/256/384/512 implementation\n > Issue "
            "date:  04/30/2005\n > http://www.ouah.org/ogay/sha2/\n >\n > Copyright (C) 2005, 2007 "
            "Olivier Gay <olivier.gay\n > All rights reserved.\n >\n > Redistribution and use in "
            "source and binary forms, with or without\n > modification, are permitted provided "
            "that the following conditions\n > are met:\n > 1. Redistributions of source code must "
            "retain the above copyright\n >    notice, this list of conditions and the following "
            "disclaimer.\n > 2. Redistributions in binary form must reproduce the above "
            "copyright\n >    notice, this list of conditions and the following disclaimer in "
            "the\n >    documentation and/or other materials provided with the distribution.\n > "
            "3. Neither the name of the project nor the names of its contributors\n >    may be "
            "used to endorse or promote products derived from this software\n >    without "
            "specific prior written permission.\n >\n > THIS SOFTWARE IS PROVIDED BY THE PROJECT "
            "AND CONTRIBUTORS ``AS IS'' AND\n > ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT "
            "NOT LIMITED TO, THE\n > IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
            "PARTICULAR PURPOSE\n > ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS "
            "BE LIABLE\n > FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR "
            "CONSEQUENTIAL\n > DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE "
            "GOODS\n > OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)\n > "
            "HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n > "
            "LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY\n > OUT OF "
            "THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF\n > SUCH DAMAGE.\n "
            "\n\n\n See also \n"
        );
        // function-signature: genesis::utils::SHA256::SHA256()()
        // file:genesis/utils/tool/hash/sha256.hpp line:130
        cl.def( pybind11::init( []() { return new genesis::utils::SHA256(); } ) );
        // function-signature: genesis::utils::SHA256::SHA256(const class genesis::utils::SHA256
        // &)(const class genesis::utils::SHA256 &) file:genesis/utils/tool/hash/sha256.hpp line:133
        cl.def( pybind11::init( []( genesis::utils::SHA256 const& o ) {
            return new genesis::utils::SHA256( o );
        } ) );
        // function-signature: class genesis::utils::SHA256 &
        // genesis::utils::SHA256::operator=(const class genesis::utils::SHA256 &)(const class
        // genesis::utils::SHA256 &) file:genesis/utils/tool/hash/sha256.hpp line:136
        cl.def(
            "assign",
            ( class genesis::utils::SHA256 &
              (genesis::utils::SHA256::*)(const class genesis::utils::SHA256&)) &
                genesis::utils::SHA256::operator=,
            "C++: genesis::utils::SHA256::operator=(const class genesis::utils::SHA256 &) --> "
            "class genesis::utils::SHA256 &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: std::string genesis::utils::SHA256::read_hex(class
        // std::shared_ptr<class genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::utils::io::BaseInputSource>) file:genesis/utils/tool/hash/sha256.hpp line:146
        cl.def_static(
            "read_hex",
            ( std::string( * )( class std::shared_ptr<class genesis::utils::io::BaseInputSource> )
            ) & genesis::utils::SHA256::read_hex,
            "Calculate the checksum for the content of an input source.\n\nC++: "
            "genesis::utils::SHA256::read_hex(class std::shared_ptr<class "
            "genesis::utils::io::BaseInputSource>) --> std::string",
            pybind11::arg( "source" )
        );
        // function-signature: struct std::array<unsigned int, 8>
        // genesis::utils::SHA256::read_digest(class std::shared_ptr<class
        // genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::utils::io::BaseInputSource>) file:genesis/utils/tool/hash/sha256.hpp line:151
        cl.def_static(
            "read_digest",
            ( struct std::array<unsigned int, 8>( * )( class std::shared_ptr<
                                                       class genesis::utils::io::BaseInputSource> )
            ) & genesis::utils::SHA256::read_digest,
            "Calculate the hash digest for the content of an input source.\n\nC++: "
            "genesis::utils::SHA256::read_digest(class std::shared_ptr<class "
            "genesis::utils::io::BaseInputSource>) --> struct std::array<unsigned int, 8>",
            pybind11::arg( "source" )
        );
        // function-signature: std::string genesis::utils::SHA256::digest_to_hex(const struct
        // std::array<unsigned int, 8> &)(const struct std::array<unsigned int, 8> &)
        // file:genesis/utils/tool/hash/sha256.hpp line:153
        cl.def_static(
            "digest_to_hex",
            ( std::string( * )( const struct std::array<unsigned int, 8>& ) ) &
                genesis::utils::SHA256::digest_to_hex,
            "C++: genesis::utils::SHA256::digest_to_hex(const struct std::array<unsigned int, 8> "
            "&) --> std::string",
            pybind11::arg( "digest" )
        );
        // function-signature: struct std::array<unsigned int, 8>
        // genesis::utils::SHA256::hex_to_digest(const std::string &)(const std::string &)
        // file:genesis/utils/tool/hash/sha256.hpp line:154
        cl.def_static(
            "hex_to_digest",
            ( struct std::array<unsigned int, 8>( * )( const std::string& ) ) &
                genesis::utils::SHA256::hex_to_digest,
            "C++: genesis::utils::SHA256::hex_to_digest(const std::string &) --> struct "
            "std::array<unsigned int, 8>",
            pybind11::arg( "hex" )
        );
        // function-signature: void genesis::utils::SHA256::clear()()
        // file:genesis/utils/tool/hash/sha256.hpp line:163
        cl.def(
            "clear",
            ( void( genesis::utils::SHA256::* )() ) & genesis::utils::SHA256::clear,
            "Reset to initial state, that is, delete any intermediate input from update() "
            "calls.\n\nC++: genesis::utils::SHA256::clear() --> void"
        );
        // function-signature: void genesis::utils::SHA256::update(class std::shared_ptr<class
        // genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::utils::io::BaseInputSource>) file:genesis/utils/tool/hash/sha256.hpp line:165
        cl.def(
            "update",
            ( void( genesis::utils::SHA256::* )( class std::shared_ptr<
                                                 class genesis::utils::io::BaseInputSource> ) ) &
                genesis::utils::SHA256::update,
            "C++: genesis::utils::SHA256::update(class std::shared_ptr<class "
            "genesis::utils::io::BaseInputSource>) --> void",
            pybind11::arg( "source" )
        );
        // function-signature: void genesis::utils::SHA256::update(const std::string &)(const
        // std::string &) file:genesis/utils/tool/hash/sha256.hpp line:166
        cl.def(
            "update",
            ( void( genesis::utils::SHA256::* )( const std::string& ) ) &
                genesis::utils::SHA256::update,
            "C++: genesis::utils::SHA256::update(const std::string &) --> void",
            pybind11::arg( "s" )
        );
        // function-signature: void genesis::utils::SHA256::update(const char *, unsigned
        // long)(const char *, unsigned long) file:genesis/utils/tool/hash/sha256.hpp line:168
        cl.def(
            "update",
            ( void( genesis::utils::SHA256::* )( const char*, unsigned long ) ) &
                genesis::utils::SHA256::update,
            "C++: genesis::utils::SHA256::update(const char *, unsigned long) --> void",
            pybind11::arg( "input" ),
            pybind11::arg( "length" )
        );
        // function-signature: std::string genesis::utils::SHA256::final_hex()()
        // file:genesis/utils/tool/hash/sha256.hpp line:173
        cl.def(
            "final_hex",
            ( std::string( genesis::utils::SHA256::* )() ) & genesis::utils::SHA256::final_hex,
            "Finish the calculation, prepare the object for next use, and return the hash.\n\nC++: "
            "genesis::utils::SHA256::final_hex() --> std::string"
        );
        // function-signature: struct std::array<unsigned int, 8>
        // genesis::utils::SHA256::final_digest()() file:genesis/utils/tool/hash/sha256.hpp line:178
        cl.def(
            "final_digest",
            ( struct std::array<unsigned int, 8>( genesis::utils::SHA256::* )() ) &
                genesis::utils::SHA256::final_digest,
            "Finish the calculation, prepare the object for next use, and return the "
            "digest.\n\nC++: genesis::utils::SHA256::final_digest() --> struct std::array<unsigned "
            "int, 8>"
        );
    }
}
