#include <array>
#include <genesis/util/io/base_input_source.hpp>
#include <genesis/util/tool/geodesy/function.hpp>
#include <genesis/util/tool/geodesy/geo_coordinate.hpp>
#include <genesis/util/tool/hash/function.hpp>
#include <genesis/util/tool/hash/md5.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
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

void bind_genesis_util_tool_geodesy_geo_coordinate(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::GeoCoordinate file:genesis/util/tool/geodesy/geo_coordinate.hpp line:46
        pybind11::
            class_<genesis::util::GeoCoordinate, std::shared_ptr<genesis::util::GeoCoordinate>>
                cl( M( "genesis::util" ), "GeoCoordinate", "Geographical coordinates in degrees." );
        // function-signature: genesis::util::GeoCoordinate::GeoCoordinate()()
        // file:genesis/util/tool/geodesy/geo_coordinate.hpp line:54
        cl.def( pybind11::init( []() { return new genesis::util::GeoCoordinate(); } ) );
        // function-signature: genesis::util::GeoCoordinate::GeoCoordinate(double, double)(double,
        // double) file:genesis/util/tool/geodesy/geo_coordinate.hpp line:56
        cl.def( pybind11::init<double, double>(), pybind11::arg( "lat" ), pybind11::arg( "lon" ) );

        // function-signature: genesis::util::GeoCoordinate::GeoCoordinate(const class
        // genesis::util::GeoCoordinate &)(const class genesis::util::GeoCoordinate &)
        // file:genesis/util/tool/geodesy/geo_coordinate.hpp line:64
        cl.def( pybind11::init( []( genesis::util::GeoCoordinate const& o ) {
            return new genesis::util::GeoCoordinate( o );
        } ) );
        // function-signature: class genesis::util::GeoCoordinate &
        // genesis::util::GeoCoordinate::operator=(const class genesis::util::GeoCoordinate &)(const
        // class genesis::util::GeoCoordinate &) file:genesis/util/tool/geodesy/geo_coordinate.hpp
        // line:67
        cl.def(
            "assign",
            ( class genesis::util::GeoCoordinate &
              (genesis::util::GeoCoordinate::*)(const class genesis::util::GeoCoordinate&)) &
                genesis::util::GeoCoordinate::operator=,
            "C++: genesis::util::GeoCoordinate::operator=(const class genesis::util::GeoCoordinate "
            "&) --> class genesis::util::GeoCoordinate &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double genesis::util::GeoCoordinate::latitude() const()
        // file:genesis/util/tool/geodesy/geo_coordinate.hpp line:77
        cl.def(
            "latitude",
            ( double( genesis::util::GeoCoordinate::* )() const ) &
                genesis::util::GeoCoordinate::latitude,
            "Latitude, in range `[ -90.0, 90.0 ]`.\n\nC++: "
            "genesis::util::GeoCoordinate::latitude() const --> double"
        );
        // function-signature: double genesis::util::GeoCoordinate::longitude() const()
        // file:genesis/util/tool/geodesy/geo_coordinate.hpp line:85
        cl.def(
            "longitude",
            ( double( genesis::util::GeoCoordinate::* )() const ) &
                genesis::util::GeoCoordinate::longitude,
            "Longitude, in range `[ -180.0, 180.0 ]`.\n\nC++: "
            "genesis::util::GeoCoordinate::longitude() const --> double"
        );
        // function-signature: class genesis::util::GeoCoordinate &
        // genesis::util::GeoCoordinate::latitude(double)(double)
        // file:genesis/util/tool/geodesy/geo_coordinate.hpp line:93
        cl.def(
            "latitude",
            ( class genesis::util::GeoCoordinate & (genesis::util::GeoCoordinate::*)(double)) &
                genesis::util::GeoCoordinate::latitude,
            "Latitude, in range `[ -90.0, 90.0 ]`.\n\nC++: "
            "genesis::util::GeoCoordinate::latitude(double) --> class genesis::util::GeoCoordinate "
            "&",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::GeoCoordinate &
        // genesis::util::GeoCoordinate::longitude(double)(double)
        // file:genesis/util/tool/geodesy/geo_coordinate.hpp line:105
        cl.def(
            "longitude",
            ( class genesis::util::GeoCoordinate & (genesis::util::GeoCoordinate::*)(double)) &
                genesis::util::GeoCoordinate::longitude,
            "Longitude, in range `[ -180.0, 180.0 ]`.\n\nC++: "
            "genesis::util::GeoCoordinate::longitude(double) --> class "
            "genesis::util::GeoCoordinate &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );

        cl.def( "__str__", []( genesis::util::GeoCoordinate const& o ) -> std::string {
            std::ostringstream s;
            using namespace genesis::util;
            s << o;
            return s.str();
        } );
    }
    // genesis::util::sanitize_geo_coordinate(const std::string &, bool)
    // file:genesis/util/tool/geodesy/function.hpp line:68 function-signature: std::string
    // genesis::util::sanitize_geo_coordinate(const std::string &, bool)(const std::string &, bool)
    // file:genesis/util/tool/geodesy/function.hpp line:68
    M( "genesis::util" )
        .def(
            "sanitize_geo_coordinate",
            []( const std::string& a0 ) -> std::string {
                return genesis::util::sanitize_geo_coordinate( a0 );
            },
            "",
            pybind11::arg( "coordinate" )
        );
    M( "genesis::util" )
        .def(
            "sanitize_geo_coordinate",
            ( std::string( * )( const std::string&, bool ) ) &
                genesis::util::sanitize_geo_coordinate,
            "Replace non-ascii symbols used in geographic coordinates by their ascii "
            "equivalents.\n\n The function replaces symbols such as degrees, primes, double primes "
            "by simple letters ('dms')\n that serve the same function.\n\n Furthermore, if  is "
            "`true` (default), the function assumes that the given\n coordinate contains two "
            "components (North/South and East/West). In that case, if there is exaclty\n one comma "
            "in the input, it is replaced by a slash. This is meant to avoid ambiguity in "
            "parsing\n of a coordinate that uses commas as decimal separator.\n Hence, when "
            "sanitizing a coordinate with only one component,  needs to be set\n to `false` in "
            "order to avoid replacing a single decimal separator comma by a slash.\n\nC++: "
            "genesis::util::sanitize_geo_coordinate(const std::string &, bool) --> std::string",
            pybind11::arg( "coordinate" ),
            pybind11::arg( "two_components" )
        );

    // genesis::util::convert_geo_coordinate(const std::string &, const std::string &)
    // file:genesis/util/tool/geodesy/function.hpp line:76 function-signature: class
    // genesis::util::GeoCoordinate genesis::util::convert_geo_coordinate(const std::string &, const
    // std::string &)(const std::string &, const std::string &)
    // file:genesis/util/tool/geodesy/function.hpp line:76
    M( "genesis::util" )
        .def(
            "convert_geo_coordinate",
            ( class genesis::util::GeoCoordinate( * )( const std::string&, const std::string& ) ) &
                genesis::util::convert_geo_coordinate,
            "Parse strings of geographic coordinates.\n\n This is the same as "
            "convert_geo_coordinate( std::string const& ), with the only different being\n that "
            "the two components (NW and EW) are given separately. See there for details.\n\nC++: "
            "genesis::util::convert_geo_coordinate(const std::string &, const std::string &) --> "
            "class genesis::util::GeoCoordinate",
            pybind11::arg( "latitude" ),
            pybind11::arg( "longitude" )
        );

    // genesis::util::convert_geo_coordinate(const std::string &)
    // file:genesis/util/tool/geodesy/function.hpp line:111 function-signature: class
    // genesis::util::GeoCoordinate genesis::util::convert_geo_coordinate(const std::string &)(const
    // std::string &) file:genesis/util/tool/geodesy/function.hpp line:111
    M( "genesis::util" )
        .def(
            "convert_geo_coordinate",
            ( class genesis::util::GeoCoordinate( * )( const std::string& ) ) &
                genesis::util::convert_geo_coordinate,
            "Parse a string of geographic coordinates.\n\n The function accepts many different "
            "formats for geographical coordinates.\n Some exemplary valid cases:\n\n   * "
            "`50d4m17.698N 14d24m2.826E`\n   * `40:26:46N,79:56:55W`\n   * `40:26:46.302N "
            "179:56:55.903W`\n   * `49°59'56.948\"N, 15°48'22.989\"E`\n   * `50d4m17.698N "
            "14d24m2.826E`\n   * `49.9991522N, 150.8063858E`\n   * `40°26′47″N 79°58′36″W`\n   * "
            "`40d 26′ 47″ N 79d 58′ 36″ W`\n   * `40.446195N 79.948862W`\n   * `40,446195° "
            "79,948862°`\n   * `40° 26.7717 / -79° 56.93172`\n   * `40.446195, -79.948862`\n   * N "
            "49° 59.94913', E 15° 48.38315'\n\n The hemispheres can be given as `[NS]` for the "
            "first component and `[EW]` for the second,\n and can either preceed or follow the "
            "numeric values.\n Degrees, Minutes and Seconds can be separated by the letters "
            "`[dms]`, by degree (`°`),\n prime (` ′ `), single quotation mark (`'`), double prime "
            "(`″`), double quoation mark (`\"`),\n or by colons (`:`), respectively.\n The two "
            "components (NS and EW) can be separated by comma (`,`) or slash (`/`).\n\n There are "
            "some caveats: The hemisphere strings (`NESW`) have to be in capitial letters,\n while "
            "the letters used for degrees, minute and seconds (`dms`) have to be lower case.\n "
            "This constraint avoids ambiguity between seconds (`s`) and south (`S`).\n Also, "
            "either the decimal separator can be a comma, or the separator between components,\n "
            "but not both. This again avoids ambiguity while parsing.\n\nC++: "
            "genesis::util::convert_geo_coordinate(const std::string &) --> class "
            "genesis::util::GeoCoordinate",
            pybind11::arg( "coordinate" )
        );

    // genesis::util::geo_distance(const class genesis::util::GeoCoordinate &, const class
    // genesis::util::GeoCoordinate &) file:genesis/util/tool/geodesy/function.hpp line:124
    // function-signature: double genesis::util::geo_distance(const class
    // genesis::util::GeoCoordinate &, const class genesis::util::GeoCoordinate &)(const class
    // genesis::util::GeoCoordinate &, const class genesis::util::GeoCoordinate &)
    // file:genesis/util/tool/geodesy/function.hpp line:124
    M( "genesis::util" )
        .def(
            "geo_distance",
            ( double ( * )( const class genesis::util::GeoCoordinate&, const class genesis::util::GeoCoordinate& )
            ) & genesis::util::geo_distance,
            "Calculate the distance (in km) between two points on Earth.\n\n Uses the [Haversine "
            "formula](https://en.wikipedia.org/wiki/Haversine_formula).\n See \n\n\n\n\nC++: "
            "genesis::util::geo_distance(const class genesis::util::GeoCoordinate &, const class "
            "genesis::util::GeoCoordinate &) --> double",
            pybind11::arg( "c1" ),
            pybind11::arg( "c2" )
        );

    // genesis::util::HashingFunctions file:genesis/util/tool/hash/function.hpp line:53
    pybind11::enum_<genesis::util::HashingFunctions>(
        M( "genesis::util" ),
        "HashingFunctions",
        "List of the currently implemented hashing functions.\n\n This is useful in order to "
        "select the used hashing function at runtime for some algorithms.\n\n \n hash_hex()"
    )
        .value( "kMD5", genesis::util::HashingFunctions::kMD5 )
        .value( "kSHA1", genesis::util::HashingFunctions::kSHA1 )
        .value( "kSHA256", genesis::util::HashingFunctions::kSHA256 );

    ;

    // genesis::util::hash_hex(class std::shared_ptr<class genesis::util::io::BaseInputSource>, enum
    // genesis::util::HashingFunctions) file:genesis/util/tool/hash/function.hpp line:81
    // function-signature: std::string genesis::util::hash_hex(class std::shared_ptr<class
    // genesis::util::io::BaseInputSource>, enum genesis::util::HashingFunctions)(class
    // std::shared_ptr<class genesis::util::io::BaseInputSource>, enum
    // genesis::util::HashingFunctions) file:genesis/util/tool/hash/function.hpp line:81
    M( "genesis::util" )
        .def(
            "hash_hex",
            ( std::string( * )(
                class std::shared_ptr<class genesis::util::io::BaseInputSource>,
                enum genesis::util::HashingFunctions
            ) ) &
                genesis::util::hash_hex,
            "Calculate the hash of an input source, using a given hashing function, and return its "
            "hex\n representation as a string.\n\n See ::HashingFunctions for the list of "
            "available hashing functions.\n\nC++: genesis::util::hash_hex(class "
            "std::shared_ptr<class genesis::util::io::BaseInputSource>, enum "
            "genesis::util::HashingFunctions) --> std::string",
            pybind11::arg( "source" ),
            pybind11::arg( "hash_fct" )
        );

    { // genesis::util::MD5 file:genesis/util/tool/hash/md5.hpp line:88
        pybind11::class_<genesis::util::MD5, std::shared_ptr<genesis::util::MD5>> cl(
            M( "genesis::util" ),
            "MD5",
            "Calculate MD5 hashes for strings and files.\n\n After creating an object of this "
            "type, call update() with your input data as argument, as often\n as needed. Then, "
            "call final_hex() or final_digest() to obtain the hash as either a hex string\n or a "
            "DigestType object and reset the object for reuse.\n\n If you simply need the hash for "
            "a string or content of a file, use read_hex() or\n read_digest(), which are static "
            "shortcuts for the above using an input source.\n Use functions such as "
            "genesis::util::io::from_file() and genesis::util::io::from_string() to conveniently\n "
            "get an input source that can be used here.\n\n The implementation is based on "
            "http://www.zedwood.com/article/cpp-md5-function,\n which itself was converted to C++ "
            "class by Frank Thilo (thilo-ag.org) for bzflag\n (http://www.bzflag.org), and is "
            "based on the reference implementation of `RFC 1321`:\n\n \n\n > Copyright (C) 1991-2, "
            "RSA Data Security, Inc. Created 1991. All rights reserved.\n >\n > License to copy "
            "and use this software is granted provided that it\n > is identified as the \"RSA Data "
            "Security, Inc. MD5 Message-Digest\n > Algorithm\" in all material mentioning or "
            "referencing this software\n > or this function.\n >\n > License is also granted to "
            "make and use derivative works provided\n > that such works are identified as "
            "\"derived from the RSA Data\n > Security, Inc. MD5 Message-Digest Algorithm\" in all "
            "material\n > mentioning or referencing the derived work.\n >\n > RSA Data Security, "
            "Inc. makes no representations concerning either\n > the merchantability of this "
            "software or the suitability of this\n > software for any particular purpose. It is "
            "provided \"as is\"\n > without express or implied warranty of any kind.\n >\n > These "
            "notices must be retained in any copies of any part of this\n > documentation and/or "
            "software.\n \n\n\n See also \n"
        );
        // function-signature: genesis::util::MD5::MD5()() file:genesis/util/tool/hash/md5.hpp
        // line:117
        cl.def( pybind11::init( []() { return new genesis::util::MD5(); } ) );
        // function-signature: genesis::util::MD5::MD5(const class genesis::util::MD5 &)(const class
        // genesis::util::MD5 &) file:genesis/util/tool/hash/md5.hpp line:120
        cl.def( pybind11::init( []( genesis::util::MD5 const& o ) {
            return new genesis::util::MD5( o );
        } ) );
        // function-signature: class genesis::util::MD5 & genesis::util::MD5::operator=(const class
        // genesis::util::MD5 &)(const class genesis::util::MD5 &)
        // file:genesis/util/tool/hash/md5.hpp line:123
        cl.def(
            "assign",
            ( class genesis::util::MD5 & (genesis::util::MD5::*)(const class genesis::util::MD5&)) &
                genesis::util::MD5::operator=,
            "C++: genesis::util::MD5::operator=(const class genesis::util::MD5 &) --> class "
            "genesis::util::MD5 &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: std::string genesis::util::MD5::read_hex(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>) file:genesis/util/tool/hash/md5.hpp line:133
        cl.def_static(
            "read_hex",
            ( std::string( * )( class std::shared_ptr<class genesis::util::io::BaseInputSource> )
            ) & genesis::util::MD5::read_hex,
            "Calculate the checksum for the content of an input source.\n\nC++: "
            "genesis::util::MD5::read_hex(class std::shared_ptr<class "
            "genesis::util::io::BaseInputSource>) --> std::string",
            pybind11::arg( "source" )
        );
        // function-signature: struct std::array<unsigned char, 16>
        // genesis::util::MD5::read_digest(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>) file:genesis/util/tool/hash/md5.hpp line:138
        cl.def_static(
            "read_digest",
            ( struct std::array<unsigned char, 16>( * )( class std::shared_ptr<
                                                         class genesis::util::io::BaseInputSource> )
            ) & genesis::util::MD5::read_digest,
            "Calculate the hash digest for the content of an input source.\n\nC++: "
            "genesis::util::MD5::read_digest(class std::shared_ptr<class "
            "genesis::util::io::BaseInputSource>) --> struct std::array<unsigned char, 16>",
            pybind11::arg( "source" )
        );
        // function-signature: std::string genesis::util::MD5::digest_to_hex(const struct
        // std::array<unsigned char, 16> &)(const struct std::array<unsigned char, 16> &)
        // file:genesis/util/tool/hash/md5.hpp line:140
        cl.def_static(
            "digest_to_hex",
            ( std::string( * )( const struct std::array<unsigned char, 16>& ) ) &
                genesis::util::MD5::digest_to_hex,
            "C++: genesis::util::MD5::digest_to_hex(const struct std::array<unsigned char, 16> &) "
            "--> std::string",
            pybind11::arg( "digest" )
        );
        // function-signature: struct std::array<unsigned char, 16>
        // genesis::util::MD5::hex_to_digest(const std::string &)(const std::string &)
        // file:genesis/util/tool/hash/md5.hpp line:141
        cl.def_static(
            "hex_to_digest",
            ( struct std::array<unsigned char, 16>( * )( const std::string& ) ) &
                genesis::util::MD5::hex_to_digest,
            "C++: genesis::util::MD5::hex_to_digest(const std::string &) --> struct "
            "std::array<unsigned char, 16>",
            pybind11::arg( "hex" )
        );
        // function-signature: void genesis::util::MD5::clear()()
        // file:genesis/util/tool/hash/md5.hpp line:150
        cl.def(
            "clear",
            ( void( genesis::util::MD5::* )() ) & genesis::util::MD5::clear,
            "Reset to initial state, that is, delete any intermediate input from update() "
            "calls.\n\nC++: genesis::util::MD5::clear() --> void"
        );
        // function-signature: void genesis::util::MD5::update(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>)(class std::shared_ptr<class
        // genesis::util::io::BaseInputSource>) file:genesis/util/tool/hash/md5.hpp line:152
        cl.def(
            "update",
            ( void( genesis::util::MD5::* )( class std::shared_ptr<
                                             class genesis::util::io::BaseInputSource> ) ) &
                genesis::util::MD5::update,
            "C++: genesis::util::MD5::update(class std::shared_ptr<class "
            "genesis::util::io::BaseInputSource>) --> void",
            pybind11::arg( "source" )
        );
        // function-signature: void genesis::util::MD5::update(const std::string &)(const
        // std::string &) file:genesis/util/tool/hash/md5.hpp line:153
        cl.def(
            "update",
            ( void( genesis::util::MD5::* )( const std::string& ) ) & genesis::util::MD5::update,
            "C++: genesis::util::MD5::update(const std::string &) --> void",
            pybind11::arg( "s" )
        );
        // function-signature: void genesis::util::MD5::update(const char *, unsigned int)(const
        // char *, unsigned int) file:genesis/util/tool/hash/md5.hpp line:155
        cl.def(
            "update",
            ( void( genesis::util::MD5::* )( const char*, unsigned int ) ) &
                genesis::util::MD5::update,
            "C++: genesis::util::MD5::update(const char *, unsigned int) --> void",
            pybind11::arg( "input" ),
            pybind11::arg( "length" )
        );
        // function-signature: std::string genesis::util::MD5::final_hex()()
        // file:genesis/util/tool/hash/md5.hpp line:160
        cl.def(
            "final_hex",
            ( std::string( genesis::util::MD5::* )() ) & genesis::util::MD5::final_hex,
            "Finish the calculation, prepare the object for next use, and return the hash.\n\nC++: "
            "genesis::util::MD5::final_hex() --> std::string"
        );
        // function-signature: struct std::array<unsigned char, 16>
        // genesis::util::MD5::final_digest()() file:genesis/util/tool/hash/md5.hpp line:165
        cl.def(
            "final_digest",
            ( struct std::array<unsigned char, 16>( genesis::util::MD5::* )() ) &
                genesis::util::MD5::final_digest,
            "Finish the calculation, prepare the object for next use, and return the "
            "digest.\n\nC++: genesis::util::MD5::final_digest() --> struct std::array<unsigned "
            "char, 16>"
        );
    }
}
